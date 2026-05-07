1) drm setup for browsing i915/xe linux sources

- create a new xcodeprogj and add drm_extracted
- after indexing browsing structures should be fine


2) nblue setup
   
- Add nblue to opencore
- Install framebuffer + hookcase to /l/e
- Install acel+bundles to /s/l/e
- wrong kexts permissions or bad kext install = boot hang
- sudo chmod -R 755
- sudo chown -R root:wheel
- be sure owner rw, others ro
- default kexts setup using nred logic. change inside xcodeproj
- update lilu and sdk as need by your os
- boot with (remove allow3d for frame only)
  debug=0x144 keepsyms=1 IGLogLevel=0xe -disablegfxfirmware -allow3d

default is load tgl frame + acel if installed. if none then system icl frame loads. install or recovery model all are skiped



This count as "working"
<img width="1000" height="476" alt="image thumb png c6fd6d01c83d4117a609b66686be680b" src="https://github.com/user-attachments/assets/6f17a34a-7129-45f3-aac5-697c77bd79ab" />
