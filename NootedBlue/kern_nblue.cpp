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
        },this);
	
}


void NBlue::processPatcher(KernelPatcher &patcher) {
    auto *devInfo = DeviceInfo::create();
    if (devInfo) {
        devInfo->processSwitchOff();
		
        this->iGPU = OSDynamicCast(IOPCIDevice, devInfo->videoBuiltin);
        PANIC_COND(!this->iGPU, "nblue", "videoBuiltin is not IOPCIDevice");
		
		WIOKit::renameDevice(this->iGPU, "IGPU");
		WIOKit::awaitPublishing(this->iGPU);
		
		//this->iGPU->enablePCIPowerManagement(kPCIPMCSPowerStateD0);
		this->iGPU->setBusMasterEnable(true);
		this->iGPU->setMemoryEnable(true);
		//this->iGPU->setIOEnable(true);
		
        static uint8_t builtin[] = {0x00};
		
		/*static uint8_t builtin2[] = {0x00, 0x00, 0x52, 0x8A};
		static uint8_t builtin3[] = {0x52, 0x8A, 0x00, 0x00};
		int ok=0;
		//tgl
		if (ok)
		{
			builtin2[2]=0x49;
			builtin2[3]=0x9A;
			builtin3[0]=0x49;
			builtin3[1]=0x9A;
		}
		
		this->iGPU->setProperty("AAPL,ig-platform-id", builtin2, arrsize(builtin2));
		this->iGPU->setProperty("device-id", builtin3, arrsize(builtin3));
		*/
		this->iGPU->setProperty("built-in", builtin, arrsize(builtin));
		this->iGPU->setProperty("AAPL,slot-name", const_cast<char *>("built-in"), 9);
		this->iGPU->setProperty("hda-gfx", const_cast<char *>("onboard-1"), 10);
		this->iGPU->setProperty("model", const_cast<char *>("Intel Iris Xe Graphics"), 23);
			

        this->deviceId = WIOKit::readPCIConfigValue(this->iGPU, WIOKit::kIOPCIConfigDeviceID);
        this->pciRevision = WIOKit::readPCIConfigValue(NBlue::callback->iGPU, WIOKit::kIOPCIConfigRevisionID);

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
		
		
		int ok2=0;
			ok=0;
			vnode = NULLVP;
			ctxt = vfs_context_create(nullptr);
			err = vnode_lookup("/Library/Extensions/AppleIntelTGLGraphicsFramebuffer.kext/Contents/MacOS/AppleIntelTGLGraphicsFramebuffer", 0, &vnode, ctxt);
			if (!err) vnode_put(vnode);
			vfs_context_rele(ctxt);
			if (!err) ok=1;
			if (ok) ok2=1;
		
	
		int tcap=0;
		if (1) {

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
		}
		if (1) {
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
			else if (!ok2)
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
		this->rmmioLen=this->rmmio->getLength();
	}
}


bool NBlue::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
	
	if (kextIOAcceleratorFamily2.loadIndex == index) {
		
		return true;
	}  else if (kextIOGraphics.loadIndex == index) {

		return true;
	}  else if (kextAGDP.loadIndex == index) {
		
		const LookupPatchPlus boardIdPatch {&kextAGDP, kAGDPBoardIDKeyOriginal, kAGDPBoardIDKeyPatched, 1};
		SYSLOG_COND(!boardIdPatch.apply(patcher, address, size), "AGDP", "Failed to apply AGDP board-id patch");

		if (getKernelVersion() == KernelVersion::Ventura) {
			const LookupPatchPlus patch {&kextAGDP, kAGDPFBCountCheckOriginal13, kAGDPFBCountCheckPatched13, 1};
			SYSLOG_COND(!patch.apply(patcher, address, size), "AGDP", "Failed to apply AGDP FB count check patch");
		} else {
			const LookupPatchPlus patch {&kextAGDP, kAGDPFBCountCheckOriginal, kAGDPFBCountCheckPatched, 1};
			SYSLOG_COND(!patch.apply(patcher, address, size), "AGDP", "Failed to apply AGDP FB count check patch");
		}
		
		
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





u32 _get_blocksize(const u8 *block_base)
{
	if (*block_base == BDB_MIPI_SEQUENCE && *(block_base + 3) >= 3)
		return *((const u32 *)(block_base + 4));
	else
		return *((const u16 *)(block_base + 1));
}

const void *find_raw_section(const void *_bdb, int section_id)
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

size_t lfp_data_min_size(const struct bdb_header *bdb)
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

int get_child_device_expected_size(u16 vbt_version)
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
const char *aux_ch_name(struct intel_display *display,
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


const char *intel_ddi_encoder_name(struct intel_display *display,
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

enum aux_ch map_aux_ch(struct intel_display *display, u8 aux_channel)
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
const char* phy_to_string(enum phy port)
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

enum phy intel_port_to_phy(struct intel_display *display, enum port port)
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

enum port __dvo_port_to_port(int n_ports, int n_dvo,
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

enum port dvo_port_to_port(struct intel_display *display, u8 dvo_port)
{

	const int port_mapping[][3] = {
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

	const int rkl_port_mapping[][3] = {
		[PORT_A] = { DVO_PORT_HDMIA, DVO_PORT_DPA, -1 },
		[PORT_B] = { DVO_PORT_HDMIB, DVO_PORT_DPB, -1 },
		[PORT_C] = { -1 },
		[PORT_TC1] = { DVO_PORT_HDMIC, DVO_PORT_DPC, -1 },
		[PORT_TC2] = { DVO_PORT_HDMID, DVO_PORT_DPD, -1 },
	};

	const int adls_port_mapping[][3] = {
		[PORT_A] = { DVO_PORT_HDMIA, DVO_PORT_DPA, -1 },
		[PORT_B] = { -1 },
		[PORT_C] = { -1 },
		[PORT_TC1] = { DVO_PORT_HDMIB, DVO_PORT_DPB, -1 },
		[PORT_TC2] = { DVO_PORT_HDMIC, DVO_PORT_DPC, -1 },
		[PORT_TC3] = { DVO_PORT_HDMID, DVO_PORT_DPD, -1 },
		[PORT_TC4] = { DVO_PORT_HDMIE, DVO_PORT_DPE, -1 },
	};
	const int xelpd_port_mapping[][3] = {
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

const char* port_to_string(enum port port)
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


int vbt_get_panel_type(const struct bdb_header *bdb)
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



u32 bxt_hz_to_pwm(u32 pwm_freq_hz)
{
	return DIV_ROUND_CLOSEST(KHz(19200), pwm_freq_hz);
}

u32 cnp_hz_to_pwm(u32 rawclk_freq, u32 pwm_freq_hz)
{

	return DIV_ROUND_CLOSEST(KHz(rawclk_freq),pwm_freq_hz);
}

uint32_t scale(uint32_t source_val,
					  uint32_t source_min, uint32_t source_max,
					  uint32_t target_min, uint32_t target_max)
{
	if (source_val < source_min) {
		source_val = source_min;
	} else if (source_val > source_max) {
		source_val = source_max;
	}
	uint64_t source_range = static_cast<uint64_t>(source_max) - source_min;
	uint64_t target_range = static_cast<uint64_t>(target_max) - target_min;

	uint64_t target_val = (static_cast<uint64_t>(source_val) - source_min) * target_range;

	target_val = (target_val + (source_range / 2)) / source_range;

	target_val += target_min;

	return static_cast<uint32_t>(target_val);
}



int cnp_rawclk()
{
	int divider, fraction;
	u32 rawclk;

	if (NBlue::callback->readReg32( SFUSE_STRAP) & SFUSE_STRAP_RAW_FREQUENCY) {
		/* 24 MHz */
		divider = 24000;
		fraction = 0;
	} else {
		/* 19.2 MHz */
		divider = 19000;
		fraction = 200;
	}

	rawclk = CNP_RAWCLK_DIV(divider / 1000);
	if (fraction) {
		int numerator = 1;

		rawclk |= CNP_RAWCLK_DEN(DIV_ROUND_CLOSEST(numerator * 1000,
							   fraction) - 1);
		//if (INTEL_PCH_TYPE(display) >= PCH_ICP)
			rawclk |= ICP_RAWCLK_NUM(numerator);
	}
	
	NBlue::callback->writeReg32( PCH_RAWCLK_FREQ, rawclk);
	return divider + fraction;
}

bool cnp_backlight_controller_is_valid( int controller)
{
	if (controller < 0 || controller >= 2)
		return false;

	if (controller == 1)
		return NBlue::callback->readReg32( SOUTH_CHICKEN1) & ICP_SECOND_PPS_IO_SELECT;

	return true;
}


void bxt_set_backlight(struct intel_panel *panel, u32 level)
{
	NBlue::callback->writeReg32( BXT_BLC_PWM_DUTY(panel->backlight.controller), level);
}

void cnp_enable_backlight(struct intel_panel *panel, u32 level)
{
	u32 pwm_ctl;

	pwm_ctl = NBlue::callback->readReg32( BXT_BLC_PWM_CTL(panel->backlight.controller));
	if (pwm_ctl & BXT_BLC_PWM_ENABLE) {
		pwm_ctl &= ~BXT_BLC_PWM_ENABLE;
		NBlue::callback->writeReg32( BXT_BLC_PWM_CTL(panel->backlight.controller),
				   pwm_ctl);
	}

	NBlue::callback->writeReg32( BXT_BLC_PWM_FREQ(panel->backlight.controller),
			   panel->backlight.pwm_level_max);

	bxt_set_backlight(panel, level);

	pwm_ctl = 0;
	if (panel->backlight.active_low_pwm)
		pwm_ctl |= BXT_BLC_PWM_POLARITY;
	
	NBlue::callback->writeReg32( BXT_BLC_PWM_CTL(panel->backlight.controller), pwm_ctl);
	NBlue::callback->readReg32( BXT_BLC_PWM_CTL(panel->backlight.controller));
	NBlue::callback->writeReg32( BXT_BLC_PWM_CTL(panel->backlight.controller),
			   pwm_ctl | BXT_BLC_PWM_ENABLE);
}

unsigned int panel_bits(unsigned int value, int panel_type, int num_bits)
{
	return (value >> (panel_type * num_bits)) & (BIT(num_bits) - 1);
}

void vbt_edp_to_pps_delays(struct intel_pps_delays *pps,
				  const struct edp_power_seq *edp_pps)
{
	pps->power_up = edp_pps->t1_t3;
	pps->backlight_on = edp_pps->t8;
	pps->backlight_off = edp_pps->t9;
	pps->power_down = edp_pps->t10;
	pps->power_cycle = edp_pps->t11_t12;
}
bool panel_bool(unsigned int value, int panel_type)
{
	return panel_bits(value, panel_type, 1);
}

void
parse_edp(const struct bdb_header *bdb,
		  struct intel_panel *panel)
{
	const struct bdb_edp *edp;
	const struct edp_fast_link_params *edp_link_params;
	int panel_type = panel->vbt.panel_type;

	edp = (struct bdb_edp *)find_raw_section(bdb, BDB_EDP);
	
	if (!edp)
		return;

	switch (panel_bits(edp->color_depth, panel_type, 2)) {
	case EDP_18BPP:
		panel->vbt.edp.bpp = 18;
		break;
	case EDP_24BPP:
		panel->vbt.edp.bpp = 24;
		break;
	case EDP_30BPP:
		panel->vbt.edp.bpp = 30;
		break;
	}

	edp_link_params = &edp->fast_link_params[panel_type];

	vbt_edp_to_pps_delays(&panel->vbt.edp.pps,
				  &edp->power_seqs[panel_type]);

	if (bdb->version >= 224) {
		panel->vbt.edp.rate =
			edp->edp_fast_link_training_rate[panel_type] * 20;
	} else {
		switch (edp_link_params->rate) {
		case EDP_RATE_1_62:
			panel->vbt.edp.rate = 162000;
			break;
		case EDP_RATE_2_7:
			panel->vbt.edp.rate = 270000;
			break;
		case EDP_RATE_5_4:
			panel->vbt.edp.rate = 540000;
			break;
		default:
			break;
		}
	}

	switch (edp_link_params->lanes) {
	case EDP_LANE_1:
		panel->vbt.edp.lanes = 1;
		break;
	case EDP_LANE_2:
		panel->vbt.edp.lanes = 2;
		break;
	case EDP_LANE_4:
		panel->vbt.edp.lanes = 4;
		break;
	default:
		break;
	}


	
	switch (edp_link_params->preemphasis) {
	case EDP_PREEMPHASIS_NONE:
		panel->vbt.edp.preemphasis = DP_TRAIN_PRE_EMPH_LEVEL_0;
		break;
	case EDP_PREEMPHASIS_3_5dB:
		panel->vbt.edp.preemphasis = DP_TRAIN_PRE_EMPH_LEVEL_1;
		break;
	case EDP_PREEMPHASIS_6dB:
		panel->vbt.edp.preemphasis = DP_TRAIN_PRE_EMPH_LEVEL_2;
		break;
	case EDP_PREEMPHASIS_9_5dB:
		panel->vbt.edp.preemphasis = DP_TRAIN_PRE_EMPH_LEVEL_3;
		break;
	default:
		break;
	}

	switch (edp_link_params->vswing) {
	case EDP_VSWING_0_4V:
		panel->vbt.edp.vswing = DP_TRAIN_VOLTAGE_SWING_LEVEL_0;
		break;
	case EDP_VSWING_0_6V:
		panel->vbt.edp.vswing = DP_TRAIN_VOLTAGE_SWING_LEVEL_1;
		break;
	case EDP_VSWING_0_8V:
		panel->vbt.edp.vswing = DP_TRAIN_VOLTAGE_SWING_LEVEL_2;
		break;
	case EDP_VSWING_1_2V:
		panel->vbt.edp.vswing = DP_TRAIN_VOLTAGE_SWING_LEVEL_3;
		break;
	default:
		break;
	}

	if (bdb->version >= 173) {
		u8 vswing;

		/* Don't read from VBT if module parameter has valid value*/
		if (0/*display->params.edp_vswing*/) {
			//panel->vbt.edp.low_vswing =
				//display->params.edp_vswing == 1;
		} else {
			vswing = (edp->edp_vswing_preemph >> (panel_type * 4)) & 0xF;
			panel->vbt.edp.low_vswing = vswing == 0;
		}
	}

	panel->vbt.edp.drrs_msa_timing_delay =
		panel_bits(edp->sdrrs_msa_timing_delay, panel_type, 2);

	if (bdb->version >= 244)
		panel->vbt.edp.max_link_rate =
			edp->edp_max_port_link_rate[panel_type] * 20;

	if (bdb->version >= 251)
		panel->vbt.edp.dsc_disable =
			panel_bool(edp->edp_dsc_disable, panel_type);

	if (bdb->version >= 261)
		panel->vbt.edp.pipe_joiner_enable =
			panel_bool(edp->pipe_joiner_enable, panel_type);
}

int pps_units_to_msecs(int val)
{
	return DIV_ROUND_UP(val, 10);
}
int msecs_to_pps_units(int msecs)
{
	/* PPS uses 100us units */
	return msecs * 10;
}

 u32 ilk_get_pp_control(struct intel_panel *panel)
{
	u32 control;

	//lockdep_assert_held(&display->pps.mutex);
	control = NBlue::callback->readReg32(panel->regs.pp_ctrl);
	if ((control & PANEL_UNLOCK_MASK) != PANEL_UNLOCK_REGS) {
		control &= ~PANEL_UNLOCK_MASK;
		control |= PANEL_UNLOCK_REGS;
	}
	return control;
}




void
parse_lfp_backlight(struct intel_display *display,
			struct intel_panel *panel)
{
	const struct bdb_header *bdb=display->bdb;
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
	if (display->vbt.version >= 191) {
		const struct lfp_backlight_control_method *method;

		method = &backlight_data->backlight_control[panel_type];
		
		panel->vbt.backlight.type = static_cast<enum intel_backlight_type>(method->type);
		panel->vbt.backlight.controller = method->controller;
	}
	
	panel->vbt.backlight.pwm_freq_hz = entry->pwm_freq_hz;
	panel->vbt.backlight.active_low_pwm = entry->active_low_pwm;

	if (display->vbt.version >= 234) {
		u16 min_level;
		bool scale;

		level = backlight_data->brightness_level[panel_type].level;
		min_level = backlight_data->brightness_min_level[panel_type].level;

		if (display->vbt.version >= 236)
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

	if (display->vbt.version >= 239)
		panel->vbt.backlight.hdr_dpcd_refresh_timeout =
			DIV_ROUND_UP(backlight_data->hdr_dpcd_refresh_timeout[panel_type], 100);
	else
		panel->vbt.backlight.hdr_dpcd_refresh_timeout = 30;
	
	
	panel->backlight.controller = panel->vbt.backlight.controller;
	if (!cnp_backlight_controller_is_valid( panel->backlight.controller)) {
		panel->backlight.controller = 0;
	}
	panel->pps.pps_idx=panel->vbt.backlight.controller;
	
	if (panel->backlight.controller == 1) {
		u32 val = NBlue::callback->readReg32( UTIL_PIN_CTL);
		panel->backlight.util_pin_active_low =val & UTIL_PIN_POLARITY;
	}
	
	
	panel->rawclk_freq= cnp_rawclk();
	
	u32 pwm_ctl = NBlue::callback->readReg32(BXT_BLC_PWM_CTL(panel->backlight.controller));
	panel->backlight.active_low_pwm = pwm_ctl & BXT_BLC_PWM_POLARITY;
	panel->backlight.pwm_level_max = NBlue::callback->readReg32( BXT_BLC_PWM_FREQ(panel->backlight.controller));

	if (!panel->backlight.pwm_level_max)
		panel->backlight.pwm_level_max = cnp_hz_to_pwm(panel->rawclk_freq,panel->vbt.backlight.pwm_freq_hz);
	
	int val = static_cast<int>(panel->vbt.backlight.min_brightness);
	int vmin = (val < 0) ? 0 : (val > 64 ? 64 : val);
	
	panel->backlight.pwm_level_min = scale(vmin, 0, 255, 0, panel->backlight.pwm_level_max);
	
	panel->backlight.level=panel->backlight.pwm_level_max-(5*panel->vbt.backlight.min_brightness)*0x100;
	
	panel->backlight.pwm_enabled = pwm_ctl & BXT_BLC_PWM_ENABLE;
	
	//cnp_enable_backlight()
	//bxt_set_backlight()
	
	
	u32 pp_on, pp_off, pp_ctl, power_cycle_delay,pp_div,pp_sta;
	
	panel->regs.pp_ctrl = PP_CONTROL(panel, panel->pps.pps_idx);
	panel->regs.pp_stat = PP_STATUS(panel, panel->pps.pps_idx);
	panel->regs.pp_on = PP_ON_DELAYS(panel, panel->pps.pps_idx);
	panel->regs.pp_off = PP_OFF_DELAYS(panel, panel->pps.pps_idx);
	panel->regs.pp_div = PP_DIVISOR(panel, panel->pps.pps_idx);
	
	
	pp_ctl = NBlue::callback->readReg32(panel->regs.pp_ctrl);
	pp_on = NBlue::callback->readReg32( panel->regs.pp_on);
	pp_off = NBlue::callback->readReg32( panel->regs.pp_off);
	pp_div = NBlue::callback->readReg32( panel->regs.pp_div);
	pp_sta = NBlue::callback->readReg32( panel->regs.pp_stat);
	
	panel->pps.bios_pps_delays.power_up = REG_FIELD_GET(PANEL_POWER_UP_DELAY_MASK, pp_on);
	panel->pps.bios_pps_delays.backlight_on = REG_FIELD_GET(PANEL_LIGHT_ON_DELAY_MASK, pp_on);
	panel->pps.bios_pps_delays.backlight_off = REG_FIELD_GET(PANEL_LIGHT_OFF_DELAY_MASK, pp_off);
	panel->pps.bios_pps_delays.power_down = REG_FIELD_GET(PANEL_POWER_DOWN_DELAY_MASK, pp_off);

	if (NBlue::callback->readReg32(panel->regs.pp_div)!=-1) {
		u32 pp_div;
		pp_div = NBlue::callback->readReg32( panel->regs.pp_div);
		power_cycle_delay = REG_FIELD_GET(PANEL_POWER_CYCLE_DELAY_MASK, pp_div);
	} else {
		power_cycle_delay = REG_FIELD_GET(BXT_POWER_CYCLE_DELAY_MASK, pp_ctl);
	}
	
	panel->pps.bios_pps_delays.power_cycle = power_cycle_delay ? (power_cycle_delay - 1) * 1000 : 0;
	
	struct intel_pps_delays bios, vbt, spec;
	
	spec.power_up = msecs_to_pps_units(10 + 200);
	spec.backlight_on = msecs_to_pps_units(50);
	spec.backlight_off = msecs_to_pps_units(50);
	spec.power_down = msecs_to_pps_units(500);
	spec.power_cycle = msecs_to_pps_units(10+500);
	
	parse_edp(bdb,panel);
	vbt = panel->vbt.edp.pps;
	bios = panel->pps.bios_pps_delays;
	
	struct intel_pps_delays *finalb = &panel->pps.pps_delays;
	
#define assign_final(field)	finalb->field = (max(bios.field, vbt.field) == 0 ? \
					   spec.field : \
					   max(bios.field, vbt.field))
	assign_final(power_up);
	assign_final(backlight_on);
	assign_final(backlight_off);
	assign_final(power_down);
	assign_final(power_cycle);
#undef assign_final
	
	
	panel->pps.panel_power_up_delay = pps_units_to_msecs(finalb->power_up);
	panel->pps.backlight_on_delay = pps_units_to_msecs(finalb->backlight_on);
	panel->pps.backlight_off_delay = pps_units_to_msecs(finalb->backlight_off);
	panel->pps.panel_power_down_delay = pps_units_to_msecs(finalb->power_down);
	panel->pps.panel_power_cycle_delay = pps_units_to_msecs(finalb->power_cycle);
	
	finalb->power_cycle = roundup(finalb->power_cycle, msecs_to_pps_units(100));
	
	OSArray *connectorArray = OSArray::withCapacity(1);
	OSDictionary *connectorDict = OSDictionary::withCapacity(40);
	
	connectorDict->setObject("step_name", OSString::withCString( DISPLAY_RUNTIME_INFO(display)->step_name ));
	connectorDict->setObject("display_ver", OSNumber::withNumber(DISPLAY_VER(display), 32));
	
	connectorDict->setObject("panel_type", OSNumber::withNumber(panel_type, 32));
	connectorDict->setObject("pp_ctl", OSNumber::withNumber(pp_ctl, 32));
	connectorDict->setObject("pp_status", OSNumber::withNumber(pp_sta, 32));
	connectorDict->setObject("pp_on", OSNumber::withNumber(pp_on, 32));
	connectorDict->setObject("pp_off", OSNumber::withNumber(pp_off, 32));
	connectorDict->setObject("pp_div", OSNumber::withNumber(pp_div, 32));
	connectorDict->setObject("panel->rawclk_freq", OSNumber::withNumber(panel->rawclk_freq, 32));
	connectorDict->setObject("levelX", OSNumber::withNumber(panel->backlight.level, 32));
	connectorDict->setObject("vbt.pwm_freq_hz", OSNumber::withNumber(panel->vbt.backlight.pwm_freq_hz, 32));
	connectorDict->setObject("vbt.controller", OSNumber::withNumber(panel->vbt.backlight.controller, 32));
	connectorDict->setObject("vbt.min_brightness", OSNumber::withNumber(panel->vbt.backlight.min_brightness, 32));
	connectorDict->setObject("vbt.edp.bpp", OSNumber::withNumber(panel->vbt.edp.bpp, 32));
	connectorDict->setObject("vbt.edp.rate", OSNumber::withNumber(panel->vbt.edp.rate, 32));
	connectorDict->setObject("vbt.edp.lanes", OSNumber::withNumber(panel->vbt.edp.lanes, 32));
	
	connectorDict->setObject("pwm_enabled", OSNumber::withNumber(panel->backlight.pwm_enabled, 32));
	connectorDict->setObject("pwm_level_min", OSNumber::withNumber(panel->backlight.pwm_level_min, 32));
	connectorDict->setObject("pwm_level_max", OSNumber::withNumber(panel->backlight.pwm_level_max, 32));
	connectorDict->setObject("active_low_pwm", OSNumber::withNumber(panel->backlight.active_low_pwm , 32));
	
	connectorDict->setObject("pps->power_up", OSNumber::withNumber(panel->pps.pps_delays.power_up, 32));
	connectorDict->setObject("pps->power_down", OSNumber::withNumber(panel->pps.pps_delays.power_down, 32));
	connectorDict->setObject("pps->power_cycle", OSNumber::withNumber(panel->pps.pps_delays.power_cycle, 32));
	connectorDict->setObject("pps->backlight_on", OSNumber::withNumber(panel->pps.pps_delays.backlight_on, 32));
	connectorDict->setObject("pps->backlight_off", OSNumber::withNumber(panel->pps.pps_delays.backlight_off, 32));
	
	connectorDict->setObject("pps->round", OSNumber::withNumber(DIV_ROUND_UP(panel->pps.pps_delays.power_cycle, 1000) + 1, 32));
	
	
	
	connectorArray->setObject(connectorDict);
	connectorDict->release();

	NBlue::callback->iGPU->setProperty("PANEL", connectorArray);
	connectorArray->release();
	
	
}



void init_bdb_block(struct intel_display *display, const struct bdb_header *bdb, int section_id, size_t min_size)
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
		
		OSArray *connectorArray = OSArray::withCapacity(6);
		
		for (i = 0; i < 6; i++) {
			display->bconnectors[i].index=i;
			display->bconnectors[i].busId=0;
			display->bconnectors[i].pipe=0;
			display->bconnectors[i].pad=0;
			display->bconnectors[i].type=ConnectorDummy;
			display->bconnectors[i].flags=0;
		}

		for (i = 0; i < child_device_num; i++) {
			const u8 *base = (u8 *)(defs);
			const struct child_device_config *child = reinterpret_cast<const struct child_device_config *>(
				base + sizeof(struct bdb_general_definitions) + (i * entry_size));

			if (child->device_type == 0)
				continue;

			enum port port = dvo_port_to_port(display, child->dvo_port);
			
			if (port == PORT_NONE && (child->device_type & DEVICE_TYPE_MIPI_OUTPUT)) {
				if (child->dvo_port == DVO_PORT_MIPIA) port = PORT_A;
				else if (child->dvo_port == DVO_PORT_MIPIC) port = (DISPLAY_VER(display) >= 11) ? PORT_B : PORT_C;
			}

			bool is_dvi   = (child->device_type & DEVICE_TYPE_TMDS_DVI_SIGNALING);
			bool is_hdmi  = is_dvi && !(child->device_type & DEVICE_TYPE_NOT_HDMI_OUTPUT);
			bool is_dp    = (child->device_type & DEVICE_TYPE_DISPLAYPORT_OUTPUT);
			bool is_edp   = is_dp && (child->device_type & DEVICE_TYPE_INTERNAL_CONNECTOR);
			bool is_dsi   = (child->device_type & DEVICE_TYPE_MIPI_OUTPUT);
			bool is_crt   = (child->device_type & DEVICE_TYPE_ANALOG_OUTPUT);
			enum phy phy = intel_port_to_phy(display, port);
			enum aux_ch aux_ch = map_aux_ch(display,child->aux_channel);
			char buf[AUX_CH_NAME_BUFSIZE+3];
			char buf2[6];
			
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
			connectorDict->setObject("DDI", OSString::withCString(intel_ddi_encoder_name(display, port,buf2, sizeof(buf2))));
			connectorDict->setObject("PHY", OSString::withCString(phy_to_string(phy)));
			connectorDict->setObject("AUX", OSString::withCString(aux_ch_name(display, buf, sizeof(buf), aux_ch)));
			connectorDict->setObject("GMBUS", OSNumber::withNumber(child->ddc_pin, 32));
			
			connectorArray->setObject(connectorDict);
			connectorDict->release();
			
			ConnectorType type=ConnectorDummy;
			if (is_dp) type=ConnectorDP;
			if (is_edp) type=ConnectorLVDS;
			if (is_hdmi) type=ConnectorHDMI;
			
			u32 flags=0;
			if (is_dp) flags=0x1+0x400;
			if (is_edp) flags=0x1+0x0+0x10;// not builtin
			if (is_hdmi) flags=0x1+0x200;
			
			display->bconnectors[i].index=i;
			display->bconnectors[i].busId=child->ddc_pin;
			display->bconnectors[i].pipe=port;
			display->bconnectors[i].pad=0;
			display->bconnectors[i].type=type;
			display->bconnectors[i].flags=flags;
			
			if (i==0) display->bconnectors[i].pipe=1; //hack
		}

		NBlue::callback->iGPU->setProperty("Bios_Connectors", connectorArray);
		connectorArray->release();

		}
}

void init_bdb_blocks(struct intel_display *display)
{
	for (size_t i = 0; i < ARRAY_SIZE(bdb_blocks); i++) {
		int section_id = bdb_blocks[i].section_id;
		size_t min_size = bdb_blocks[i].min_size;

		if (section_id == BDB_LFP_DATA)
			min_size = lfp_data_min_size(display->bdb);

		init_bdb_block(display, display->bdb, section_id, min_size);
	}
}

static const struct platform_desc *find_platform_desc()
{
	u32 i;

	for (i = 0; i < ARRAY_SIZE(intel_display_ids); i++) {
		if (intel_display_ids[i].devid == NBlue::callback->deviceId)
			return intel_display_ids[i].desc;
	}

	return NULL;
}

static const struct subplatform_desc *
find_subplatform_desc( const struct platform_desc *desc)
{
	const struct subplatform_desc *sp;
	const u16 *id;

	for (sp = desc->subplatforms; sp && sp->pciidlist; sp++)
		for (id = sp->pciidlist; *id; id++)
			if (*id == NBlue::callback->deviceId)
				return sp;

	return NULL;
}


#define __STEP_NAME(name) [STEP_##name] = #name,

static void initialize_step(struct intel_display *display, enum intel_step step)
{
	static const char step_names[][3] = {
		STEP_NAME_LIST(__STEP_NAME)
	};

	DISPLAY_RUNTIME_INFO(display)->step = step;

	if (step >= 0 && step < ARRAY_SIZE(step_names))
		strcpy(DISPLAY_RUNTIME_INFO(display)->step_name, step_names[step],sizeof(DISPLAY_RUNTIME_INFO(display)->step_name));
}

#undef __STEP_NAME

static enum intel_step get_pre_gmdid_step(
					  const struct stepping_desc *main,
					  const struct stepping_desc *sub)
{
	const enum intel_step *map = main->map;
	int size = (int)main->size;
	int revision = NBlue::callback->pciRevision;
	enum intel_step step;

	if (sub && sub->map && sub->size) {
		map = sub->map;
		size = (int)sub->size;
	}

	if (!map || !size)
		return STEP_NONE;

	if (revision < size && map[revision] != STEP_NONE) {
		step = map[revision];
	} else {

		while (revision < size && map[revision] == STEP_NONE)
			revision++;

		if (revision < size) {
			step = map[revision];
		} else {
			step = STEP_FUTURE;
		}
	}

	return step;
}



void __bitmap_or(unsigned long *dst, const unsigned long *bitmap1,
		 const unsigned long *bitmap2, int bits)
{
	int k;
	int nr = BITS_TO_LONGS(bits);

	for (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

#define small_const_nbits(nbits) \
	(__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG && (nbits) > 0)
static void bitmap_or(unsigned long *dst, const unsigned long *src1,
		   const unsigned long *src2, unsigned int nbits)
{
	if (small_const_nbits(nbits))
		*dst = *src1 | *src2;
	else
		__bitmap_or(dst, src1, src2, nbits);
}
static unsigned int display_platforms_num_bits(void)
{
	return sizeof(((struct intel_display_platforms *)0)->bitmap) * BITS_PER_BYTE;
}
static void display_platforms_or(struct intel_display_platforms *dst,
				 const struct intel_display_platforms *src)
{
	bitmap_or(dst->bitmap, dst->bitmap, src->bitmap, display_platforms_num_bits());
}

struct intel_display *intel_display_device_probe(struct intel_display *display)
{
	const struct intel_display_device_info *info;
	const struct platform_desc *desc;
	const struct subplatform_desc *subdesc;
	enum intel_step step;


	desc = find_platform_desc();

	info = desc->info;

	DISPLAY_INFO(display) = info;

	memcpy(DISPLAY_RUNTIME_INFO(display),
		   &DISPLAY_INFO(display)->__runtime_defaults,
		   sizeof(*DISPLAY_RUNTIME_INFO(display)));


	display->platform = desc->platforms;

	subdesc = find_subplatform_desc( desc);
	if (subdesc) {

		display_platforms_or(&display->platform, &subdesc->platforms);
	}

	step = get_pre_gmdid_step( &desc->step_info,
					  subdesc ? &subdesc->step_info : NULL);
	

	initialize_step(display, step);


	return display;

}

int NBlue::intel_opregion_setup()
{
	struct intel_display *display=&display_base;
	struct intel_opregion *opregion = &display->opregion;
	display->dmc.dmc=&dmc0;
	display->dmc.dmc->display=display;
	display->dmc.dmc->max_fw_size=0x20000;
	
	u32 asls, mboxes;
	char buf[sizeof(OPREGION_SIGNATURE)];
	int err = 0;
	const void *vbt = nullptr;
	u32 vbt_size = 0;
	
	asls = iGPU->configRead32(0xFC);
	
	if (asls == 0) {
		return -ENOMEM;
	}

	IOPhysicalAddress physAddr = asls;
	IOMemoryDescriptor *memDesc = IOMemoryDescriptor::withPhysicalAddress(
		physAddr, OPREGION_SIZE, kIODirectionInOut);
	
	IOMemoryMap *memMap = memDesc->map();
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

				intel_display_device_probe(display);

				display->pps.mmio_base = PCH_PPS_BASE;
				display->panel.pps.mmio_base = PCH_PPS_BASE;
				display->vbt.version = bdb->version;
				display->bdb=bdb;
				init_bdb_blocks(display);
			}
		}
	

err_out:
	if (memMap) memMap->release();
	if (memDesc) memDesc->release();
	
	return err;
}

void NBlue::parse_backlight()
{
	parse_lfp_backlight(&display_base,&display_base.panel);
}

UInt32 NBlue::readReg32(unsigned long reg) {
	if (Gen11::callback)
	return Gen11::callback->raReadRegister32(nullptr,reg);
	
	if (reg < rmmioLen-4) {
		return this->rmmioPtr[reg];
	} else {
		return 0;
	}
}

void NBlue::writeReg32(unsigned long reg, UInt32 val) {
	if (Gen11::callback)
	return Gen11::callback->raWriteRegister32(nullptr,reg,val);
	
	if (reg < rmmioLen-4) {
		this->rmmioPtr[reg] = val;
	}
}

uint32_t NBlue::intel_de_rmw(uint32_t reg, uint32_t clear, uint32_t set)
{
	uint32_t old, val;
	old = readReg32( reg);
	val = (old & ~clear) | set;
	writeReg32( reg, val);
	return old;
}
