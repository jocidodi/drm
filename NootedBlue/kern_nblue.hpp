//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#pragma once

typedef unsigned int  IOSelect;
//#include "kern_netdbg.hpp"
#include <Headers/kern_patcher.hpp>
#include <Headers/kern_iokit.hpp>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/graphics/IOFramebuffer.h>
#include <IOKit/acpi/IOACPIPlatformExpert.h>


#define BIT(n) (1<< n)
#define REG_BIT(n) (1<< n)
#define   RING_FORCE_TO_NONPRIV_ACCESS_RW	(0 << 28)
#define __MASKED_FIELD(mask, value) ((mask) << 16 | (value))
#define _MASKED_FIELD(mask, value) ({ __MASKED_FIELD(mask, value); })
#define _MASKED_BIT_ENABLE(a)	({ __typeof(a) _a = (a); _MASKED_FIELD(_a, _a); })
#define _MASKED_BIT_DISABLE(a)	(_MASKED_FIELD((a), 0))

//! Hack
class AppleACPIPlatformExpert : IOACPIPlatformExpert {
	friend class NBlue;
};


/*
class EXPORT PRODUCT_NAME : public IOService {
	OSDeclareDefaultStructors(PRODUCT_NAME);

	public:
	IOService *probe(IOService *provider, SInt32 *score) override;
	bool start(IOService *provider) override;
};*/

class NBlue {
    friend class Gen11;
	friend class DYLDPatches;

    public:
    static NBlue *callback;
    void init();
    void processPatcher(KernelPatcher &patcher);
    bool processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size);
	void setRMMIOIfNecessary();
	
	static uint16_t configRead16(IORegistryEntry *service, uint32_t space, uint8_t offset);
	static uint32_t configRead32(IORegistryEntry *service, uint32_t space, uint8_t offset);
	WIOKit::t_PCIConfigRead16 orgConfigRead16 {nullptr};
	WIOKit::t_PCIConfigRead32 orgConfigRead32 {nullptr};
	
	
	static size_t wrapFunctionReturnZero();
	
	mach_vm_address_t orgApplePanelSetDisplay {0};
	static bool wrapApplePanelSetDisplay(IOService *that, IODisplay *display);
	
	UInt32 stolen_size;
	uint32_t framebufferId {0};
	
    private:
	
	
	inline UInt32 readReg32(unsigned long reg) {
		if (reg * sizeof(uint32_t) < this->rmmio->getLength()) {
			return this->rmmioPtr[reg];
		} else {
			return 0;
		}
	}
	
	inline void writeReg32(unsigned long reg, UInt32 val) {
		if ((reg * sizeof(uint32_t)) < this->rmmio->getLength()) {
			this->rmmioPtr[reg] = val;
		}
	}
	

bool getVBIOSFromOpRegion() {
	return false;
}
int detectConnectors(void *connectorsp) {
	return 0;
}
	
	static bool wrapAddDrivers(void* const self, OSArray* const array, const bool doNubMatching);
	mach_vm_address_t orgAddDrivers{0};
	
    uint32_t deviceId {0};
    uint16_t revision {0};
    uint32_t pciRevision {0};
    IOPCIDevice *iGPU {nullptr};
	bool isRealTGL = false;  
	uint32_t cpuModel {0};
	
	IOMemoryMap *rmmio {nullptr};
	volatile UInt32 *rmmioPtr {nullptr};
	OSData *vbiosData;

	
};


//! Neutralise access to AGDP configuration by board identifier.
static const UInt8 kAGDPBoardIDKeyOriginal[] = "board-id";
static const UInt8 kAGDPBoardIDKeyPatched[] =  "applehax";


