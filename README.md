1) drm setup for browsing i915/xe linux sources

- create a new xcodeproj and add drm_extracted
- after indexing browsing structures should be fine


2) nblue setup
   
- Add nblue to opencore
- use le_kexts.sh to install kexts. (drag and drop sle_Internal/debugLE/Library to terminal)
- type chmod +x le_kexts.sh if it doesnt run.
- default kexts info.plist setup is inside nblue Firmware folder. rebuild nblue to apply
- update lilu and sdk as need by your os
- boot with (remove allow3d for frame only)
  debug=0x144 keepsyms=1 IGLogLevel=0xe -disablegfxfirmware -allow3d

Default is load tgl frame + acel if installed. bad install will cause boot hang.
change kexts load logic in wrapAddDrivers(). In mac os install or recovery mode all kexts are skiped

if tgl not installed nblue will load icl kexts

focus on development is icl while keeping tgl as fallback for some functions as need.
