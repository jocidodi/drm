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


constexpr UInt32 mmPCIE_INDEX2 = 0xE;
constexpr UInt32 mmPCIE_DATA2 = 0xF;
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
	
#pragma pack(push, 1)
struct vbt_header {
	char signature[4]; // '$VBT'
	uint16_t version;
	uint16_t header_size;
	uint16_t vbt_size;
	uint8_t vbt_checksum;
	uint8_t reserved0;
	uint32_t bdb_offset;
};

struct bdb_header {
	char signature[16]; // 'BIOS_DATA_BLOCK'
	uint16_t version;
	uint16_t header_size;
	uint16_t bdb_size;
};

struct child_device_config {
	uint16_t device_handle;
	uint16_t device_type;
	uint16_t aux_channel;
	uint8_t  dvo_port;
	uint8_t  i2c_pin;
	uint8_t  slave_addr;
	uint8_t  ddc_pin;
	uint16_t edid_ptr;
	uint8_t  dvo_cfg;
	uint8_t  dvo2_port;
	uint8_t  i2c2_pin;
	uint8_t  slave2_addr;
	uint8_t  ddc2_pin;
	uint8_t  capabilities;
	uint8_t  dvo_wiring;
	uint8_t  dvo2_wiring;
	uint8_t  extended_type;
	uint8_t  dvo_function;
	uint8_t  pipe_cap;
	uint8_t  hpd_status;
	uint16_t lspcon;
	uint8_t  aux_channel_2;
	uint8_t  flags[3];
};
#pragma pack(pop)

	struct PACKED ConnectorI {
		uint32_t index;
		uint32_t busId;
		uint32_t pipe;
		uint32_t pad;
		uint32_t type;
		uint32_t flags;
	};
	
static const void *get_block(const void *bdb_base, int block_id, uint16_t *block_size) {
	const struct bdb_header *bdb = (const struct bdb_header *)bdb_base;
	const uint8_t *base = (const uint8_t *)bdb_base;
	
	for (int offset = sizeof(struct bdb_header); offset < bdb->bdb_size; ) {
		const uint8_t *block_ptr = base + offset;
		uint8_t id = block_ptr[0];
		uint16_t size = *(uint16_t *)(block_ptr + 1);
		
		if (id == 0 || size == 0) break;
		if (id == block_id) {
			if (block_size) *block_size = size;
			return block_ptr + 3;
		}
		offset += 3 + size;
	}
	return nullptr;
}

static int parse_vbt_internal(const void *vbt_ptr, ConnectorI *connectors, int max_connectors) {
	if (!vbt_ptr) return 0;
	const struct vbt_header *vbt = (const struct vbt_header *)vbt_ptr;
	
	if (memcmp(vbt->signature, "$VBT", 4) != 0) return 0;
	
	const struct bdb_header *bdb = (const struct bdb_header *)((const uint8_t *)vbt + vbt->bdb_offset);
	if (memcmp(bdb->signature, "BIOS_DATA_BLOCK", 16) != 0) return 0;

	uint16_t block_size = 0;
	const uint8_t *block_data = (const uint8_t *)get_block(bdb, 1, &block_size);
	if (!block_data || block_size == 0) return 0;

	uint8_t child_dev_count = block_data[0];
	if (child_dev_count == 0) return 0;

	const int gen11_child_dev_size = 33;
	const struct child_device_config *child_dev =
		(const struct child_device_config *)(block_data + block_size - (child_dev_count * gen11_child_dev_size));

	int detected = 0;
	for (int i = 0; i < child_dev_count && detected < max_connectors; i++) {
		uint8_t dvo_port = child_dev[i].dvo_port;
		uint16_t dev_type = child_dev[i].device_type;
		if (dvo_port > 5) continue;

		uint32_t type = 1;
		uint16_t flags = 0;

		if (dev_type & 0x0004) {       // LVDS
			type = 2; flags = 0x18;
		} else if (dev_type & 0x0008) { // HDMI
			type = 0x800; flags = 0x801;
		} else if (dev_type & 0x0010) { // DP
			type = 0x400; flags = 0x201;
		} else if (dev_type & 0x0001) { // Integrated
			type = 2; flags = 0x18;
		}

		if (type != 1) {
			
			connectors[dvo_port].busId = 0;
			connectors[dvo_port].pad = 0;
			connectors[dvo_port].pipe = dvo_port;
			connectors[dvo_port].index = dvo_port;
			connectors[dvo_port].type = type;
			connectors[dvo_port].flags = flags;
			//connectors[index].is_connected = (child_dev[i].hpd_status > 0);
			detected++;
		}
	}
	return detected;
}


