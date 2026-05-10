1) drm setup for browsing i915/xe linux sources

- create a new xcodeproj and add drm_extracted
- after indexing browsing structures should be fine


2) nblue setup
   
- Add nblue to opencore
- use le_kexts.sh to install kexts. (drag and drop sle_Internal/debugLE/Library to terminal)
- default kexts info.plist setup is inside nblue Firmware folder. rebuild nblue to apply
- update lilu and sdk as need by your os
- boot with (remove allow3d for frame only)
  debug=0x144 keepsyms=1 IGLogLevel=0xe -disablegfxfirmware -allow3d

Default is load tgl frame + acel if installed. bad install will cause boot hang.
if no tgl installed then system icl frame loads. In mac os install or recovery mode all kexts are skiped



This count as "working"
<img width="1000" height="476" alt="image thumb png c6fd6d01c83d4117a609b66686be680b" src="https://github.com/user-attachments/assets/6f17a34a-7129-45f3-aac5-697c77bd79ab" />
