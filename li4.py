#!/usr/bin/env python3
"""
Linux Module to Xcode Extractor
"""

import os
import re
import shutil
import subprocess
import sys
import logging
from pathlib import Path
from typing import List, Tuple, Set
from collections import deque
from collections import defaultdict

KERNEL_URL = "https://github.com/torvalds/linux.git"
KERNEL_TAG = "master"
SOURCE_DIR = Path("linux_kernel").resolve()
DEST_DIR = Path("drm_extracted").resolve()
REQUIRED_TOOLS = ["clang", "make", "bc", "flex", "openssl", "nm"]

# ==============================================================================
# MODULE TARGETS
# ==============================================================================
TARGET_MODULES: List[Tuple[str, str]] = [
    ("drivers/gpu/drm/i915", "CONFIG_DRM_I915"),
    ("drivers/gpu/drm/xe", "CONFIG_DRM_XE"),
    ("drivers/gpu/drm/display", "CONFIG_DRM_DISPLAY_HELPER"),
    ("drivers/gpu/drm/virtio", "CONFIG_DRM_VIRTIO_GPU"),
]
# ==============================================================================

EXTRA_CONFIG_FLAGS: List[str] = [
    "CONFIG_DRM_KMS_HELPER",
    "CONFIG_DRM_I915_SELFTEST",
]

# ==============================================================================
# EXACT 1:1 MAPPING OF LINUXINCLUDE + USERINCLUDE (8 PATHS)
# ==============================================================================
LINUXINCLUDE_PREFIXES = [
    "arch/x86/include",
    "arch/x86/include/generated",
    "include",
    "arch/x86/include/uapi",
    "arch/x86/include/generated/uapi",
    "include/uapi",
    "include/generated/uapi",
]

logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)-5s] %(message)s", datefmt="%H:%M:%S")
logger = logging.getLogger(__name__)

class BuildError(Exception): pass
class DependencyError(Exception): pass

