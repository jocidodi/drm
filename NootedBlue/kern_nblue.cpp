//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#include "kern_nblue.hpp"
#include "kern_gen11.hpp"
#include "DYLDPatches.hpp"
#include "HDMI.hpp"
#include "Firmware.hpp"
#include "kern_patcherplus.hpp"
#include <Headers/kern_api.hpp>
#include <Headers/kern_devinfo.hpp>
#include <IOKit/IOCatalogue.h>

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
static ConnectorInfo bconnectors[6];

void NBlue::init() {
    callback = this;
	
	lilu.onKextLoadForce(&kextAGDP);
	lilu.onKextLoadForce(&kextBacklight);
	lilu.onKextLoadForce(&kextMCCSControl);
	lilu.onKextLoadForce(&kextIOGraphics);
	lilu.onKextLoadForce(&kextIOAcceleratorFamily2);
	
	gen11.init();
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


void NBlue::processPatcher(KernelPatcher &patcher) {
    auto *devInfo = DeviceInfo::create();
    if (devInfo) {
        devInfo->processSwitchOff();
		
        this->iGPU = OSDynamicCast(IOPCIDevice, devInfo->videoBuiltin);
        PANIC_COND(!this->iGPU, "nblue", "videoBuiltin is not IOPCIDevice");
		
		//this->iGPU->enablePCIPowerManagement(kPCIPMCSPowerStateD0);
		this->iGPU->setBusMasterEnable(true);
		this->iGPU->setMemoryEnable(true);
		//this->iGPU->setIOEnable(true);
		
		WIOKit::renameDevice(this->iGPU, "IGPU");
		WIOKit::awaitPublishing(this->iGPU);
		
        static uint8_t builtin[] = {0x00};
		
		this->iGPU->setProperty("built-in", builtin, arrsize(builtin));
		this->iGPU->setProperty("AAPL,slot-name", const_cast<char *>("built-in"), 9);
		this->iGPU->setProperty("hda-gfx", const_cast<char *>("onboard-1"), 10);
		this->iGPU->setProperty("model", const_cast<char *>("Intel Iris Xe Graphics"), 23);
			
		setRMMIOIfNecessary();

        this->deviceId = WIOKit::readPCIConfigValue(this->iGPU, WIOKit::kIOPCIConfigDeviceID);
        this->pciRevision = WIOKit::readPCIConfigValue(NBlue::callback->iGPU, WIOKit::kIOPCIConfigRevisionID);

		// --- CPUID Detection ---
		uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
		asm volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(1));
		
		uint32_t family = (eax >> 8) & 0xF;
		uint32_t model = (eax >> 4) & 0xF;
		uint32_t extModel = (eax >> 16) & 0xF;
		uint32_t stepping = eax & 0xF;
		if (family == 0x6) {
			model |= (extModel << 4);
		}
		this->cpuModel = model;
		this->isRealTGL = (model == 0x8C || model == 0x8D);
		this->isRKL = (model == 0xA7);
		this->isADL = (model == 0x97 || model == 0x9A || model == 0xBE);
		this->isRPL = (model == 0xB7 || model == 0xBA || model == 0xBF ||
					   model == 0xA6 || model == 0xAA);
		this->isMTL = (model == 0xAC || model == 0xAD);


		SYSLOG("ngreen", "CPU family=0x%x model=0x%x stepping=%u isRealTGL=%d",family, model, stepping, this->isRealTGL);
		
		if (intel_opregion_setup(this->iGPU)!=0) panic("BAD BIOS");
		
		KernelPatcher::routeVirtual(this->iGPU, WIOKit::PCIConfigOffset::ConfigRead16, configRead16, &orgConfigRead16);
		KernelPatcher::routeVirtual(this->iGPU, WIOKit::PCIConfigOffset::ConfigRead32, configRead32, &orgConfigRead32);

        DeviceInfo::deleter(devInfo);
		
    } else {
        SYSLOG("nblue", "Failed to create DeviceInfo");
    }
	
	
	dyldpatches.processPatcher(patcher);
	
	KernelPatcher::RouteRequest request{"__ZN11IOCatalogue10addDriversEP7OSArrayb", wrapAddDrivers,
										this->orgAddDrivers};
	PANIC_COND(!patcher.routeMultipleLong(KernelPatcher::KernelID, &request, 1), "DriverInjector",
			   "Failed to route addDrivers");
}

