1) drm setup for browsing i915/xe linux sources

- create a new xcodeproj and add drm_extracted
- after indexing browsing structures should be fine


2) nblue setup
   
- Add nblue to opencore
- use le_kexts.sh to install kexts. (drag and drop sle_Internal/frame/debugLE/Library and sle_Internal/frame/hook/Library to terminal)
- type chmod +x le_kexts.sh if it doesnt run.
- update lilu and sdk as need by your os
- boot with
  debug=0x144 keepsyms=1 IGLogLevel=0xe -disablegfxfirmware
- use -allow3d for graphics kext loading

Default is load tgl kexts.
if tgl is not installed nblue will load icl kexts

focus on development is icl while keeping tgl as fallback.
