//! Copyright © 2022-2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5.
//! See LICENSE for details.

#include "DYLDPatches.hpp"
#include "kern_nblue.hpp"
#include <Headers/kern_api.hpp>
#include <Headers/kern_devinfo.hpp>
#include <IOKit/IODeviceTreeSupport.h>

DYLDPatches *DYLDPatches::callback = nullptr;

void DYLDPatches::init() { callback = this; }

void DYLDPatches::processPatcher(KernelPatcher &patcher) {

	//return;
	
	auto *entry = IORegistryEntry::fromPath("/", gIODTPlane);
	if (entry) {
		DBGLOG("DYLD", "Setting hwgva-id to iMacPro1,1");
		entry->setProperty("hwgva-id", const_cast<char *>(kHwGvaId), arrsize(kHwGvaId));
		OSSafeReleaseNULL(entry);
	}

	KernelPatcher::RouteRequest request {"_cs_validate_page", wrapCsValidatePage, this->orgCsValidatePage};

	SYSLOG_COND(!patcher.routeMultipleLong(KernelPatcher::KernelID, &request, 1), "DYLD",
		"Failed to route kernel symbols");
}

void DYLDPatches::wrapCsValidatePage(vnode *vp, memory_object_t pager, memory_object_offset_t page_offset,
	const void *data, int *validated_p, int *tainted_p, int *nx_p) {
	FunctionCast(wrapCsValidatePage, callback->orgCsValidatePage)(vp, pager, page_offset, data, validated_p, tainted_p,
		nx_p);

	char path[PATH_MAX];
	int pathlen = PATH_MAX;
	if (vn_getpath(vp, path, &pathlen) != 0) { return; }

	if (!UserPatcher::matchSharedCachePath(path)) {
	    if ((!strncmp(path, kCoreLSKDMSEPath, arrsize(kCoreLSKDMSEPath))) ||
			(!strncmp(path, kCoreLSKDPath, arrsize(kCoreLSKDPath)))) {
		const DYLDPatch patch = {kCoreLSKDOriginal, kCoreLSKDPatched, "CoreLSKD streaming CPUID to Haswell"};
		patch.apply(const_cast<void *>(data), PAGE_SIZE);
		}
		
		if ((!strncmp(path, TGLGraphicsMTLDriver, arrsize(TGLGraphicsMTLDriver)) )) {
		const DYLDPatch patches[] = {
			{kTGLGraphicsMTLDrivero, kTGLGraphicsMTLDriverp, "kTGLGraphicsMTLDriverp"},
		};
		DYLDPatch::applyAll(patches, const_cast<void *>(data), PAGE_SIZE);
		}
		
		if ((!strncmp(path, libMTLIGCCompilerPluginPath, arrsize(libMTLIGCCompilerPluginPath)) )) {
		const DYLDPatch patches[] = {
			{klibMTLIGCCompilerPluginOriginal, klibMTLIGCCompilerPluginPatched, "klibMTLIGCCompilerPluginOriginal"},
			{klibMTLIGCCompilerPluginOriginal2, klibMTLIGCCompilerPluginPatched2, "klibMTLIGCCompilerPluginOriginal2"},
		};
		DYLDPatch::applyAll(patches, const_cast<void *>(data), PAGE_SIZE);
		}
		
		
		return;
	}

	if (UNLIKELY(KernelPatcher::findAndReplace(const_cast<void *>(data), PAGE_SIZE, kVideoToolboxDRMModelOriginal,
			arrsize(kVideoToolboxDRMModelOriginal), BaseDeviceInfo::get().modelIdentifier, 20))) {
		DBGLOG("DYLD", "Applied 'VideoToolbox DRM model check' patch");
	}

	const DYLDPatch patches[] = {
		{kAGVABoardIdOriginal, kAGVABoardIdPatched, "iMacPro1,1 spoof (AppleGVA)"},
		{kHEVCEncBoardIdOriginal, kHEVCEncBoardIdPatched, "iMacPro1,1 spoof (AppleGVAHEVCEncoder)"},
	};
	DYLDPatch::applyAll(patches, const_cast<void *>(data), PAGE_SIZE);
	
	
	static const uint8_t gpuPathFind[] = {
		0x2F, 0x4C, 0x69, 0x62, 0x72, 0x61, 0x72, 0x79, // /Library
		0x2F, 0x47, 0x50, 0x55, 0x42, 0x75, 0x6E, 0x64, // /GPUBund
		0x6C, 0x65, 0x73, 0x00,                           // les\0
	};
	static const uint8_t gpuPathRepl[] = {
		0x2F, 0x4C, 0x69, 0x62, 0x72, 0x61, 0x72, 0x79, // /Library
		0x2F, 0x45, 0x78, 0x74, 0x65, 0x6E, 0x73, 0x69, // /Extensi
		0x6F, 0x6E, 0x73, 0x00,                           // ons\0
	};
	if (UNLIKELY(KernelPatcher::findAndReplace(const_cast<void *>(data), PAGE_SIZE,
			gpuPathFind, arrsize(gpuPathFind), gpuPathRepl, arrsize(gpuPathRepl)))) {
		SYSLOG("DYLD", "V50: Patched gpu_bundle_find_trusted: /Library/GPUBundles -> /Library/Extensions");
	}
	
	
		
	
	//GPUWranglerGPUIsRemovable
	static const uint8_t f5[] = {0x31, 0xFF, 0x4C, 0x3B, 0x35, 0xC1, 0x82, 0xA0, 0x33, 0x40, 0x0F, 0x94, 0xC7, 0x48, 0x8D, 0x35, 0xC1, 0x44, 0x00, 0x00, 0xE8, 0xB9, 0x14, 0x00, 0x00, 0x48, 0x89, 0xDF, 0xE8, 0x1F, 0x00, 0x00, 0x00, 0x83, 0xE0, 0x02, 0xD1, 0xE8};
	static const uint8_t r5[] = {0x31, 0xFF, 0x4C, 0x3B, 0x35, 0xC1, 0x82, 0xA0, 0x33, 0x40, 0x0F, 0x94, 0xC7, 0x48, 0x8D, 0x35, 0xC1, 0x44, 0x00, 0x00, 0xE8, 0xB9, 0x14, 0x00, 0x00, 0x48, 0x89, 0xDF, 0xE8, 0x1F, 0x00, 0x00, 0x00, 0x83, 0xE0, 0x02, 0x31, 0xC0};
	
	
	if (getKernelVersion() >= KernelVersion::Ventura) {
		const DYLDPatch patches[] = {
			
			{f5, r5, "f5"},

		};
		//DYLDPatch::applyAll(patches, const_cast<void *>(data), PAGE_SIZE);
	}

	
}