bool NBlue::wrapAddDrivers(void* const self, OSArray* const array, const bool doNubMatching)
{
	if (1)
	{
		static uint8_t builtin2[] = {0x00, 0x00, 0x5d, 0x8A};
		static uint8_t builtin3[] = {0x5d, 0x8A, 0x00, 0x00};
		int ok=0;
		vnode_t vnode = NULLVP;
		vfs_context_t ctxt = vfs_context_create(nullptr);
		errno_t err = vnode_lookup("/Library/Extensions/AppleIntelTGLGraphicsFramebuffer.kext/Contents/MacOS/AppleIntelTGLGraphicsFramebuffer", 0, &vnode, ctxt);
		if (!err) vnode_put(vnode);
		vfs_context_rele(ctxt);
		if (!err) ok=1;
		
		//tgl
		if (ok)
		{
			builtin2[2]=0x49;
			builtin2[3]=0x9A;
			builtin3[0]=0x49;
			builtin3[1]=0x9A;
		}
		
		NBlue::callback->iGPU->setProperty("AAPL,ig-platform-id", builtin2, arrsize(builtin2));
		NBlue::callback->iGPU->setProperty("device-id", builtin3, arrsize(builtin3));
	}
	
	
	
	OSArray *tdrivers= array;
	bool doNub=doNubMatching;
	int ok=0;
	vnode_t vnode = NULLVP;
	vfs_context_t ctxt = vfs_context_create(nullptr);
	errno_t err = vnode_lookup("/usr/bin/sudo", 0, &vnode, ctxt);
	if (!err) vnode_put(vnode);
	vfs_context_rele(ctxt);
	if (!err) ok=1;
	
	
	
	
	if (ok) { //not running mac os installer
	
		int tcap=0;
		if (ok) {
				ok=0;
				vnode = NULLVP;
				ctxt = vfs_context_create(nullptr);
				err = vnode_lookup("/Library/Extensions/AppleIntelTGLGraphicsFramebuffer.kext/Contents/MacOS/AppleIntelTGLGraphicsFramebuffer", 0, &vnode, ctxt);
			   if (!err) vnode_put(vnode);
			   vfs_context_rele(ctxt);
			   if (!err) ok=1;

			//if (ok) { // remove this to load icl if tgl not installed
				const auto driversXML = getFWByName(ok ? "Driverf2.xml":"Driverf1.xml");
				auto *dataNull = new char[driversXML.size + 1];
				memcpy(dataNull, driversXML.data, driversXML.size);
				dataNull[driversXML.size] = 0;
				OSString *errStr = nullptr;
				auto *dataUnserialized = OSUnserializeXML(dataNull, driversXML.size + 1, &errStr);
				delete[] dataNull;
				PANIC_COND(!dataUnserialized, "NRed", "Failed to unserialize Drivers.xml: %s",
						   errStr ? errStr->getCStringNoCopy() : "Unspecified");
				auto *drivers = OSDynamicCast(OSDictionary, dataUnserialized);
				PANIC_COND(!drivers, "NRed", "Failed to cast Drivers.xml data");
				auto* Match = OSDynamicCast(OSString, drivers->getObject("IOPCIPrimaryMatch"));
				if (Match->getLength()>0){
					tcap++;
					tdrivers->ensureCapacity(tcap);
					tdrivers->setObject(tcap-1,drivers);
					doNub=true;
				} else ok=0;
				OSSafeReleaseNULL(dataUnserialized);
				IOFree(driversXML.data, driversXML.size);
			//}
		}
		if (1/*ok*/) {
				ok=0;
				int sle=-1;
				vnode = NULLVP;
				ctxt = vfs_context_create(nullptr);
				err = vnode_lookup("/System/Library/Extensions/AppleIntelTGLGraphics.kext/Contents/MacOS/AppleIntelTGLGraphics", 0, &vnode, ctxt);
				if (!err) vnode_put(vnode);
				vfs_context_rele(ctxt);
				if (!err) ok=1;
				if (!err) sle=1;
			
					if (!ok) {
						vnode = NULLVP;
						ctxt = vfs_context_create(nullptr);
						err = vnode_lookup("/Library/Extensions/AppleIntelTGLGraphics.kext/Contents/MacOS/AppleIntelTGLGraphics", 0, &vnode, ctxt);
					   if (!err) vnode_put(vnode);
					   vfs_context_rele(ctxt);
					   if (!err) ok=1;
						if (!err) sle=0;
					}
			
			if (ok && sle>=0){
				const auto driversXML = getFWByName(sle ? "Drivera2.xml": "Drivera3.xml");
				auto *dataNull = new char[driversXML.size + 1];
				memcpy(dataNull, driversXML.data, driversXML.size);
				dataNull[driversXML.size] = 0;
				OSString *errStr = nullptr;
				auto *dataUnserialized = OSUnserializeXML(dataNull, driversXML.size + 1, &errStr);
				delete[] dataNull;
				PANIC_COND(!dataUnserialized, "NRed", "Failed to unserialize Drivers.xml: %s",
						   errStr ? errStr->getCStringNoCopy() : "Unspecified");
				auto *drivers = OSDynamicCast(OSDictionary, dataUnserialized);
				PANIC_COND(!drivers, "NRed", "Failed to cast Drivers.xml data");
				auto* Match = OSDynamicCast(OSString, drivers->getObject("IOPCIPrimaryMatch"));
				if (Match->getLength()>0){
					tcap++;
					tdrivers->ensureCapacity(tcap);
					tdrivers->setObject(tcap-1,drivers);
					doNub=true;
				}
				OSSafeReleaseNULL(dataUnserialized);
				IOFree(driversXML.data, driversXML.size);
			}
			else
			{
				const auto driversXML = getFWByName("Drivera1.xml");
				auto *dataNull = new char[driversXML.size + 1];
				memcpy(dataNull, driversXML.data, driversXML.size);
				dataNull[driversXML.size] = 0;
				OSString *errStr = nullptr;
				auto *dataUnserialized = OSUnserializeXML(dataNull, driversXML.size + 1, &errStr);
				delete[] dataNull;
				PANIC_COND(!dataUnserialized, "NRed", "Failed to unserialize Drivers.xml: %s",
						   errStr ? errStr->getCStringNoCopy() : "Unspecified");
				auto *drivers = OSDynamicCast(OSDictionary, dataUnserialized);
				PANIC_COND(!drivers, "NRed", "Failed to cast Drivers.xml data");
				auto* Match = OSDynamicCast(OSString, drivers->getObject("IOPCIPrimaryMatch"));
				if (Match->getLength()>0){
					tcap++;
					tdrivers->ensureCapacity(tcap);
					tdrivers->setObject(tcap-1,drivers);
					doNub=true;
				}
				OSSafeReleaseNULL(dataUnserialized);
				IOFree(driversXML.data, driversXML.size);
			}
		}
		
	}

	return FunctionCast(wrapAddDrivers, callback->orgAddDrivers)(self, array, doNub);
}


void NBlue::setRMMIOIfNecessary() {
	if (UNLIKELY(!this->rmmio || !this->rmmio->getLength())) {
		this->rmmio = this->iGPU->mapDeviceMemoryWithRegister(kIOPCIConfigBaseAddress0);
		this->rmmioPtr = reinterpret_cast<volatile uint32_t *>(this->rmmio->getVirtualAddress());
	}
}

bool NBlue::detectConnectors(void *connectorsp) {
	struct ConnectorInfo *cc=(struct ConnectorInfo*)connectorsp;
	
	for (int i = 0; i < 6; i++) {
		cc[i].index=bconnectors[i].index;
		cc[i].busId=bconnectors[i].busId;
		cc[i].pipe=bconnectors[i].pipe;
		cc[i].pad=bconnectors[i].pad;
		cc[i].type=bconnectors[i].type;
		cc[i].flags=bconnectors[i].flags;
	}
	

	return true;
}