bool getVBIOSFromOpRegion() {
	
	auto* pciDevice = OSDynamicCast(IOPCIDevice, iGPU);
	if (!pciDevice) {
		return false;
	}

	uint32_t opRegionPhys = pciDevice->configRead32(0xFC);
	
	if (!opRegionPhys || (opRegionPhys & 0xFFF)) {
		return false;
	}

	auto opRegionDesc = IOMemoryDescriptor::withPhysicalAddress(opRegionPhys, 0x2000, kIODirectionOutIn);
	if (!opRegionDesc) {
		return false;
	}

	auto opRegionMap = opRegionDesc->map();
	if (!opRegionMap) {
		opRegionDesc->release();
		return false;
	}

	volatile uint8_t* opRegion = (uint8_t*)opRegionMap->getVirtualAddress();

	uint32_t vbtPhysAddr = *((uint32_t*)(opRegion + 0x1A0));

	opRegionMap->release();
	opRegionDesc->release();

	if (!vbtPhysAddr) {
		return false;
	}

	auto vbtDesc = IOMemoryDescriptor::withPhysicalAddress(vbtPhysAddr, 0x2000, kIODirectionOutIn);
	if (!vbtDesc) {
		return false;
	}

	auto vbtMap = vbtDesc->map();
	if (!vbtMap) {
		vbtDesc->release();
		return false;
	}

	struct vbt_header* vbt = (struct vbt_header*)vbtMap->getVirtualAddress();

	if (memcmp(vbt->signature, "$VBT", 4) != 0) {
		vbtMap->release();
		vbtDesc->release();
		return false;
	}

	uint32_t vbtSize = vbt->vbt_size;
	if (vbtSize == 0 || vbtSize > 0x2000) {
		vbtMap->release();
		vbtDesc->release();
		return false;
	}

	this->vbiosData = OSData::withBytes(vbt, vbtSize);

	vbtMap->release();
	vbtDesc->release();

	return (this->vbiosData != nullptr);
}

int detectConnectors(void *connectorsp) {
	if (!this->vbiosData) {
		return 0;
	}
	int max_connectors=6;
	ConnectorI *connectors=(ConnectorI*)connectorsp;
	UInt32 vbiosLen = this->vbiosData->getLength();
	const UInt8* vbiosPtr = static_cast<const UInt8*>(this->vbiosData->getBytesNoCopy());

	if (!vbiosPtr || vbiosLen < sizeof(struct vbt_header)) {
		return 0;
	}
	
	int count = parse_vbt_internal(vbiosPtr, connectors, max_connectors);
	
	return count;
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

//! Change frame-buffer count >= 2 check to >= 1.
static const UInt8 kAGDPFBCountCheckOriginal[] = {0x02, 0x00, 0x00, 0x83, 0xF8, 0x02};
static const UInt8 kAGDPFBCountCheckPatched[] = {0x02, 0x00, 0x00, 0x83, 0xF8, 0x01};

//! Ditto
static const UInt8 kAGDPFBCountCheckVenturaOriginal[] = {0x41, 0x83, 0xBE, 0x14, 0x02, 0x00, 0x00, 0x02};
static const UInt8 kAGDPFBCountCheckVenturaPatched[] = {0x41, 0x83, 0xBE, 0x14, 0x02, 0x00, 0x00, 0x01};

//! Neutralise access to AGDP configuration by board identifier.
static const UInt8 kAGDPBoardIDKeyOriginal[] = "board-id";
static const UInt8 kAGDPBoardIDKeyPatched[] =  "applehax";