def run_cmd(cmd: List[str], cwd: Path = None, check=True, timeout=None) -> subprocess.CompletedProcess:
    logger.info(f"Executing: {' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True, timeout=timeout)
    if check and result.returncode != 0:
        logger.error(f"STDERR (last 500 chars): {result.stderr[-500:]}")
        raise subprocess.CalledProcessError(result.returncode, cmd)
    return result

def check_dependencies():
    logger.info("[1/6] Checking dependencies...")
    missing = [t for t in REQUIRED_TOOLS if shutil.which(t) is None]
    if missing: raise DependencyError(f"Missing: {', '.join(missing)}")

def prepare_kernel_source():
    logger.info("[2/6] Preparing kernel source...")
    if not SOURCE_DIR.exists():
        run_cmd(["git", "clone", "--depth", "1", "--branch", KERNEL_TAG, KERNEL_URL, str(SOURCE_DIR)], timeout=300)

def configure_kernel():
    logger.info("[3/6] Configuring kernel (Native Kconfig + Targets)...")
    DEST_DIR.mkdir(parents=True, exist_ok=True)
    run_cmd(["make", "LLVM=1", "x86_64_defconfig"], cwd=SOURCE_DIR, timeout=120)
    run_cmd(["./scripts/config", "--enable", "CONFIG_MODULES"], cwd=SOURCE_DIR)
    
    for mod_path, mod_config in TARGET_MODULES:
        run_cmd(["./scripts/config", "--enable", mod_config], cwd=SOURCE_DIR)
        
    if EXTRA_CONFIG_FLAGS:
        for cfg in EXTRA_CONFIG_FLAGS:
            run_cmd(["./scripts/config", "--enable", cfg], cwd=SOURCE_DIR)
    
    run_cmd(["make", "LLVM=1", "olddefconfig"], cwd=SOURCE_DIR, timeout=120)
    logger.info("  Kernel configured.")

def prepare_and_build():
    logger.info("[4/6] Building vmlinux to resolve all transitive symbols...")
    run_cmd(["make", "LLVM=1", "modules_prepare"], cwd=SOURCE_DIR, timeout=300)
    
    log_file = SOURCE_DIR / "gpu_build.log"
    result = subprocess.run(
        ["make", "LLVM=1", "vmlinux"],
        cwd=SOURCE_DIR,
        stdout=open(log_file, "w"),
        stderr=subprocess.STDOUT
    )
    if result.returncode != 0:
        raise BuildError(f"vmlinux build failed. Check {log_file}")
    logger.info("  Build successful.")

def copy_generated_includes():

    logger.info("  Copying include/generated folder...")
    src_generated = SOURCE_DIR / "include" / "generated"
    dst_generated = DEST_DIR / "include" / "generated"
    
    if not src_generated.is_dir():
        logger.warning("  include/generated folder not found in source, skipping.")
        return 0
    
    if dst_generated.exists():
        shutil.rmtree(dst_generated)
    
    shutil.copytree(src_generated, dst_generated)
    
    logger.info("  Copying arch/x86/include/generated folder...")
    src_generated = SOURCE_DIR / "arch" / "x86" / "include" / "generated"
    dst_generated = DEST_DIR / "arch" / "x86" / "include" / "generated"
    
    if not src_generated.is_dir():
        logger.warning("  arch/x86/include/generated folder not found in source, skipping.")
        return 0
    
    if dst_generated.exists():
        shutil.rmtree(dst_generated)
    
    shutil.copytree(src_generated, dst_generated)

    dst_generated = DEST_DIR / "include" / "linux"
    dst_generated.mkdir(parents=True, exist_ok=True)
    src_generated = SOURCE_DIR / "include" / "linux" / "compiler-version.h"
    dst_generated = DEST_DIR / "include" / "linux" / "compiler-version.h" 

    shutil.copy2(src_generated, dst_generated)



def copy_module_tree() -> int:
    logger.info("[5/6] Scanning .c files and resolving includes to proper folders...")
    
    include_regex = re.compile(r'^\s*#\s*include\s+[<"]([^>"]+)[>"]', re.MULTILINE)
    
    target_c_files = set()
    for mod_path, _ in TARGET_MODULES:
        src_base = SOURCE_DIR / mod_path
        if src_base.is_dir():
            for c_file in src_base.rglob("*.c"):
                target_c_files.add(c_file.relative_to(SOURCE_DIR))
                
    parent_dirs = set(str(Path(m[0]).parent) for m in TARGET_MODULES)
    for parent_path in parent_dirs:
        src_parent = SOURCE_DIR / parent_path
        if not src_parent.is_dir(): continue
        for c_file in src_parent.glob("*.c"):
            if c_file.with_suffix(".o").exists():
                target_c_files.add(c_file.relative_to(SOURCE_DIR))

    logger.info(f"  Found {len(target_c_files)} target .c files to process.")

    visited = set()
    to_process = deque(target_c_files)  
    copied_count = 0

    def resolve_and_copy(include_name: str, current_src_file: Path) -> bool:
        nonlocal copied_count  
        if include_name in visited:
            return False
            
        found_abs_path = None
        
        rel_to_current = (current_src_file.parent / include_name).resolve()
        if rel_to_current.is_file():
            found_abs_path = rel_to_current
            
        if not found_abs_path:
            for prefix in LINUXINCLUDE_PREFIXES:
                candidate = (SOURCE_DIR / prefix / include_name).resolve()
                if candidate.is_file():
                    found_abs_path = candidate
                    break
                    
        # If a file includes "display/foo.h", look in the module root.
        if not found_abs_path:
            for mod_path, _ in TARGET_MODULES:
                candidate = (SOURCE_DIR / mod_path / include_name).resolve()
                if candidate.is_file():
                    found_abs_path = candidate
                    break
                    
        if not found_abs_path:
            return False
            
        try:
            rel_dest = found_abs_path.relative_to(SOURCE_DIR.resolve())
        except ValueError:
            return False 
            
        visited.add(include_name)
        dst_path = DEST_DIR / rel_dest
        
        if not dst_path.exists():
            dst_path.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(found_abs_path, dst_path)
            copied_count += 1
            
        if rel_dest not in to_process:
            to_process.append(rel_dest)
            
        return True

    while to_process:
        rel_file_path = to_process.popleft() 
        
        dst_file_path = DEST_DIR / rel_file_path
        if not dst_file_path.exists():
            dst_file_path.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(SOURCE_DIR / rel_file_path, dst_file_path)
            copied_count += 1
            
        try:
            content = (SOURCE_DIR / rel_file_path).read_text(errors='ignore')
            for match in include_regex.finditer(content):
                include_name = match.group(1)
                resolve_and_copy(include_name, SOURCE_DIR / rel_file_path)
        except Exception as e:
            logger.warning(f"  Failed to scan {rel_file_path}: {e}")

    logger.info(f"  Extraction complete: {copied_count} files mapped to proper folders.")
    return copied_count


def add_extern_c():
    logger.info("  Adding extern C guards to extracted .h files...")
    added = 0
    skipped = 0

    for h in DEST_DIR.rglob("*.h"):
        try:
            content = h.read_text(errors='ignore')
            if 'extern "C"' in content:
                continue
            
            lines = content.split('\n')
            insert_after = -1
            
            for i, line in enumerate(lines):
                if re.match(r'^\s*#\s*pragma\s+once\s*$', line):
                    insert_after = i
                    break
                if re.match(r'^\s*#\s*ifndef\s+([A-Z_][A-Z0-9_]*)\s*$', line):
                    if i + 1 < len(lines) and re.match(r'^\s*#\s*define\s', lines[i+1]):
                        insert_after = i + 1
                        break
            
            if insert_after == -1:
                skipped += 1
                continue
            
            lines.insert(insert_after + 1, "")
            lines.insert(insert_after + 2, "#ifdef __cplusplus")
            lines.insert(insert_after + 3, 'extern "C" {')
            lines.insert(insert_after + 4, "#endif")
            lines.insert(insert_after + 5, "#include <linux/mydefs.h>")        

            for i in range(len(lines) - 1, -1, -1):
                if re.match(r'^\s*#\s*endif\b', lines[i]):
                    lines.insert(i, "#ifdef __cplusplus")
                    lines.insert(i + 1, "}")
                    lines.insert(i + 2, "#endif")
                    break
            
            h.write_text('\n'.join(lines))
            added += 1
            
        except Exception as e:
            logger.debug(f"  Failed to process {h}: {e}")
    
    logger.info(f"    Added extern C linkage to {added} headers (Skipped {skipped} without guards).")



def extract_xcode_flags() -> Tuple[List[str], List[str], List[str], List[str]]:
    logger.info("  Extracting Makefile flags and translating paths for Xcode...")
    
    cmd_file = None
    check_dirs = [SOURCE_DIR / m[0] for m in TARGET_MODULES]
    for mod_dir in check_dirs:
        if not mod_dir.is_dir(): continue
        cmds = list(mod_dir.rglob("*.o.cmd"))
        if cmds:
            cmd_file = cmds[0]
            break
            
    if not cmd_file:
        fallback = SOURCE_DIR / "init" / "main.o.cmd"
        cmd_file = fallback if fallback.is_file() else None

    if not cmd_file:
        logger.error("  CRITICAL: No .cmd files found!")
        return [], [], [], None

    cmd_text = cmd_file.read_text(errors='ignore').replace('\\\n', ' ')
    
    DEST_XCODE_DIR = "$(PROJECT_DIR)/drm_extracted"

    linuxinclude_flags = []
    kbuild_cflags = []
    subdir_overrides = []
    
    def make_xcode_path(makefile_path: str) -> str:
        clean = makefile_path
        if clean.startswith('./'):
            clean = clean[2:]
        return f"{DEST_XCODE_DIR}/{clean}"

    for line in cmd_text.splitlines():
        line = line.strip()
        if not (line.startswith('cmd_') or line.startswith('savedcmd_')) or ':=' not in line:
            continue
            
        parts = line.split(':=', 1)
        if len(parts) != 2: continue
            
        args = parts[1].split()
        i = 0
        while i < len(args):
            arg = args[i]
            
            if 'clang' in arg or arg == 'gcc' or arg.endswith('.c') or arg.endswith('.S') or arg == '-c':
                i += 1; continue
            if arg == '-o':
                i += 2; continue
            
            next_arg = args[i+1] if i + 1 < len(args) and not args[i+1].startswith('-') else None
            
            if arg == '-nostdinc':
                kbuild_cflags.append(arg)
                i += 1; continue
                
            if arg.startswith('-I'):
                xcode_path = make_xcode_path(arg[2:])
                linuxinclude_flags.append(xcode_path)
                i += 1; continue
                
            if arg == '-include' and next_arg:
                xcode_inc = make_xcode_path(next_arg)
                full_arg = f"-include{xcode_inc}"
                
                if 'compiler-version.h' in next_arg or 'kconfig.h' in next_arg or 'compiler_types.h' in next_arg:
                    kbuild_cflags.append(full_arg)
                else:
                    subdir_overrides.append(full_arg)
                i += 2; continue

            if arg.startswith('-D__KERNEL__') or arg.startswith('-DCONFIG_'):
                kbuild_cflags.append(f"{arg} {next_arg}" if next_arg else arg)
                i += (2 if next_arg else 1); continue

            if arg.startswith('-DKBUILD_MOD') or arg.startswith('-D__KBUILD_MODNAME'):
                subdir_overrides.append(f"{arg} {next_arg}" if next_arg else arg)
                i += (2 if next_arg else 1); continue
            if arg.startswith('-I./drivers/') or (re.match(r'^-D[A-Z0-9_]+$', arg) and not arg.startswith('-D__') and 'CONFIG' not in arg):
                subdir_overrides.append(f"{arg} {next_arg}" if next_arg else arg)
                i += (2 if next_arg else 1); continue

            if arg.startswith('-Wp,'):
                i += 1; continue
                
            kbuild_cflags.append(f"{arg} {next_arg}" if next_arg else arg)
            i += (2 if next_arg else 1)
            
        break
        
    linuxinclude_flags = list(dict.fromkeys(linuxinclude_flags))
    return kbuild_cflags, linuxinclude_flags, subdir_overrides, cmd_file


def generate_xcode_settings(kbuild_cflags: List[str], linuxinclude: List[str], subdir_overrides: List[str], cmd_file: Path):
    logger.info("  Generating Xcode settings mapped to Makefile variables...")
    
    cflags_str = "\n".join(kbuild_cflags) if kbuild_cflags else "# (None)"
    includes_str = "\n".join(linuxinclude) if linuxinclude else "# (None)"
    overrides_str = "\n".join(subdir_overrides) if subdir_overrides else "# (None)"
    
    output_text = f"""================================================================================
XCODE BUILD SETTINGS 
================================================================================
Header Search Paths
--------------------------------------------------------------------------------
{includes_str}
--------------------------------------------------------------------------------

Other C Flags
--------------------------------------------------------------------------------
-I$(PROJECT_DIR)/drm_extracted
{cflags_str}
--------------------------------------------------------------------------------

SKIPPED: Sub-Directory Makefile Overrides 
--------------------------------------------------------------------------------
{overrides_str}
--------------------------------------------------------------------------------
================================================================================
"""

    txt_path = DEST_DIR / "xcode_build_settings.txt"
    txt_path.write_text(output_text)
        
    logger.info(f"    Wrote settings to: {txt_path.name}")



def fix_relative_includes():

    logger.info("  Fixing broken/internal includes (Index-Powered)...")
    fixed_count = 0
    fallback_count = 0
    
    include_regex = re.compile(r'^\s*#\s*include\s+([<"])([^>"]+)[>"]', re.MULTILINE)
    
    module_roots = [SOURCE_DIR / Path(m[0]) for m in TARGET_MODULES]
    dest_roots = [DEST_DIR / Path(m[0]) for m in TARGET_MODULES]
    global_dest_roots = [DEST_DIR / p for p in LINUXINCLUDE_PREFIXES] + [DEST_DIR / "include" / "asm-generic"]
    global_src_roots = [SOURCE_DIR / p for p in LINUXINCLUDE_PREFIXES] + [SOURCE_DIR / "include" / "asm-generic"]

    IGNORE_SYSTEM_INCLUDES = [
    ]
    
    def is_system_header(name: str) -> bool:
        return name in IGNORE_SYSTEM_INCLUDES or any(name.startswith(p) for p in IGNORE_SYSTEM_INCLUDES)

    logger.info("    Building file index...")
    file_index = {}
    for f in DEST_DIR.rglob("*.[ch]"):
            file_index[f.name] = f

    files_processed = 0 

    for file_path in DEST_DIR.rglob("*.[ch]"):

        try:
            content = file_path.read_text(errors='ignore')
        except Exception: continue
            
        file_dir = file_path.parent
        new_content = content
        rel_file = file_path.relative_to(DEST_DIR)
        files_processed += 1
        
        def replacer(match):
            nonlocal fixed_count, fallback_count, files_processed
            
            delimiter = match.group(1)
            inc_name = match.group(2)
            
            if delimiter == '"':
                if (file_dir / inc_name).is_file():
                    return match.group(0)
                
                for root in dest_roots:
                    candidate = root / inc_name
                    if candidate.is_file():
                        rel_path = os.path.relpath(candidate, file_dir)
                        fixed_count += 1
                        return f'#include "{rel_path}"'
                
                if inc_name in file_index:
                    found_file = file_index[inc_name]
                    if any(str(found_file).startswith(str(root)) for root in dest_roots):
                        rel_path = os.path.relpath(found_file, file_dir)
                        fixed_count += 1
                        return f'#include "{rel_path}"'
                
                if not is_system_header(inc_name):
                    for i, src_root in enumerate(module_roots):
                        if (src_root / inc_name).is_file():
                            dest_cand = dest_roots[i] / inc_name
                            dest_cand.parent.mkdir(parents=True, exist_ok=True)
                            shutil.copy2(src_root / inc_name, dest_cand)
                            fallback_count += 1
                            logger.warning(f"    FALLBACK COPY: '{inc_name}' grabbed for {rel_file}")
                            rel_path = os.path.relpath(dest_cand, file_dir)
                            return f'#include "{rel_path}"'
                        
                return match.group(0)
            
            if delimiter == '<':
                for root in global_dest_roots:
                    if (root / inc_name).is_file():
                        rel_path = os.path.relpath(root / inc_name, file_dir)
                        fixed_count += 1
                        return f'#include "{rel_path}"'
                
                for root in dest_roots:
                    candidate = root / inc_name
                    if candidate.is_file():
                        rel_path = os.path.relpath(candidate, file_dir)
                        fixed_count += 1
                        return f'#include "{rel_path}"'
                
                if inc_name in file_index:
                    found_file = file_index[inc_name]
                    if any(str(found_file).startswith(str(root)) for root in dest_roots):
                        rel_path = os.path.relpath(found_file, file_dir)
                        fixed_count += 1
                        return f'#include "{rel_path}"'

                if not is_system_header(inc_name):
                    for i, src_root in enumerate(global_src_roots):
                        if (src_root / inc_name).is_file():
                            dest_cand = global_dest_roots[i] / inc_name
                            dest_cand.parent.mkdir(parents=True, exist_ok=True)
                            shutil.copy2(src_root / inc_name, dest_cand)
                            fallback_count += 1
                            logger.warning(f"    FALLBACK COPY: <{inc_name}> grabbed for {rel_file}")
                            rel_path = os.path.relpath(dest_cand, file_dir)
                            return f'#include "{rel_path}"'
                        
            return match.group(0)

        new_content = include_regex.sub(replacer, new_content)
        if new_content != content:
            try:
                file_path.write_text(new_content)
            except Exception as e:
                logger.error(f"    [ERROR] Failed to write {rel_file}: {e}")
            
    logger.info(f"    Fixed {fixed_count} internal includes to strict relative paths.")
    if fallback_count > 0:
        logger.warning(f"    WARNING: {fallback_count} files had to be fetched via fallback!")


def ai_lies():

    logger.info("  Testing include resolution for all extracted files...")
    log_path = DEST_DIR / "include_resolve_test.log"
    
    include_regex = re.compile(r'^\s*#\s*include\s+([<"])([^>"]+)[>"]', re.MULTILINE)
    
    tested_files = 0
    missing_includes = 0
    
    with open(log_path, 'w') as log:
        log.write("Include Resolution Test Report\n")
        log.write("=" * 60 + "\n\n")
        
        for file_path in DEST_DIR.rglob("*.[ch]"):
            
            tested_files += 1
            try:
                content = file_path.read_text(errors='ignore')
            except Exception:
                continue
                
            for match in include_regex.finditer(content):
                delimiter = match.group(1)
                include_name = match.group(2)

                closing = ">" if delimiter == "<" else '"'
                original_include = f"#include {delimiter}{include_name}{closing}"
                
                resolved = False
                
                if delimiter == '"':
                    # Local/relative include: resolve relative to current file's directory
                    candidate = (file_path.parent / include_name).resolve()
                    if candidate.is_file():
                        resolved = True
                        
                elif delimiter == '<':
                    # System include: resolve against LINUXINCLUDE_PREFIXES
                    for prefix in LINUXINCLUDE_PREFIXES:
                        candidate = (DEST_DIR / prefix / include_name).resolve()
                        if candidate.is_file():
                            resolved = True
                            break
                            
                if not resolved:
                    missing_includes += 1
                    rel_file = file_path.relative_to(DEST_DIR)
                    log.write(f"MISSING: {original_include}\n")
                    log.write(f"  File:   {rel_file}\n\n")
                    
        log.write("=" * 60 + "\n")
        log.write("SUMMARY:\n")
        log.write(f"  Files tested:       {tested_files}\n")
        log.write(f"  Unresolved includes: {missing_includes}\n")
        
    if missing_includes == 0:
        logger.info(f"    Resolution test PASSED. All includes found.")
    else:
        logger.warning(f"    Resolution test FAILED. {missing_includes} missing includes. See {log_path.name}")


def strip_junk_includes():
    logger.info("  Stripping alternate-OS/junk includes from extracted files...")
    
    JUNK_INCLUDES = {
        "generated/gcc-plugins.h",
        "generated/randstruct_hash.h",
        "generated/integer-wrap.h",
        "asm/compiler.h",
        "asm/hash.h",
        "asm/bitrev.h",
        "asm/cachetype.h",
        "asm/membarrier.h",
        "uapi/asm-generic/signal.h",
        "acdragonflyex.h", "acefiex.h", "acmsvcex.h",
        "acmsvc.h", "acmacosx.h", "acdragonfly.h", "acfreebsd.h",
        "acnetbsd.h", "acsolaris.h", "acmodesto.h", "acnetware.h",
        "accygwin.h", "acwin.h", "acwin64.h", "acvxworks.h",
        "acos2.h", "achaiku.h", "acqnx.h", "acefi.h",
        "zephyr/kernel.h", "zephyr/device.h", "zephyr/fs/fs.h",
        "zephyr/sys/printk.h", "zephyr/sys/__assert.h", 
    }
    
    include_regex = re.compile(r'^\s*#\s*include\s+[<"]([^>"]+)[>"]\s*\n', re.MULTILINE)
    stripped_count = 0
    
    for file_path in DEST_DIR.rglob("*.[ch]"):
            
        try:
            content = file_path.read_text(errors='ignore')
        except Exception:
            continue
            
        def replacer(match):
            nonlocal stripped_count
            if match.group(1) in JUNK_INCLUDES:
                stripped_count += 1
                return ""  # Completely delete the line
            return match.group(0)
            
        new_content = include_regex.sub(replacer, content)
        
        if new_content != content:
            file_path.write_text(new_content)
            
    logger.info(f"    Deleted {stripped_count} alternate-OS include lines.")


def create_mydefs_h():
    logger.info("  Creating mydefs.h in destination root...")
    mydefs_path = DEST_DIR / "include" / "linux" / "mydefs.h"
    content = """

#define __KERNEL__
#include <linux/compiler-version.h>
#include <linux/kconfig.h>
#include <linux/compiler_types.h>

"""
    mydefs_path.write_text(content)


def copy_driver_build_files():
    logger.info("  Copying Kconfig and Makefiles from target roots and parents...")
    copied_count = 0
    processed_parents = set()

    for mod_path, _ in TARGET_MODULES:
        src_dir = SOURCE_DIR / mod_path
        dst_dir = DEST_DIR / mod_path
        
        if not src_dir.exists():
            continue
            
        dst_dir.mkdir(parents=True, exist_ok=True)

        for name in ["Kconfig", "Makefile"]:
            src_file = src_dir / name
            if src_file.is_file():
                dst_file = dst_dir / name
                if not dst_file.exists():
                    shutil.copy2(src_file, dst_file)
                    logger.info(f"    Copied {mod_path}/{name}")
                    copied_count += 1

        parent_src = src_dir.parent
        parent_key = str(parent_src)

        if parent_key not in processed_parents and parent_src.exists():
            parent_dst = DEST_DIR / parent_src.relative_to(SOURCE_DIR)
            parent_dst.mkdir(parents=True, exist_ok=True)

            for name in ["Kconfig", "Kconfig.debug", "Makefile"]:
                src_parent_file = parent_src / name
                if src_parent_file.is_file():
                    dst_parent_file = parent_dst / name
                    if not dst_parent_file.exists():
                        shutil.copy2(src_parent_file, dst_parent_file)
                        rel_path = parent_src.relative_to(SOURCE_DIR)
                        logger.info(f"    Copied Parent: {rel_path}/{name}")
                        copied_count += 1
            
            processed_parents.add(parent_key)
    
    if copied_count > 0:
        logger.info(f"  Copied {copied_count} build files.")




def main():
    logger.info("="*60)
    logger.info(" Kernel Module to Xcode Extractor")
    logger.info("="*60)
    try:
        if DEST_DIR.exists(): shutil.rmtree(DEST_DIR)
        DEST_DIR.mkdir()
            
        if not TARGET_MODULES:
            raise BuildError("No TARGET_MODULES defined in script!")
            
        check_dependencies()
        prepare_kernel_source()
        configure_kernel()       
        prepare_and_build()      
        
        copy_generated_includes()
        create_mydefs_h()
        file_count = copy_module_tree()
        add_extern_c()
        strip_junk_includes()
        fix_relative_includes()
        strip_junk_includes()

        kbuild_cflags, linuxinclude, subdir_overrides, cmd_file = extract_xcode_flags()
        generate_xcode_settings(kbuild_cflags, linuxinclude, subdir_overrides, cmd_file)

        ai_lies()
        copy_driver_build_files()
        
        logger.info("\n" + "="*60)
        logger.info(" EXTRACTION SUCCESSFUL")
        logger.info(f" Output:           {DEST_DIR.resolve()}")
        logger.info(f" Total Files:      {file_count} (Recursive .c/.h Scanning)")
        logger.info("="*60)
    except Exception as e:
        logger.error(f"\n[!] Failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()