bool NBlue::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
	
	if (kextIOAcceleratorFamily2.loadIndex == index) {
		
		return true;
	}  else if (kextIOGraphics.loadIndex == index) {

		return true;
	}  else if (kextAGDP.loadIndex == index) {
		const LookupPatchPlus patch {&kextAGDP, kAGDPBoardIDKeyOriginal, kAGDPBoardIDKeyPatched, 1};
		SYSLOG_COND(!patch.apply(patcher, address, size), "NBlue", "Failed to apply AGDP board-id patch");
		
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


static ApplePanelData appleBacklightData[] = {
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


static const struct {
	enum bdb_block_id section_id;
	size_t min_size;
} bdb_blocks[] = {
	{ .section_id = BDB_GENERAL_FEATURES,
	  .min_size = sizeof(struct bdb_general_features), },
	{ .section_id = BDB_GENERAL_DEFINITIONS,
	  .min_size = sizeof(struct bdb_general_definitions), },
	{ .section_id = BDB_PSR,
	  .min_size = sizeof(struct bdb_psr), },
	{ .section_id = BDB_DRIVER_FEATURES,
	  .min_size = sizeof(struct bdb_driver_features), },
	{ .section_id = BDB_SDVO_LVDS_OPTIONS,
	  .min_size = sizeof(struct bdb_sdvo_lvds_options), },
	{ .section_id = BDB_SDVO_LVDS_DTD,
	  .min_size = sizeof(struct bdb_sdvo_lvds_dtd), },
	{ .section_id = BDB_EDP,
	  .min_size = sizeof(struct bdb_edp), },
	{ .section_id = BDB_LFP_OPTIONS,
	  .min_size = sizeof(struct bdb_lfp_options), },

	{ .section_id = BDB_LFP_DATA_PTRS,
	  .min_size = sizeof(struct bdb_lfp_data_ptrs), },
	{ .section_id = BDB_LFP_DATA,
	  .min_size = 0, /* special case */ },
	{ .section_id = BDB_LFP_BACKLIGHT,
	  .min_size = sizeof(struct bdb_lfp_backlight), },
	{ .section_id = BDB_LFP_POWER,
	  .min_size = sizeof(struct bdb_lfp_power), },
	{ .section_id = BDB_MIPI_CONFIG,
	  .min_size = sizeof(struct bdb_mipi_config), },
	{ .section_id = BDB_MIPI_SEQUENCE,
	  .min_size = sizeof(struct bdb_mipi_sequence) },
	{ .section_id = BDB_COMPRESSION_PARAMETERS,
	  .min_size = sizeof(struct bdb_compression_parameters), },
	{ .section_id = BDB_GENERIC_DTD,
	  .min_size = sizeof(struct bdb_generic_dtd), },
};

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))


static u32 _get_blocksize(const u8 *block_base)
{
	if (*block_base == BDB_MIPI_SEQUENCE && *(block_base + 3) >= 3)
		return *((const u32 *)(block_base + 4));
	else
		return *((const u16 *)(block_base + 1));
}

static const void *find_raw_section(const void *_bdb, int section_id)
{
	const struct bdb_header *bdb = static_cast<const struct bdb_header *>(_bdb);
	const u8 *base = static_cast<const u8 *>(_bdb);
	int index = 0;
	u32 total, current_size;
	int current_id;


	index = (bdb->header_size) ? bdb->header_size : 16;
	total = bdb->bdb_size;

	while (index + 3 < total) {
		current_id = *(base + index);
		current_size = _get_blocksize(base + index);
		index += 3;

		if (index + current_size > total)
			return NULL;

		if (current_id == section_id)
			return base + index;

		index += current_size;
	}

	return NULL;
}

static size_t lfp_data_min_size(const struct bdb_header *bdb)
{
	const struct bdb_lfp_data_ptrs *ptrs;
	size_t size;

	ptrs = (struct bdb_lfp_data_ptrs*)find_raw_section((void*)bdb, BDB_LFP_DATA_PTRS);
	if (!ptrs)
		return 0;

	size = sizeof(struct bdb_lfp_data);
	if (ptrs->panel_name.table_size)
		size = max(size, ptrs->panel_name.offset +
			   sizeof(struct bdb_lfp_data_tail));

	return size;
}

static int get_child_device_expected_size(u16 vbt_version)
{
	if (vbt_version > 264) return -1;
	else if (vbt_version >= 263) return 44;
	else if (vbt_version >= 256) return 40;
	else if (vbt_version >= 216) return 39;
	else if (vbt_version >= 196) return 38;
	else if (vbt_version >= 195) return 37;
	else if (vbt_version >= 111) return 33;
	else if (vbt_version >= 106) return 27;
	else return 22;
}

enum port {
	PORT_A = 0,
	PORT_B,
	PORT_C,
	PORT_D,
	PORT_E,
	PORT_F,
	PORT_G,
	PORT_H,
	PORT_I,
	PORT_TC1 = 10, // Type-C Port 1
	PORT_TC2,
	PORT_TC3,
	PORT_TC4,
	PORT_TC5,
	PORT_TC6,
	PORT_D_XELPD, // Special mapping for XeHPD
	PORT_E_XELPD,
	PORT_NONE,
};

enum phy {
	PHY_NONE = -1,

	PHY_A = 0,
	PHY_B,
	PHY_C,
	PHY_D,
	PHY_E,
	PHY_F,
	PHY_G,
	PHY_H,
	PHY_I,

	I915_MAX_PHYS
};
enum aux_ch {
	AUX_CH_NONE = -1,

	AUX_CH_A,
	AUX_CH_B,
	AUX_CH_C,
	AUX_CH_D,
	AUX_CH_E, /* ICL+ */
	AUX_CH_F,
	AUX_CH_G,
	AUX_CH_H,
	AUX_CH_I,

	/* tgl+ */
	AUX_CH_USBC1 = AUX_CH_D,
	AUX_CH_USBC2,
	AUX_CH_USBC3,
	AUX_CH_USBC4,
	AUX_CH_USBC5,
	AUX_CH_USBC6,

	/* XE_LPD repositions D/E offsets and bitfields */
	AUX_CH_D_XELPD = AUX_CH_USBC5,
	AUX_CH_E_XELPD,
};

static const u8 adlp_aux_ch_map[] = {
	[AUX_CH_A] = DP_AUX_A,
	[AUX_CH_B] = DP_AUX_B,
	[AUX_CH_C] = DP_AUX_C,
	[AUX_CH_D_XELPD] = DP_AUX_D,
	[AUX_CH_E_XELPD] = DP_AUX_E,
	[AUX_CH_USBC1] = DP_AUX_F,
	[AUX_CH_USBC2] = DP_AUX_G,
	[AUX_CH_USBC3] = DP_AUX_H,
	[AUX_CH_USBC4] = DP_AUX_I,
};

