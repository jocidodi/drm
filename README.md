1) drm setup for browsing i915/xe linux sources

- create a new xcodeproj and add drm_extracted
- after indexing browsing structures should be fine


2) nblue setup

boot args: debug=0x144 keepsyms=1 IGLogLevel=0xe -disablegfxfirmware

add -allow3d to load graphics
   
use opencore device injection for icl

<img width="621" height="88" alt="Captura de ecrã 2026-06-09, às 17 12 37" src="https://github.com/user-attachments/assets/283a2569-d0ab-4fd7-8080-3448f14f8ecf" />

for tgl use 0x9a49
