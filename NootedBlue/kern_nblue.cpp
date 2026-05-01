//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#include "kern_nblue.hpp"
#include "kern_gen11.hpp"
#include "kern_model.hpp"
#include "DYLDPatches.hpp"
#include "HDMI.hpp"
#include "kern_patcherplus.hpp"
#include <Headers/kern_api.hpp>
#include <Headers/kern_devinfo.hpp>


static const char *pathIOAcceleratorFamily2= "/System/Library/Extensions/IOAcceleratorFamily2.kext/Contents/MacOS/IOAcceleratorFamily2";
static const char *pathAGDP = "/System/Library/Extensions/AppleGraphicsControl.kext/Contents/PlugIns/"
							  "AppleGraphicsDevicePolicy.kext/Contents/MacOS/AppleGraphicsDevicePolicy";
static const char *pathBacklight = "/System/Library/Extensions/AppleBacklight.kext/Contents/MacOS/AppleBacklight";
static const char *pathMCCSControl = "/System/Library/Extensions/AppleMCCSControl.kext/Contents/MacOS/AppleMCCSControl";
static const char *pathIOGraphics= "/System/Library/Extensions/IOGraphicsFamily.kext/IOGraphicsFamily";

static KernelPatcher::KextInfo kextAGDP {"com.apple.driver.AppleGraphicsDevicePolicy", &pathAGDP, 1, {true}, {},
	KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextBacklight {"com.apple.driver.AppleBacklight", &pathBacklight, 1, {true}, {},
	KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextMCCSControl {"com.apple.driver.AppleMCCSControl", &pathMCCSControl, 1, {true}, {},
	KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextIOGraphics { "com.apple.iokit.IOGraphicsFamily", &pathIOGraphics, 1, {true}, {},
	KernelPatcher::KextInfo::Unloaded };
static KernelPatcher::KextInfo kextIOAcceleratorFamily2 { "com.apple.iokit.IOAcceleratorFamily2", &pathIOAcceleratorFamily2, 1, {true}, {},
	KernelPatcher::KextInfo::Unloaded };

NBlue *NBlue::callback = nullptr;

static Gen11 gen11;
static DYLDPatches dyldpatches;
static HDMI agfxhda;

void NBlue::init() {
    callback = this;
	
	lilu.onKextLoadForce(&kextAGDP);
	lilu.onKextLoadForce(&kextBacklight);
	lilu.onKextLoadForce(&kextMCCSControl);
	lilu.onKextLoadForce(&kextIOGraphics);
	lilu.onKextLoadForce(&kextIOAcceleratorFamily2);
	
	gen11.init();
	//agfxhda.init();
	dyldpatches.init();
	
    lilu.onPatcherLoadForce(
        [](void *user, KernelPatcher &patcher) { static_cast<NBlue *>(user)->processPatcher(patcher); }, this);
    lilu.onKextLoadForce(
        nullptr, 0,
        [](void *user, KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
            static_cast<NBlue *>(user)->processKext(patcher, index, address, size);
        },
        this);
	
}

void NBlue::configurePCISpace(const char *propName, IOService *pciDevice,
					   IOPCIAddressSpace addrSpace,
					   UInt64 address,
					   UInt64 length)
{
	bool isExtended = addrSpace.s.t;
	UInt8 spaceType = addrSpace.s.space;
	UInt32 regOffset;

	
	if(isExtended) {
		addrSpace.es.registerNumExtended=(uint32_t)address;
		regOffset = addrSpace.es.registerNum;
	} else {
		regOffset = addrSpace.s.registerNum;
	}
	
	addrSpace.s.reloc=1;
	if (!address ) addrSpace.s.reloc=0;
	if (!strcmp(propName,"ranges")) addrSpace.s.reloc=1;
	
	if (address) {
		if(spaceType != kIOPCIConfigSpace) {
			if(spaceType == kIOPCIIOSpace) {
				UInt32 ioValue = (static_cast<UInt32>(address) & 0xFFFFFFFC) | 0x1;
			   // pciDevice->configWrite32(regOffset, (ioValue));
			} else {
				UInt32 addressLo = static_cast<UInt32>(address & 0xFFFFFFFF);
				UInt32 addressHi = static_cast<UInt32>(address >> 32);
				
				addressLo &= ~0xF;
				
				if(addrSpace.s.prefetch) {
					addressLo |= 0x8;
				}
				
				switch(spaceType) {
					case kIOPCI32BitMemorySpace:
						addressLo |= 0x0;
						break;
					case kIOPCI64BitMemorySpace:
						addressLo |= 0x4;
						break;
					default:
						break;
				}
				
				
			   // pciDevice->configWrite32(regOffset, (addressLo));
				if (spaceType == kIOPCI64BitMemorySpace) {
				  //  pciDevice->configWrite32(regOffset + 4,0xFFFFFFFF);
				   // pciDevice->configWrite32(regOffset + 4, (addressHi));
				}
			}
		}
	}

	
	if(spaceType == kIOPCI64BitMemorySpace) addrSpace.s.space =kIOPCI32BitMemorySpace;
	
	UInt32 configEntry[5];
	configEntry[0] = (addrSpace.bits);
	
	if(spaceType == kIOPCIIOSpace && address==0) {
		configEntry[1] = 0;
		configEntry[2] = 0;
	} else {
		configEntry[1] = (static_cast<UInt32>(address >> 32));
		configEntry[2] = (static_cast<UInt32>(address & 0xFFFFFFFF));
	}
	configEntry[3] = (static_cast<UInt32>(length >> 32));
	configEntry[4] = (static_cast<UInt32>(length & 0xFFFFFFFF));

	OSData *existing = OSDynamicCast(OSData, pciDevice->getProperty(propName));
	OSData *updated = existing ? OSData::withData(existing) : OSData::withCapacity(0);
	
	if(updated) {
		updated->appendBytes(configEntry, sizeof(configEntry));
		pciDevice->setProperty(propName, updated);
		updated->release();
	}

}
void NBlue::processPatcher(KernelPatcher &patcher) {
    auto *devInfo = DeviceInfo::create();
    if (devInfo) {
        devInfo->processSwitchOff();
		
		

        this->iGPU = OSDynamicCast(IOPCIDevice, devInfo->videoBuiltin);
        PANIC_COND(!this->iGPU, "nblue", "videoBuiltin is not IOPCIDevice");
		/*
		IOPCIAddressSpace space;

		this->iGPU->removeProperty("reg");
		this->iGPU->removeProperty("assigned-addresses");
		this->iGPU->removeProperty("IODeviceMemory");
		
		
		IOPCIAddressSpace addrSpace=this->iGPU->space;
			UInt64 address;
			UInt64 length;
		
		address=0;
		length=0;
		addrSpace.s.space=kIOPCIConfigSpace;
		addrSpace.s.registerNum=0x0;
		configurePCISpace("reg", this->iGPU,addrSpace,address,length);
		
		address=0;
		length=0x1000000;
		addrSpace.s.space=kIOPCI64BitMemorySpace;
		addrSpace.s.registerNum=0x10;
		configurePCISpace("reg", this->iGPU,addrSpace,address,length);
		
		
		addrSpace.s.prefetch=1;
		address=0;
		length=0x10000000;
		addrSpace.s.space=kIOPCI64BitMemorySpace;
		addrSpace.s.registerNum=0x18;
		configurePCISpace("reg", this->iGPU,addrSpace,address,length);


		
		addrSpace.s.prefetch=0;
		address=0;
		length=0x40;
		addrSpace.s.space=kIOPCIIOSpace;
		addrSpace.s.registerNum=0x20;
		configurePCISpace("reg", this->iGPU,addrSpace,address,length);
		

		
		
		IODeviceMemory::InitElement rangeList[3];
		
		address=0x6002000000;
		length=0x1000000;
		addrSpace.s.space=kIOPCI64BitMemorySpace;
		addrSpace.s.registerNum=0x10;
		configurePCISpace("assigned-addresses", this->iGPU,addrSpace,address,length);
		
		rangeList[0] =
		{
			.start = address ,
			.length = length,
			.tag = addrSpace.bits
		};
		
		addrSpace.s.prefetch=1;
		address=0x4000000000;
		length=0x10000000;
		addrSpace.s.space=kIOPCI64BitMemorySpace;
		addrSpace.s.registerNum=0x18;
		configurePCISpace("assigned-addresses", this->iGPU,addrSpace,address,length);

		rangeList[1] =
		{
			.start = address ,
			.length = length,
			.tag = addrSpace.bits
		};
		
		addrSpace.s.prefetch=0;
		address=0x4000;
		length=0x40;
		addrSpace.s.space=kIOPCIIOSpace;
		addrSpace.s.registerNum=0x20;
		configurePCISpace("assigned-addresses", this->iGPU,addrSpace,address,length);


		rangeList[2] =
		{
			.start = address ,
			.length = length,
			.tag = addrSpace.bits
		};
		
		OSArray *memoryArray = IODeviceMemory::arrayFromList(rangeList, 3);
		if (memoryArray) {
			this->iGPU->setProperty("IODeviceMemory",memoryArray);
			//this->iGPU->setDeviceMemory(memoryArray);
			memoryArray->release();
		}
		

		this->iGPU->configWrite32(kIOPCIConfigBaseAddress0,  0x02000000);
		this->iGPU->configWrite32(kIOPCIConfigBaseAddress1,  0x60);
		*/
		
		//this->iGPU->enablePCIPowerManagement(kPCIPMCSPowerStateD0);
		this->iGPU->setBusMasterEnable(true);
		this->iGPU->setMemoryEnable(true);
		//this->iGPU->setIOEnable(true);
		
		WIOKit::renameDevice(this->iGPU, "IGPU");
		WIOKit::awaitPublishing(this->iGPU);
		
		/*
		 UInt8  msiCap;
	     bool hasMSI = this->iGPU->findPCICapability(kIOPCICapabilityIDMSI, &msiCap);
		UInt16 control= this->iGPU->configRead16( msiCap + 2);
		control |= 1;
		this->iGPU->configWrite16(msiCap + 2, control);
		 control = this->iGPU->configRead16(kIOPCIConfigCommand);
		 control |= kIOPCICommandInterruptDisable | kIOPCICommandBusMaster;
		this->iGPU->configWrite16(kIOPCIConfigCommand, control);
		this->iGPU->setProperty("IOPCIMSIMode", kOSBooleanTrue);
		*/
		
		UInt8 *configBytes = (UInt8 *)IOMalloc(0x100);
		for (UInt16 offset = 0; offset < 0x100; offset++) {
			configBytes[offset] = this->iGPU->configRead8(offset);
		}
		this->iGPU->setProperty("cfgspace", configBytes, 0x100);
		//this->iGPU->setProperty("ATY,Platforminfo", nullptr, 0);
		
        static uint8_t builtin[] = {0x00};
		//static uint8_t builtin2[] = {0x02, 0x00, 0x5c, 0x8A};
		//static uint8_t builtin3[] = {0x5c, 0x8A,0x00,0x00};
		
		static uint8_t builtin2[] = {0x00, 0x00, 0x49, 0x9A};
		static uint8_t builtin3[] = {0x49, 0x9A,0x00,0x00};

		
		
		static uint8_t sconf[] = {};
		
		
		//[drm:pps_init_registers [i915]] panel power sequencer register settings: PP_ON 0x7d00001, PP_OFF 0x1f40001, PP_DIV 0x60
		//hwGetPanelTimingProperties()
		
		static uint8_t panel[] = {0x01, 0x00, 0x00, 0x00};
		/*static uint8_t panel1[] = {0xd0, 0x07, 0x00, 0x00};
		static uint8_t panel2[] = {0x01, 0x00, 0x00, 0x00};
		static uint8_t panel3[] = {0x4f, 0x01, 0x00, 0x00};
		static uint8_t panel4[] = {0x06, 0x00, 0x00, 0x00};

		if (!this->iGPU->getProperty("AAPL00,PanelPowerUp")) {
			this->iGPU->setProperty("AAPL00,PanelPowerUp", panel, arrsize(panel));
			this->iGPU->setProperty("AAPL00,PanelPowerOn", panel1, arrsize(panel1));
			this->iGPU->setProperty("AAPL00,PanelPowerDown", panel2, arrsize(panel2));
			this->iGPU->setProperty("AAPL00,PanelPowerOff", panel3, arrsize(panel3));
			this->iGPU->setProperty("AAPL00,PanelCycleDelay", panel4, arrsize(panel4));
		}*/

		//this->iGPU->setProperty("AAPL01,IgnoreConnection", panel, arrsize(panel));
		
		this->iGPU->setProperty("built-in", builtin, arrsize(builtin));
		this->iGPU->setProperty("AAPL,slot-name", const_cast<char *>("built-in"), 9);
		this->iGPU->setProperty("hda-gfx", const_cast<char *>("onboard-1"), 10);
		this->iGPU->setProperty("model", const_cast<char *>("Intel Iris Xe Graphics"), 23);
		
		
		//auto *prop = OSDynamicCast(OSData, this->iGPU->getProperty("saved-config"));
		//if (!prop) this->iGPU->setProperty("saved-config", sconf, 0xea);
			
		this->iGPU->setProperty("AAPL,ig-platform-id", builtin2, arrsize(builtin2));
		this->iGPU->setProperty("device-id", builtin3, arrsize(builtin3));


		//auto x = OSDynamicCast(OSData, this->iGPU->getProperty("AAPL,ig-platform-id"));
		//framebufferId = *(uint32_t*)x->getBytesNoCopy();
		
		//NETLOG("gen11", "framebufferId: = %x", framebufferId);
		setRMMIOIfNecessary();

        this->deviceId = WIOKit::readPCIConfigValue(this->iGPU, WIOKit::kIOPCIConfigDeviceID);
        this->pciRevision = WIOKit::readPCIConfigValue(NBlue::callback->iGPU, WIOKit::kIOPCIConfigRevisionID);
		
		
		KernelPatcher::routeVirtual(this->iGPU, WIOKit::PCIConfigOffset::ConfigRead16, configRead16, &orgConfigRead16);
		KernelPatcher::routeVirtual(this->iGPU, WIOKit::PCIConfigOffset::ConfigRead32, configRead32, &orgConfigRead32);

        DeviceInfo::deleter(devInfo);
		
    } else {
        SYSLOG("nblue", "Failed to create DeviceInfo");
    }
	
	/*KernelPatcher::RouteRequest request {"__ZN15OSMetaClassBase12safeMetaCastEPKS_PK11OSMetaClass", wrapSafeMetaCast,
		this->orgSafeMetaCast};
	PANIC_COND(!patcher.routeMultipleLong(KernelPatcher::KernelID, &request, 1), "nblue",
		"Failed to route kernel symbols");*/
	
	dyldpatches.processPatcher(patcher);
	
}

OSMetaClassBase *NBlue::wrapSafeMetaCast(const OSMetaClassBase *anObject, const OSMetaClass *toMeta) {
	auto ret = FunctionCast(wrapSafeMetaCast, callback->orgSafeMetaCast)(anObject, toMeta);
	if (UNLIKELY(!ret)) {
		for (const auto &ent : callback->metaClassMap) {
			if (LIKELY(ent[0] == toMeta)) {
				return FunctionCast(wrapSafeMetaCast, callback->orgSafeMetaCast)(anObject, ent[1]);
			} else if (UNLIKELY(ent[1] == toMeta)) {
				return FunctionCast(wrapSafeMetaCast, callback->orgSafeMetaCast)(anObject, ent[0]);
			}
		}
	}
	return ret;
}

void NBlue::setRMMIOIfNecessary() {
	if (UNLIKELY(!this->rmmio || !this->rmmio->getLength())) {
		this->rmmio = this->iGPU->mapDeviceMemoryWithRegister(kIOPCIConfigBaseAddress0);
		this->rmmioPtr = reinterpret_cast<volatile uint32_t *>(this->rmmio->getVirtualAddress());
	}
}

bool NBlue::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
	if (kextIOAcceleratorFamily2.loadIndex == index) {
		
		static const uint8_t f1[]= {0x74, 0x57, 0x45, 0x8b, 0x8f, 0x84, 0x02, 0x00, 0x00, 0x48, 0x8d, 0x3d, 0x4a, 0x47, 0xfb, 0xff};
		static const uint8_t r1[]= {0xeb, 0x57, 0x45, 0x8b, 0x8f, 0x84, 0x02, 0x00, 0x00, 0x48, 0x8d, 0x3d, 0x4a, 0x47, 0xfb, 0xff};
		const LookupPatchPlus patch {&kextIOAcceleratorFamily2, f1, r1, 1};
		//SYSLOG_COND(!patch.apply(patcher, address, size), "NBlue", "Failed to apply kextIOAcceleratorFamily2 patch");

		return true;
	}  else if (kextIOGraphics.loadIndex == index) {
		/*
		KernelPatcher::RouteRequest requests[] = {
				{"__ZN13IOFramebuffer25extValidateDetailedTimingEP8OSObjectPvP25IOExternalMethodArguments", wrapValidateDetailedTiming},
			};
			patcher.routeMultiple(index, requests, address, size);
			patcher.clearError();*/
		return true;
	}  else if (kextAGDP.loadIndex == index) {
		const LookupPatchPlus patch {&kextAGDP, kAGDPBoardIDKeyOriginal, kAGDPBoardIDKeyPatched, 1};
		SYSLOG_COND(!patch.apply(patcher, address, size), "NBlue", "Failed to apply AGDP board-id patch");

		/*if (getKernelVersion() == KernelVersion::Ventura) {
			const LookupPatchPlus patch {&kextAGDP, kAGDPFBCountCheckVenturaOriginal, kAGDPFBCountCheckVenturaPatched,
				1};
			SYSLOG_COND(!patch.apply(patcher, address, size), "NBlue", "Failed to apply AGDP fb count check patch");
		} else {
			const LookupPatchPlus patch {&kextAGDP, kAGDPFBCountCheckOriginal, kAGDPFBCountCheckPatched, 1};
			SYSLOG_COND(!patch.apply(patcher, address, size), "NBlue", "Failed to apply AGDP fb count check patch");
		}*/
		return true;
	}  else if (kextBacklight.loadIndex == index) {
		KernelPatcher::RouteRequest request {"__ZN15AppleIntelPanel10setDisplayEP9IODisplay", wrapApplePanelSetDisplay,
	  orgApplePanelSetDisplay};
			if (patcher.routeMultiple(kextBacklight.loadIndex, &request, 1, address, size)) {
				const UInt8 find[] = {"F%uT%04x"};
				const UInt8 replace[] = {"F%uTxxxx"};
				const LookupPatchPlus patch {&kextBacklight, find, replace, 1};
				SYSLOG_COND(!patch.apply(patcher, address, size), "NBlue", "Failed to apply backlight patch");
			}
		return true;
} else if (kextMCCSControl.loadIndex == index) {
		KernelPatcher::RouteRequest requests[] = {
				{"__ZN25AppleMCCSControlGibraltar5probeEP9IOServicePi", wrapFunctionReturnZero},
				{"__ZN21AppleMCCSControlCello5probeEP9IOServicePi", wrapFunctionReturnZero},
			};
			patcher.routeMultiple(index, requests, address, size);
			patcher.clearError();
	return true;
} else if (gen11.processKext(patcher, index, address, size)) {
        DBGLOG("nblue", "Processed Generation 11 configuration");
	return true;
    } /*else if (agfxhda.processKext(patcher, index, address, size)) {
		DBGLOG("nblue", "Processed AppleGFXHDA");
	}*/
	
	
    return false;
}



uint16_t NBlue::configRead16(IORegistryEntry *service, uint32_t space, uint8_t offset) {
	if (callback && callback->orgConfigRead16) {
		auto result = callback->orgConfigRead16(service, space, offset);
		if (offset == WIOKit::kIOPCIConfigDeviceID && service != nullptr) {
			auto name = service->getName();
			if (name && name[0] == 'I' && name[1] == 'G' && name[2] == 'P' && name[3] == 'U') {
				uint32_t device;
				if (WIOKit::getOSDataValue(service, "device-id", device) && device != result) {
					return device;
				}
			}
		}

		return result;
	}

	return 0;
}

uint32_t NBlue::configRead32(IORegistryEntry *service, uint32_t space, uint8_t offset) {
	if (callback && callback->orgConfigRead32) {
		auto result = callback->orgConfigRead32(service, space, offset);
		// According to lvs unaligned reads may happen
		if ((offset == WIOKit::kIOPCIConfigDeviceID || offset == WIOKit::kIOPCIConfigVendorID) && service != nullptr) {
			auto name = service->getName();
			if (name && name[0] == 'I' && name[1] == 'G' && name[2] == 'P' && name[3] == 'U') {
				uint32_t device;
				if (WIOKit::getOSDataValue(service, "device-id", device) && device != (result & 0xFFFF)) {
					device = (result & 0xFFFF) | (device << 16);
					return device;
				}
			}
		}

		return result;
	}

	return 0;
}

size_t NBlue::wrapFunctionReturnZero() { return 0; }

struct ApplePanelData {
	const char *deviceName;
	UInt8 deviceData[36];
};


static ApplePanelData appleBacklightData[0]; // this disable Backlight panels

// this enables Backlight panels
//rename as whish
static ApplePanelData appleBacklightData2[] = {
	{"F14Txxxx", {0x00, 0x11, 0x00, 0x00, 0x00, 0x34, 0x00, 0x52, 0x00, 0x73, 0x00, 0x94, 0x00, 0xBE, 0x00, 0xFA, 0x01,
					 0x36, 0x01, 0x72, 0x01, 0xC5, 0x02, 0x2F, 0x02, 0xB9, 0x03, 0x60, 0x04, 0x1A, 0x05, 0x0A, 0x06,
					 0x0E, 0x07, 0x10}},
	{"F15Txxxx", {0x00, 0x11, 0x00, 0x00, 0x00, 0x36, 0x00, 0x54, 0x00, 0x7D, 0x00, 0xB2, 0x00, 0xF5, 0x01, 0x49, 0x01,
					 0xB1, 0x02, 0x2B, 0x02, 0xB8, 0x03, 0x59, 0x04, 0x13, 0x04, 0xEC, 0x05, 0xF3, 0x07, 0x34, 0x08,
					 0xAF, 0x0A, 0xD9}},
	{"F16Txxxx", {0x00, 0x11, 0x00, 0x00, 0x00, 0x18, 0x00, 0x27, 0x00, 0x3A, 0x00, 0x52, 0x00, 0x71, 0x00, 0x96, 0x00,
					 0xC4, 0x00, 0xFC, 0x01, 0x40, 0x01, 0x93, 0x01, 0xF6, 0x02, 0x6E, 0x02, 0xFE, 0x03, 0xAA, 0x04,
					 0x78, 0x05, 0x6C}},
	{"F17Txxxx", {0x00, 0x11, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x34, 0x00, 0x4F, 0x00, 0x71, 0x00, 0x9B, 0x00, 0xCF, 0x01,
					 0x0E, 0x01, 0x5D, 0x01, 0xBB, 0x02, 0x2F, 0x02, 0xB9, 0x03, 0x60, 0x04, 0x29, 0x05, 0x1E, 0x06,
					 0x44, 0x07, 0xA1}},
	{"F18Txxxx", {0x00, 0x11, 0x00, 0x00, 0x00, 0x53, 0x00, 0x8C, 0x00, 0xD5, 0x01, 0x31, 0x01, 0xA2, 0x02, 0x2E, 0x02,
					 0xD8, 0x03, 0xAE, 0x04, 0xAC, 0x05, 0xE5, 0x07, 0x59, 0x09, 0x1C, 0x0B, 0x3B, 0x0D, 0xD0, 0x10,
					 0xEA, 0x14, 0x99}},
	{"F19Txxxx", {0x00, 0x11, 0x00, 0x00, 0x02, 0x8F, 0x03, 0x53, 0x04, 0x5A, 0x05, 0xA1, 0x07, 0xAE, 0x0A, 0x3D, 0x0E,
					 0x14, 0x13, 0x74, 0x1A, 0x5E, 0x24, 0x18, 0x31, 0xA9, 0x44, 0x59, 0x5E, 0x76, 0x83, 0x11, 0xB6,
					 0xC7, 0xFF, 0x7B}},
	{"F24Txxxx", {0x00, 0x11, 0x00, 0x01, 0x00, 0x34, 0x00, 0x52, 0x00, 0x73, 0x00, 0x94, 0x00, 0xBE, 0x00, 0xFA, 0x01,
					 0x36, 0x01, 0x72, 0x01, 0xC5, 0x02, 0x2F, 0x02, 0xB9, 0x03, 0x60, 0x04, 0x1A, 0x05, 0x0A, 0x06,
					 0x0E, 0x07, 0x10}},
};

bool NBlue::wrapApplePanelSetDisplay(IOService *that, IODisplay *display) {
	static bool once = false;
	if (!once) {
		once = true;
		auto *panels = OSDynamicCast(OSDictionary, that->getProperty("ApplePanels"));
		if (panels) {
			auto *rawPanels = panels->copyCollection();
			panels = OSDynamicCast(OSDictionary, rawPanels);

			if (panels) {
				for (auto &entry : appleBacklightData) {
					auto pd = OSData::withBytes(entry.deviceData, sizeof(entry.deviceData));
					if (pd) {
						panels->setObject(entry.deviceName, pd);
						//! No release required by current AppleBacklight implementation.
					} else {
					}
				}
				that->setProperty("ApplePanels", panels);
			}

			OSSafeReleaseNULL(rawPanels);
		} else {
		}
	}

	bool ret = FunctionCast(wrapApplePanelSetDisplay, callback->orgApplePanelSetDisplay)(that, display);
	return ret;
}