/*
 * ADL-S VBT uses PHY based mapping. Combo PHYs A,B,C,D,E
 * map to DDI A,TC1,TC2,TC3,TC4 respectively.
 */
static const u8 adls_aux_ch_map[] = {
	[AUX_CH_A] = DP_AUX_A,
	[AUX_CH_USBC1] = DP_AUX_B,
	[AUX_CH_USBC2] = DP_AUX_C,
	[AUX_CH_USBC3] = DP_AUX_D,
	[AUX_CH_USBC4] = DP_AUX_E,
};

/*
 * RKL/DG1 VBT uses PHY based mapping. Combo PHYs A,B,C,D
 * map to DDI A,B,TC1,TC2 respectively.
 */
static const u8 rkl_aux_ch_map[] = {
	[AUX_CH_A] = DP_AUX_A,
	[AUX_CH_B] = DP_AUX_B,
	[AUX_CH_USBC1] = DP_AUX_C,
	[AUX_CH_USBC2] = DP_AUX_D,
};

static const u8 direct_aux_ch_map[] = {
	[AUX_CH_A] = DP_AUX_A,
	[AUX_CH_B] = DP_AUX_B,
	[AUX_CH_C] = DP_AUX_C,
	[AUX_CH_D] = DP_AUX_D, /* aka AUX_CH_USBC1 */
	[AUX_CH_E] = DP_AUX_E, /* aka AUX_CH_USBC2 */
	[AUX_CH_F] = DP_AUX_F, /* aka AUX_CH_USBC3 */
	[AUX_CH_G] = DP_AUX_G, /* aka AUX_CH_USBC4 */
	[AUX_CH_H] = DP_AUX_H, /* aka AUX_CH_USBC5 */
	[AUX_CH_I] = DP_AUX_I, /* aka AUX_CH_USBC6 */
};

struct intel_display {
	int version; // For DISPLAY_VER macro
	struct {
		bool alderlake_s;
		bool dg1;
		bool rocketlake;
	} platform;
};

#define DISPLAY_VER(d) ((d)->version)

#define AUX_CH_NAME_BUFSIZE	6

static const char *aux_ch_name(struct intel_display *display,
				   char *buf, int size, enum aux_ch aux_ch)
{
	if (DISPLAY_VER(display) >= 13 && aux_ch >= AUX_CH_D_XELPD)
		snprintf(buf, size, "AUX_CH_%c", 'A' + aux_ch - AUX_CH_D_XELPD + AUX_CH_D);
	else if (DISPLAY_VER(display) >= 12 && aux_ch >= AUX_CH_USBC1)
		snprintf(buf, size, "AUX_CH_USBC%c", '1' + aux_ch - AUX_CH_USBC1);
	else
		snprintf(buf, size, "AUX_CH_%c", 'A' + aux_ch);

	return buf;
}

#define port_name(p) ((p) + '0')
static const char *intel_ddi_encoder_name(struct intel_display *display,
					  enum port port, char *s, int size)
{
	if (DISPLAY_VER(display) >= 13 && port >= PORT_D_XELPD) {
		snprintf(s, size, "DDI_%c",
				   port_name(port - PORT_D_XELPD + PORT_D));
	} else if (DISPLAY_VER(display) >= 12) {

		snprintf(s, size,  "DDI_%c",
				 port_name(port));
	} else if (DISPLAY_VER(display) >= 11) {

		snprintf(s, size,  "DDI_%c",
				   port_name(port));
	} else {
		snprintf(s, size,  "DDI_%c", port_name(port));
	}

	return (s);
}

static enum aux_ch map_aux_ch(struct intel_display *display, u8 aux_channel)
{
	const u8 *aux_ch_map;
	int i, n_entries;

	if (DISPLAY_VER(display) >= 13) {
		aux_ch_map = adlp_aux_ch_map;
		n_entries = ARRAY_SIZE(adlp_aux_ch_map);
	} else if (display->platform.alderlake_s) {
		aux_ch_map = adls_aux_ch_map;
		n_entries = ARRAY_SIZE(adls_aux_ch_map);
	} else if (display->platform.dg1 || display->platform.rocketlake) {
		aux_ch_map = rkl_aux_ch_map;
		n_entries = ARRAY_SIZE(rkl_aux_ch_map);
	} else {
		aux_ch_map = direct_aux_ch_map;
		n_entries = ARRAY_SIZE(direct_aux_ch_map);
	}

	for (i = 0; i < n_entries; i++) {
		if (aux_ch_map[i] == aux_channel)
			return static_cast<enum aux_ch>(i);
	}

	return AUX_CH_NONE;
}
static const char* phy_to_string(enum phy port)
{
	switch (port) {
		case PHY_A: return "PHY_A";
		case PHY_B: return "PHY_B";
		case PHY_C: return "PHY_C";
		case PHY_D: return "PHY_D";
		case PHY_E: return "PHY_E";
		case PHY_F: return "PHY_F";
		case PHY_G: return "PHY_G";
		case PHY_H: return "PHY_H";
		default: return "Unknown";
	}
}

static enum phy intel_port_to_phy(struct intel_display *display, enum port port)
{
	if (DISPLAY_VER(display) >= 13 && port >= PORT_D_XELPD)
			return static_cast<enum phy>(PHY_D + port - PORT_D_XELPD);
		else if (DISPLAY_VER(display) >= 13 && port >= PORT_TC1)
			return static_cast<enum phy>(PHY_F + port - PORT_TC1);
		else if (display->platform.alderlake_s && port >= PORT_TC1)
			return static_cast<enum phy>(PHY_B + port - PORT_TC1);
		else if ((display->platform.dg1 || display->platform.rocketlake) && port >= PORT_TC1)
			return static_cast<enum phy>(PHY_C + port - PORT_TC1);
		/*else if ((display->platform.jasperlake || display->platform.elkhartlake) &&
			 port == PORT_D)
			return PHY_A;*/

		return static_cast<enum phy>(PHY_A + port - PORT_A);
}

static enum port __dvo_port_to_port(int n_ports, int n_dvo,
					const int port_mapping[][3], u8 dvo_port)
{
	int i, j;

	for (i = 0; i < n_ports; i++) {
		for (j = 0; j < n_dvo; j++) {
			if (port_mapping[i][j] == -1)
				break;

			if (dvo_port == port_mapping[i][j])
				return static_cast<enum port>(i);
		}
	}

	return PORT_NONE;
}

static enum port dvo_port_to_port(struct intel_display *display, u8 dvo_port)
{

	static const int port_mapping[][3] = {
		[PORT_A] = { DVO_PORT_HDMIA, DVO_PORT_DPA, -1 },
		[PORT_B] = { DVO_PORT_HDMIB, DVO_PORT_DPB, -1 },
		[PORT_C] = { DVO_PORT_HDMIC, DVO_PORT_DPC, -1 },
		[PORT_D] = { DVO_PORT_HDMID, DVO_PORT_DPD, -1 },
		[PORT_E] = { DVO_PORT_HDMIE, DVO_PORT_DPE, DVO_PORT_CRT },
		[PORT_F] = { DVO_PORT_HDMIF, DVO_PORT_DPF, -1 },
		[PORT_G] = { DVO_PORT_HDMIG, DVO_PORT_DPG, -1 },
		[PORT_H] = { DVO_PORT_HDMIH, DVO_PORT_DPH, -1 },
		[PORT_I] = { DVO_PORT_HDMII, DVO_PORT_DPI, -1 },
	};

	static const int rkl_port_mapping[][3] = {
		[PORT_A] = { DVO_PORT_HDMIA, DVO_PORT_DPA, -1 },
		[PORT_B] = { DVO_PORT_HDMIB, DVO_PORT_DPB, -1 },
		[PORT_C] = { -1 },
		[PORT_TC1] = { DVO_PORT_HDMIC, DVO_PORT_DPC, -1 },
		[PORT_TC2] = { DVO_PORT_HDMID, DVO_PORT_DPD, -1 },
	};

	static const int adls_port_mapping[][3] = {
		[PORT_A] = { DVO_PORT_HDMIA, DVO_PORT_DPA, -1 },
		[PORT_B] = { -1 },
		[PORT_C] = { -1 },
		[PORT_TC1] = { DVO_PORT_HDMIB, DVO_PORT_DPB, -1 },
		[PORT_TC2] = { DVO_PORT_HDMIC, DVO_PORT_DPC, -1 },
		[PORT_TC3] = { DVO_PORT_HDMID, DVO_PORT_DPD, -1 },
		[PORT_TC4] = { DVO_PORT_HDMIE, DVO_PORT_DPE, -1 },
	};
	static const int xelpd_port_mapping[][3] = {
		[PORT_A] = { DVO_PORT_HDMIA, DVO_PORT_DPA, -1 },
		[PORT_B] = { DVO_PORT_HDMIB, DVO_PORT_DPB, -1 },
		[PORT_C] = { DVO_PORT_HDMIC, DVO_PORT_DPC, -1 },
		[PORT_D_XELPD] = { DVO_PORT_HDMID, DVO_PORT_DPD, -1 },
		[PORT_E_XELPD] = { DVO_PORT_HDMIE, DVO_PORT_DPE, -1 },
		[PORT_TC1] = { DVO_PORT_HDMIF, DVO_PORT_DPF, -1 },
		[PORT_TC2] = { DVO_PORT_HDMIG, DVO_PORT_DPG, -1 },
		[PORT_TC3] = { DVO_PORT_HDMIH, DVO_PORT_DPH, -1 },
		[PORT_TC4] = { DVO_PORT_HDMII, DVO_PORT_DPI, -1 },
	};


	if (DISPLAY_VER(display) >= 13)
		return __dvo_port_to_port(sizeof(xelpd_port_mapping) / sizeof(xelpd_port_mapping[0]),
					  3, // array size
					  xelpd_port_mapping,
					  dvo_port);
	else if (display->platform.alderlake_s)
		return __dvo_port_to_port(sizeof(adls_port_mapping) / sizeof(adls_port_mapping[0]),
					  3,
					  adls_port_mapping,
					  dvo_port);
	else if (display->platform.dg1 || display->platform.rocketlake)
		return __dvo_port_to_port(sizeof(rkl_port_mapping) / sizeof(rkl_port_mapping[0]),
					  3,
					  rkl_port_mapping,
					  dvo_port);
	else
		return __dvo_port_to_port(sizeof(port_mapping) / sizeof(port_mapping[0]),
					  3,
					  port_mapping,
					  dvo_port);
}

static const char* port_to_string(enum port port)
{
	switch (port) {
		case PORT_A: return "PORT_A";
		case PORT_B: return "PORT_B";
		case PORT_C: return "PORT_C";
		case PORT_D: return "PORT_D";
		case PORT_E: return "PORT_E";
		case PORT_F: return "PORT_F";
		case PORT_G: return "PORT_G";
		case PORT_H: return "PORT_H";
		case PORT_I: return "PORT_I";
		case PORT_TC1: return "PORT_TC1";
		case PORT_TC2: return "PORT_TC2";
		case PORT_TC3: return "PORT_TC3";
		case PORT_TC4: return "PORT_TC4";
		default: return "Unknown";
	}
}


static int vbt_get_panel_type(const struct bdb_header *bdb)
{
	const struct bdb_lfp_options *lfp_options;

	lfp_options = (struct bdb_lfp_options *)find_raw_section(bdb, BDB_LFP_OPTIONS);
	if (!lfp_options)
		return -1;

	if (lfp_options->panel_type > 0xf &&
		lfp_options->panel_type != 0xff) {
		return -1;
	}

	//if (devdata && devdata->child.handle == DEVICE_HANDLE_LFP2)
	//	return lfp_options->panel_type2;

	return lfp_options->panel_type;
}
enum intel_backlight_type {
	INTEL_BACKLIGHT_PMIC,
	INTEL_BACKLIGHT_LPSS,
	INTEL_BACKLIGHT_DISPLAY_DDI,
	INTEL_BACKLIGHT_DSI_DCS,
	INTEL_BACKLIGHT_PANEL_DRIVER_INTERFACE,
	INTEL_BACKLIGHT_VESA_EDP_AUX_INTERFACE,
};
struct intel_vbt_panel_data {
	void *lfp_vbt_mode; /* if any */
	void *sdvo_lvds_vbt_mode; /* if any */
	
	/* Feature bits */
	int panel_type;
	unsigned int lvds_dither:1;
	unsigned int bios_lvds_val; /* initial [PCH_]LVDS reg val in VBIOS */
	
	bool vrr;
	
	u8 seamless_drrs_min_refresh_rate;
	//enum drrs_type drrs_type;
	
	struct {
		int max_link_rate;
		int rate;
		int lanes;
		int preemphasis;
		int vswing;
		int bpp;
		//struct intel_pps_delays pps;
		u8 drrs_msa_timing_delay;
		bool low_vswing;
		bool hobl;
		bool dsc_disable;
		bool pipe_joiner_enable;
	} edp;
	
	struct {
		bool enable;
		bool full_link;
		bool require_aux_wakeup;
		int idle_frames;
		int tp1_wakeup_time_us;
		int tp2_tp3_wakeup_time_us;
		int psr2_tp2_tp3_wakeup_time_us;
	} psr;
	
	struct {
		u16 pwm_freq_hz;
		u16 brightness_precision_bits;
		u16 hdr_dpcd_refresh_timeout;
		bool present;
		bool active_low_pwm;
		u8 min_brightness;	/* min_brightness/255 of max */
		s8 controller;		/* brightness controller number */
		enum intel_backlight_type type;
	} backlight;
	
};

struct intel_panel {
	/* Simple drm_panel */
	void *base;

	/* Fixed EDID for eDP and LVDS. May hold ERR_PTR for invalid EDID. */
	void *fixed_edid;

	//struct list_head fixed_modes;

	/* backlight */
	struct {
		bool present;
		u32 level;
		u32 min;
		u32 max;
		bool enabled;
		bool combination_mode;	/* gen 2/4 only */
		bool active_low_pwm;
		bool alternate_pwm_increment;	/* lpt+ */

		/* PWM chip */
		u32 pwm_level_min;
		u32 pwm_level_max;
		bool pwm_enabled;
		bool util_pin_active_low;	/* bxt+ */
		u8 controller;		/* bxt+ only */
		void *pwm;
		//struct pwm_state pwm_state;

		/* DPCD backlight */
		union {
			struct {
				//struct drm_edp_backlight_info info;
				bool luminance_control_support;
			} vesa;
			struct {
				bool sdr_uses_aux;
				bool supports_2084_decode;
				bool supports_2020_gamut;
				bool supports_segmented_backlight;
				bool supports_sdp_colorimetry;
				bool supports_tone_mapping;
			} intel_cap;
		} edp;

		///struct backlight_device *device;

		//const struct intel_panel_bl_funcs *funcs;
		//const struct intel_panel_bl_funcs *pwm_funcs;
		//void (*power)(struct intel_connector *, bool enable);
	} backlight;

	struct intel_vbt_panel_data vbt;
};

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define DIV_ROUND_CLOSEST(x, divisor)		\
({							\
	__typeof__(x) __x = x;				\
	__typeof__(divisor) __d = divisor;		\
							\
	(((__typeof__(x))-1) > 0 ||			\
	 ((__typeof__(divisor))-1) > 0 ||		\
	 (((__x) > 0) == ((__d) > 0))) ?		\
		(((__x) + ((__d) / 2)) / (__d)) :	\
		(((__x) - ((__d) / 2)) / (__d));	\
})
#define KHz(x) (1000 * (x))
#define MHz(x) KHz(1000 * (x))

static u32 bxt_hz_to_pwm(u32 pwm_freq_hz)
{
	return DIV_ROUND_CLOSEST(KHz(19200), pwm_freq_hz);
}

static void
parse_lfp_backlight(IOPCIDevice *igpu, const struct bdb_header *bdb,
			struct intel_panel *panel)
{
	const struct bdb_lfp_backlight *backlight_data;
	const struct lfp_backlight_data_entry *entry;
	int panel_type = vbt_get_panel_type(bdb);
	u16 level;
	
	panel->vbt.panel_type=panel_type;

	backlight_data = (struct bdb_lfp_backlight *)find_raw_section(bdb, BDB_LFP_BACKLIGHT);
	if (!backlight_data)
		return;

	if (backlight_data->entry_size != sizeof(backlight_data->data[0])) {
		return;
	}

	entry = &backlight_data->data[panel_type];

	panel->vbt.backlight.present = entry->type == BDB_BACKLIGHT_TYPE_PWM;
	if (!panel->vbt.backlight.present) {
		return;
	}

	panel->vbt.backlight.type = INTEL_BACKLIGHT_DISPLAY_DDI;
	panel->vbt.backlight.controller = 0;
	if (bdb->version >= 191) {
		const struct lfp_backlight_control_method *method;

		method = &backlight_data->backlight_control[panel_type];
		
		panel->vbt.backlight.type = static_cast<enum intel_backlight_type>(method->type);
		panel->vbt.backlight.controller = method->controller;
	}
	
	panel->vbt.backlight.pwm_freq_hz = entry->pwm_freq_hz;
	panel->vbt.backlight.active_low_pwm = entry->active_low_pwm;

	if (bdb->version >= 234) {
		u16 min_level;
		bool scale;

		level = backlight_data->brightness_level[panel_type].level;
		min_level = backlight_data->brightness_min_level[panel_type].level;

		if (bdb->version >= 236)
			scale = backlight_data->brightness_precision_bits[panel_type] == 16;
		else
			scale = level > 255;

		if (scale)
			min_level = min_level / 255;

		if (min_level > 255) {
			level = 255;
		}
		panel->vbt.backlight.min_brightness = min_level;

		panel->vbt.backlight.brightness_precision_bits =
			backlight_data->brightness_precision_bits[panel_type];
	} else {
		level = backlight_data->level[panel_type];
		panel->vbt.backlight.min_brightness = entry->min_brightness;
	}

	if (bdb->version >= 239)
		panel->vbt.backlight.hdr_dpcd_refresh_timeout =
			DIV_ROUND_UP(backlight_data->hdr_dpcd_refresh_timeout[panel_type], 100);
	else
		panel->vbt.backlight.hdr_dpcd_refresh_timeout = 30;


	OSArray *connectorArray = OSArray::withCapacity(6);
	OSDictionary *connectorDict = OSDictionary::withCapacity(10);

	connectorDict->setObject("panel_type", OSNumber::withNumber(panel_type, 32));
	connectorDict->setObject("frequency hz", OSNumber::withNumber(panel->vbt.backlight.pwm_freq_hz, 32));

	connectorDict->setObject("active", OSString::withCString(panel->vbt.backlight.active_low_pwm ? "low" : "high"));
	connectorDict->setObject("min brightness", OSNumber::withNumber(panel->vbt.backlight.min_brightness, 32));
	connectorDict->setObject("level", OSNumber::withNumber(level, 32));
	connectorDict->setObject("controller", OSNumber::withNumber(panel->vbt.backlight.controller, 32));
	
	connectorArray->setObject(connectorDict);
	connectorDict->release();

	igpu->setProperty("PANEL", connectorArray);
	connectorArray->release();
	
	
}

static void init_bdb_block(IOPCIDevice *igpu, const struct bdb_header *bdb, int section_id, size_t min_size)
{
	const void *block_data = find_raw_section(bdb, section_id);

	if (!block_data) {
		// Block not found is not necessarily an error for optional blocks
		return;
	}

	u32 block_size = _get_blocksize(static_cast<const u8 *>(block_data) - 3);

	if (block_size < min_size) {
		return;
	}


	if (section_id == BDB_GENERAL_DEFINITIONS) {
			const struct bdb_general_definitions *defs = static_cast<const struct bdb_general_definitions *>(block_data);
			int entry_size = defs->child_dev_size;
			int child_device_num;
			int i;

			int expected_size = get_child_device_expected_size(bdb->version);
			if (expected_size > 0 && entry_size != expected_size) {
			}

			child_device_num = (block_size - sizeof(struct bdb_general_definitions)) / entry_size;

		
		uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
		asm volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(1));
		uint32_t family = (eax >> 8) & 0xF;
		uint32_t model = (eax >> 4) & 0xF;
		uint32_t extModel = (eax >> 16) & 0xF;
		uint32_t stepping = eax & 0xF;
		if (family == 0x6) {
			model |= (extModel << 4);
		}
		uint32_t cpuModel = model;
		bool isRealTGL = (model == 0x8C || model == 0x8D);
		bool isRKL = (model == 0xA7);
		bool isADL = (model == 0x97 || model == 0x9A || model == 0xBE);
		bool isRPL = (model == 0xB7 || model == 0xBA || model == 0xBF ||
					   model == 0xA6 || model == 0xAA);
		bool isMTL = (model == 0xAC || model == 0xAD);


		/* Display ver 12  \
		func(tigerlake) \
		func(tigerlake_uy) \
		func(rocketlake) \
		func(dg1) \
		func(alderlake_s) \
		func(alderlake_s_raptorlake_s) \
		 Display ver 13  \
		func(alderlake_p) \
		func(alderlake_p_alderlake_n) \
		func(alderlake_p_raptorlake_p) \
		func(alderlake_p_raptorlake_u) \
		func(dg2) \
		func(dg2_g10) \
		func(dg2_g11) \
		func(dg2_g12) \*/
		
		//TODO: check ip ver
		struct intel_display display_ctx;

		if (isMTL || isADL || isRPL) {
			display_ctx.version = 13;
		} else if (isRealTGL|| isRKL) {
			display_ctx.version = 12;
		} else {
			display_ctx.version = 12;
		}

		display_ctx.platform.alderlake_s = false; // needs detection
		display_ctx.platform.rocketlake = isRKL;
		display_ctx.platform.dg1 = false;
		
		

		OSArray *connectorArray = OSArray::withCapacity(6);
		
		for (i = 0; i < 6; i++) {
			bconnectors[i].index=i;
			bconnectors[i].busId=0;
			bconnectors[i].pipe=0;
			bconnectors[i].pad=0;
			bconnectors[i].type=ConnectorDummy;
			bconnectors[i].flags=0;
		}

		for (i = 0; i < child_device_num; i++) {
			const u8 *base = (u8 *)(defs);
			const struct child_device_config *child = reinterpret_cast<const struct child_device_config *>(
				base + sizeof(struct bdb_general_definitions) + (i * entry_size));

			if (child->device_type == 0)
				continue;

			enum port port = dvo_port_to_port(&display_ctx, child->dvo_port);
			
			if (port == PORT_NONE && (child->device_type & DEVICE_TYPE_MIPI_OUTPUT)) {
				if (child->dvo_port == DVO_PORT_MIPIA) port = PORT_A;
				else if (child->dvo_port == DVO_PORT_MIPIC) port = (display_ctx.version >= 11) ? PORT_B : PORT_C;
			}

			bool is_dvi   = (child->device_type & DEVICE_TYPE_TMDS_DVI_SIGNALING);
			bool is_hdmi  = is_dvi && !(child->device_type & DEVICE_TYPE_NOT_HDMI_OUTPUT);
			bool is_dp    = (child->device_type & DEVICE_TYPE_DISPLAYPORT_OUTPUT);
			bool is_edp   = is_dp && (child->device_type & DEVICE_TYPE_INTERNAL_CONNECTOR);
			bool is_dsi   = (child->device_type & DEVICE_TYPE_MIPI_OUTPUT);
			bool is_crt   = (child->device_type & DEVICE_TYPE_ANALOG_OUTPUT);
			enum phy phy = intel_port_to_phy(&display_ctx, port);
			enum aux_ch aux_ch = map_aux_ch(&display_ctx,child->aux_channel);
			char buf[AUX_CH_NAME_BUFSIZE+3];
			char buf2[6];
			struct intel_panel panel;
			
			parse_lfp_backlight(igpu,bdb,&panel);
			
			if (aux_ch ==AUX_CH_NONE) aux_ch = (enum aux_ch)port;
			
			OSDictionary *connectorDict = OSDictionary::withCapacity(10);

			connectorDict->setObject("Index", OSNumber::withNumber(i, 32));

			connectorDict->setObject("Port", OSString::withCString(port_to_string(port)));
			
			connectorDict->setObject("DVI", is_dvi ? kOSBooleanTrue : kOSBooleanFalse);
			connectorDict->setObject("HDMI", is_hdmi ? kOSBooleanTrue : kOSBooleanFalse);
			connectorDict->setObject("DP", is_dp ? kOSBooleanTrue : kOSBooleanFalse);
			connectorDict->setObject("eDP", is_edp ? kOSBooleanTrue : kOSBooleanFalse);
			connectorDict->setObject("DSI", is_dsi ? kOSBooleanTrue : kOSBooleanFalse);
			connectorDict->setObject("CRT", is_crt ? kOSBooleanTrue : kOSBooleanFalse);
			connectorDict->setObject("DDI", OSString::withCString(intel_ddi_encoder_name(&display_ctx, port,buf2, sizeof(buf2))));
			connectorDict->setObject("PHY", OSString::withCString(phy_to_string(phy)));
			connectorDict->setObject("AUX", OSString::withCString(aux_ch_name(&display_ctx, buf, sizeof(buf), aux_ch)));
			connectorDict->setObject("GMBUS", OSNumber::withNumber((unsigned long long)child->ddc_pin, 32));

			connectorArray->setObject(connectorDict);
			connectorDict->release();
			
			ConnectorType type=ConnectorDummy;
			if (is_dp) type=ConnectorDP;
			if (is_edp) type=ConnectorLVDS;
			if (is_hdmi) type=ConnectorHDMI;
			
			u32 flags=0;
			if (is_dp) flags=0x1+0x400;
			if (is_edp) flags=0x1+0x8+0x10;
			if (is_hdmi) flags=0x1+0x200;
			
			bconnectors[port].index=port;
			bconnectors[port].busId=child->ddc_pin;
			bconnectors[port].pipe=i;
			bconnectors[port].pad=0;
			bconnectors[port].type=type;
			bconnectors[port].flags=flags;
		}

		igpu->setProperty("Bios_Connectors", connectorArray);
		connectorArray->release();

		}
}

static void init_bdb_blocks(IOPCIDevice *igpu, const struct bdb_header *bdb)
{
	for (size_t i = 0; i < ARRAY_SIZE(bdb_blocks); i++) {
		int section_id = bdb_blocks[i].section_id;
		size_t min_size = bdb_blocks[i].min_size;

		if (section_id == BDB_LFP_DATA)
			min_size = lfp_data_min_size(bdb);

		init_bdb_block(igpu, bdb, section_id, min_size);
	}
}

int NBlue::intel_opregion_setup(IOPCIDevice *igpu)
{
	struct intel_opregion *opregion = &this->opregion;
	u32 asls, mboxes;
	char buf[sizeof(OPREGION_SIGNATURE)];
	int err = 0;
	void *base = nullptr;
	const void *vbt = nullptr;
	u32 vbt_size = 0;

	asls = igpu->configRead32(0xFC);
	
	if (asls == 0) {
		return -ENOMEM;
	}

	IOPhysicalAddress physAddr = asls;
	base = IOMallocPageable(OPREGION_SIZE, PAGE_SIZE);
	if (!base) {
		return -ENOMEM;
	}

	IOMemoryDescriptor *memDesc = IOMemoryDescriptor::withPhysicalAddress(
		physAddr, OPREGION_SIZE, kIODirectionInOut);
	
	if (!memDesc) {
		IOFreePageable(base, OPREGION_SIZE);
		return -ENOMEM;
	}

	IOMemoryMap *memMap = memDesc->map();
	if (!memMap) {
		memDesc->release();
		IOFreePageable(base, OPREGION_SIZE);
		return -ENOMEM;
	}

	mach_vm_address_t virtAddr = memMap->getVirtualAddress();
	
	memcpy(buf, (void *)virtAddr, sizeof(buf));
	if (memcmp(buf, OPREGION_SIGNATURE, 16)) {
		err = -EINVAL;
		goto err_out;
	}

	opregion->header = reinterpret_cast<struct opregion_header *>(virtAddr);


	mboxes = opregion->header->mboxes;
	
	if (mboxes & MBOX_ACPI) {
			opregion->acpi = (struct opregion_acpi *)((uint8_t *)virtAddr + OPREGION_ACPI_OFFSET);
			opregion->acpi->chpd = 1;
		}

		if (mboxes & MBOX_ASLE) {
			opregion->asle = (struct opregion_asle *)((uint8_t *)virtAddr + OPREGION_ASLE_OFFSET);
			opregion->asle->ardy = ASLE_ARDY_NOT_READY;
		}

		if (mboxes & MBOX_ASLE_EXT) {
			opregion->asle_ext = (struct opregion_asle_ext *)((uint8_t *)virtAddr + OPREGION_ASLE_EXT_OFFSET);
		}

		if (mboxes & MBOX_BACKLIGHT) {
		}

	if (opregion->header->over.major >= 2 && opregion->asle &&
		opregion->asle->rvda && opregion->asle->rvds) {
		resource_size_t rvda = opregion->asle->rvda;

		if (opregion->header->over.major > 2 ||
			opregion->header->over.minor >= 1) {
			rvda += asls;
		}

		IOMemoryDescriptor *rvdaDesc = IOMemoryDescriptor::withPhysicalAddress(
			rvda, opregion->asle->rvds, kIODirectionInOut);
		
		if (rvdaDesc) {
			IOMemoryMap *rvdaMap = rvdaDesc->map();
			if (rvdaMap) {
				vbt = reinterpret_cast<const void *>(rvdaMap->getVirtualAddress());
				vbt_size = opregion->asle->rvds;
				
				opregion->vbt = vbt;
				opregion->vbt_size = vbt_size;
				opregion->rvda = rvdaMap;
			}
			rvdaDesc->release();
		}
	}

	if (!opregion->vbt) {
		vbt = (void *)((uint8_t *)virtAddr + OPREGION_VBT_OFFSET);
		vbt_size = (mboxes & MBOX_ASLE_EXT) ?
			OPREGION_ASLE_EXT_OFFSET : OPREGION_SIZE;
		vbt_size -= OPREGION_VBT_OFFSET;
		
		opregion->vbt = vbt;
		opregion->vbt_size = vbt_size;
	}
	
	if (vbt) {
			const struct vbt_header *vbth = reinterpret_cast<struct vbt_header *>(const_cast<void *>(vbt));
			const char *base = static_cast<const char *>(vbt);
			const struct bdb_header *bdb = reinterpret_cast<const struct bdb_header *>(base + vbth->bdb_offset);

			if (bdb) {
				init_bdb_blocks(igpu, bdb);
			}
		}
	

err_out:
	if (memMap) memMap->release();
	if (memDesc) memDesc->release();
	if (base) IOFreePageable(base, OPREGION_SIZE);
	
	return err;
}

