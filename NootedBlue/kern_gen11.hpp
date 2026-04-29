//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#ifndef kern_gen11_hpp
#define kern_gen11_hpp
#include "kern_nblue.hpp"
#include "Firmware.hpp"
#include "kern_patcherplus.hpp"
#include <Headers/kern_util.hpp>



typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;
__extension__ typedef __signed__ long long __s64;
__extension__ typedef unsigned long long __u64;

typedef __s8  s8;
typedef __u8  u8;
typedef __s16 s16;
typedef __u16 u16;
typedef __s32 s32;
typedef __u32 u32;
typedef __s64 s64;
typedef __u64 u64;


#define MTL_MEDIA_GSI_BASE		0x380000
#define GMD_ID_MEDIA				(MTL_MEDIA_GSI_BASE + 0xd8c)
#define GMD_ID_GRAPHICS				(0xd8c)

#define   GMD_ID_ARCH_MASK			REG_GENMASK(31, 22)
#define   GMD_ID_RELEASE_MASK			REG_GENMASK(21, 14)
#define   GMD_ID_STEP				REG_GENMASK(5, 0)
struct intel_ip_version {
	u8 ver;
	u8 rel;
	u8 step;
};
struct intel_step_info {
	/*
	 * It is expected to have 4 number steps per letter. Deviation from
	 * the expectation breaks gmd_to_intel_step().
	 */
	u8 graphics_step;	/* Represents the compute tile on Xe_HPC */
	u8 media_step;
};
#define I915_GEM_PPGTT_NONE	0
#define I915_GEM_PPGTT_ALIASING	1
#define I915_GEM_PPGTT_FULL	2
enum intel_ppgtt_type {
	INTEL_PPGTT_NONE = I915_GEM_PPGTT_NONE,
	INTEL_PPGTT_ALIASING = I915_GEM_PPGTT_ALIASING,
	INTEL_PPGTT_FULL = I915_GEM_PPGTT_FULL,
};
struct intel_runtime_info {
	/*
	 * Single "graphics" IP version that represents
	 * render, compute and copy behavior.
	 */
	struct {
		struct intel_ip_version ip;
	} graphics;
	struct {
		struct intel_ip_version ip;
	} media;

	/*
	 * Platform mask is used for optimizing or-ed IS_PLATFORM calls into
	 * single runtime conditionals, and also to provide groundwork for
	 * future per platform, or per SKU build optimizations.
	 *
	 * Array can be extended when necessary if the corresponding
	 * BUILD_BUG_ON is hit.
	 */
	u32 platform_mask[2];

	u16 device_id;

	struct intel_step_info step;

	unsigned int page_sizes; /* page sizes supported by the HW */

	enum intel_ppgtt_type ppgtt_type;
	unsigned int ppgtt_size; /* log2, e.g. 31/32/48 bits */

	bool has_pooled_eu;
};




#define BIT(n) (1<< n)
#define REG_BIT(n) (1<< n)
#define   RING_FORCE_TO_NONPRIV_ACCESS_RW	(0 << 28)
#define __MASKED_FIELD(mask, value) ((mask) << 16 | (value))
#define _MASKED_FIELD(mask, value) ({ __MASKED_FIELD(mask, value); })
#define _MASKED_BIT_ENABLE(a)	({ __typeof(a) _a = (a); _MASKED_FIELD(_a, _a); })
#define _MASKED_BIT_DISABLE(a)	(_MASKED_FIELD((a), 0))

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
enum {
	PROCMON_0_85V_DOT_0,
	PROCMON_0_95V_DOT_0,
	PROCMON_0_95V_DOT_1,
	PROCMON_1_05V_DOT_0,
	PROCMON_1_05V_DOT_1,
};

#define for_each_dbuf_slice(slice_mask, __slice) \
	for ((__slice) = DBUF_S1; (__slice) < I915_MAX_DBUF_SLICES; (__slice)++) \
		for_each_if(slice_mask & BIT(__slice))

#define for_each_if(condition) if (!(condition)) {} else

#define for_each_combo_phy(__display, __phy) \
for ((__phy) = PHY_A; (__phy) < I915_MAX_PHYS; (__phy)++)	\
	for_each_if(intel_phy_is_combo(__display, __phy))

static const struct icl_procmon {
	const char *name;
	uint32_t dw1, dw9, dw10;
} icl_procmon_values[] = {
	[PROCMON_0_85V_DOT_0] = {
		.name = "0.85V dot0 (low-voltage)",
		.dw1 = 0x00000000, .dw9 = 0x62AB67BB, .dw10 = 0x51914F96,
	},
	[PROCMON_0_95V_DOT_0] = {
		.name = "0.95V dot0",
		.dw1 = 0x00000000, .dw9 = 0x86E172C7, .dw10 = 0x77CA5EAB,
	},
	[PROCMON_0_95V_DOT_1] = {
		.name = "0.95V dot1",
		.dw1 = 0x00000000, .dw9 = 0x93F87FE1, .dw10 = 0x8AE871C5,
	},
	[PROCMON_1_05V_DOT_0] = {
		.name = "1.05V dot0",
		.dw1 = 0x00000000, .dw9 = 0x98FA82DD, .dw10 = 0x89E46DC1,
	},
	[PROCMON_1_05V_DOT_1] = {
		.name = "1.05V dot1",
		.dw1 = 0x00440000, .dw9 = 0x9A00AB25, .dw10 = 0x8AE38FF1,
	},
};


#define _PORT(port, a, b)		_PICK_EVEN(port, a, b)
#define _MMIO(r) (r)
#define _MMIO_PORT(port, a, b)		_MMIO(_PORT(port, a, b))
#define   PROCESS_INFO_MASK			REG_GENMASK(28, 26)
#define   PROCESS_INFO_DOT_0			REG_FIELD_PREP(PROCESS_INFO_MASK, 0)
#define   PROCESS_INFO_DOT_1			REG_FIELD_PREP(PROCESS_INFO_MASK, 1)
#define   PROCESS_INFO_DOT_4			REG_FIELD_PREP(PROCESS_INFO_MASK, 2)
#define   VOLTAGE_INFO_MASK			REG_GENMASK(25, 24)
#define   VOLTAGE_INFO_0_85V			REG_FIELD_PREP(VOLTAGE_INFO_MASK, 0)
#define   VOLTAGE_INFO_0_95V			REG_FIELD_PREP(VOLTAGE_INFO_MASK, 1)
#define   VOLTAGE_INFO_1_05V			REG_FIELD_PREP(VOLTAGE_INFO_MASK, 2)
#define _ICL_COMBOPHY_A				0x162000
#define _ICL_COMBOPHY_B				0x6C000
#define _EHL_COMBOPHY_C				0x160000
#define _RKL_COMBOPHY_D				0x161000
#define _ADL_COMBOPHY_E				0x16B000
#define _PICK(__index, ...) (((const uint32_t []){ __VA_ARGS__ })[__index])
#define _ICL_COMBOPHY(phy)			_PICK(phy, _ICL_COMBOPHY_A, \
						  _ICL_COMBOPHY_B, \
						  _EHL_COMBOPHY_C, \
						  _RKL_COMBOPHY_D, \
						  _ADL_COMBOPHY_E)
#define _ICL_PORT_COMP				0x100
#define _ICL_PORT_COMP_DW(dw, phy)		(_ICL_COMBOPHY(phy) + \
					 _ICL_PORT_COMP + 4 * (dw))
#define ICL_PORT_COMP_DW1(phy)			_MMIO(_ICL_PORT_COMP_DW(1, phy))
#define ICL_PORT_COMP_DW3(phy)			_MMIO(_ICL_PORT_COMP_DW(3, phy))

#define _ICL_PHY_MISC_A		0x64C00
#define _ICL_PHY_MISC_B		0x64C04
#define _DG2_PHY_MISC_TC1	0x64C14 /* TC1="PHY E" but offset as if "PHY F" */
#define ICL_PHY_MISC(port)	_MMIO_PORT(port, _ICL_PHY_MISC_A, _ICL_PHY_MISC_B)
#define  ICL_PHY_MISC_MUX_DDID			(1 << 28)
#define  ICL_PHY_MISC_DE_IO_COMP_PWR_DOWN	(1 << 23)
#define   ICL_PORT_TX_DW8_ODCC_CLK_DIV_SEL_MASK	REG_GENMASK(30, 29)
#define ICL_PORT_TX_DW8_AUX(phy)		_MMIO(_ICL_PORT_TX_DW_AUX(8, phy))
#define ICL_PORT_TX_DW8_GRP(phy)		_MMIO(_ICL_PORT_TX_DW_GRP(8, phy))
#define ICL_PORT_TX_DW8_LN(ln, phy)		_MMIO(_ICL_PORT_TX_DW_LN(8, ln, phy))
#define   ICL_PORT_TX_DW8_ODCC_CLK_SEL		REG_BIT(31)
#define   ICL_PORT_TX_DW8_ODCC_CLK_DIV_SEL_MASK	REG_GENMASK(30, 29)
#define   ICL_PORT_TX_DW8_ODCC_CLK_DIV_SEL_DIV2	REG_FIELD_PREP(ICL_PORT_TX_DW8_ODCC_CLK_DIV_SEL_MASK, 0x1)
#define ICL_PORT_PCS_DW1_LN(ln, phy)		_MMIO(_ICL_PORT_PCS_DW_LN(1, ln, phy))
#define   DCC_MODE_SELECT_MASK			REG_GENMASK(21, 20)
#define   RUN_DCC_ONCE				REG_FIELD_PREP(DCC_MODE_SELECT_MASK, 0)
#define ICL_PORT_COMP_DW8(phy)			_MMIO(_ICL_PORT_COMP_DW(8, phy))
#define ICL_PORT_PCS_DW1_GRP(phy)		_MMIO(_ICL_PORT_PCS_DW_GRP(1, phy))
#define ICL_PORT_COMP_DW0(phy)			_MMIO(_ICL_PORT_COMP_DW(0, phy))
#define ICL_PORT_CL_DW5(phy)			_MMIO(_ICL_PORT_CL_DW(5, phy))
#define   CL_POWER_DOWN_ENABLE			REG_BIT(4)
#define _ICL_PORT_TX_DW_LN(dw, ln, phy) 	(_ICL_COMBOPHY(phy) + \
					  _ICL_PORT_TX_LN(ln) + 4 * (dw))
#define _ICL_PORT_TX_LN(ln)			(0x880 + (ln) * 0x100)
#define _ICL_PORT_TX_DW_AUX(dw, phy)		(_ICL_COMBOPHY(phy) + \
					 _ICL_PORT_TX_AUX + 4 * (dw))
#define _ICL_PORT_TX_DW_GRP(dw, phy)		(_ICL_COMBOPHY(phy) + \
					 _ICL_PORT_TX_GRP + 4 * (dw))
#define _ICL_PORT_TX_AUX			0x380
#define _ICL_PORT_TX_GRP			0x680
#define _ICL_PORT_TX_LN(ln)			(0x880 + (ln) * 0x100)
#define _ICL_PORT_PCS_AUX			0x300
#define _ICL_PORT_PCS_GRP			0x600
#define _ICL_PORT_PCS_LN(ln)			(0x800 + (ln) * 0x100)
#define _ICL_PORT_PCS_DW_AUX(dw, phy)		(_ICL_COMBOPHY(phy) + \
					 _ICL_PORT_PCS_AUX + 4 * (dw))
#define _ICL_PORT_PCS_DW_GRP(dw, phy)		(_ICL_COMBOPHY(phy) + \
					 _ICL_PORT_PCS_GRP + 4 * (dw))
#define _ICL_PORT_PCS_DW_LN(dw, ln, phy)	 (_ICL_COMBOPHY(phy) + \
					  _ICL_PORT_PCS_LN(ln) + 4 * (dw))
#define   IREFGEN				REG_BIT(24)
#define   COMP_INIT				REG_BIT(31)
#define _ICL_PORT_CL_DW(dw, phy)		(_ICL_COMBOPHY(phy) + \
					 4 * (dw))
#define ICL_PORT_COMP_DW9(phy)			_MMIO(_ICL_PORT_COMP_DW(9, phy))
#define ICL_PORT_COMP_DW10(phy)			_MMIO(_ICL_PORT_COMP_DW(10, phy))

#define  PWR_DOWN_LN_MASK			REG_GENMASK(7, 4)
#define  PWR_UP_ALL_LANES			REG_FIELD_PREP(PWR_DOWN_LN_MASK, 0x0)
#define  PWR_DOWN_LN_3_2_1			REG_FIELD_PREP(PWR_DOWN_LN_MASK, 0xe)
#define  PWR_DOWN_LN_3_2			REG_FIELD_PREP(PWR_DOWN_LN_MASK, 0xc)
#define  PWR_DOWN_LN_3				REG_FIELD_PREP(PWR_DOWN_LN_MASK, 0x8)
#define  PWR_DOWN_LN_2_1_0			REG_FIELD_PREP(PWR_DOWN_LN_MASK, 0x7)
#define  PWR_DOWN_LN_1_0			REG_FIELD_PREP(PWR_DOWN_LN_MASK, 0x3)
#define  PWR_DOWN_LN_3_1			REG_FIELD_PREP(PWR_DOWN_LN_MASK, 0xa)
#define  PWR_DOWN_LN_3_1_0			REG_FIELD_PREP(PWR_DOWN_LN_MASK, 0xb)

#define ICL_PORT_CL_DW10(phy)			_MMIO(_ICL_PORT_CL_DW(10, phy))

struct PACKED PortConfig {
	uint32_t index;
	uint32_t busId;
	uint32_t pipe;
	uint32_t pad;
	uint32_t type;
	uint32_t flags;
};

struct  PortDriveConfig {
	uint32_t Voltage;
	uint16_t value;
	uint16_t  flags;
};
struct PACKED LaneStatusMasks {
	uint16_t maskLane1;   // 0x000F
	uint16_t reserved1;   // 0x00FF
	uint16_t reserved2;   // 0xFF0F
	uint16_t maskLane4;   // 0xFFFF
	uint8_t  padding1[8];
	uint8_t  tableChecksum[16];
};
struct PACKED TGL_ComboPhy_Registers {
	uint32_t reg_phy_ctl_0;       uint32_t reg_phy_ctl_1;
	uint32_t reg_dpll_stat_0;     uint32_t reg_dpll_stat_1;
	uint32_t reg_phy_misc_0;      uint32_t reg_phy_misc_1;
	uint32_t reg_phy_misc_2_0;    uint32_t reg_phy_misc_2_1;
	uint32_t reg_phy_misc_3_0;    uint32_t reg_phy_misc_3_1;
	uint32_t reg_misc_4_0;        uint32_t reg_misc_4_1;
	uint32_t reg_port_ctl_0;      uint32_t reg_port_ctl_1;
	uint32_t reg_voltage_info;    uint32_t reg_voltage_info_2; // 0x5B8
	uint32_t reg_comp_ctl_0;      uint32_t reg_comp_ctl_1;     // 0x5C4
	uint32_t reg_ddi_buf_ctl_0;   uint32_t reg_ddi_buf_ctl_1;  // 0x5C8
	uint32_t reg_bias_ctl_0;      uint32_t reg_bias_ctl_1;     // 0x5D4
	uint32_t reg_pcs_ctl_0;       uint32_t reg_pcs_ctl_1;      // 0x5D8
	uint32_t reg_tx_dwell_0;      uint32_t reg_tx_dwell_1;
	uint32_t reg_pat_ctl_0;       uint32_t reg_pat_ctl_1;
	uint32_t reg_tx_lane_0;       uint32_t reg_tx_lane_1;      // 0x5F0
	uint32_t reg_tx_lane_2;       uint32_t reg_tx_lane_3;      // 0x5F8
	uint32_t reg_loadgen_0;       uint32_t reg_loadgen_1;      // 0x604
	uint32_t reg_common_ctl_0;    uint32_t reg_common_ctl_1;    // 0x608
	uint32_t reg_parity_0;        uint32_t reg_parity_1;
	uint32_t reg_idle_ctl_0;      uint32_t reg_idle_ctl_1;      // 0x61C
	uint32_t reg_aux_0;           uint32_t reg_aux_1;          // 0x620
	uint32_t reg_ln0_trans_0;     uint32_t reg_ln0_trans_1;
	uint32_t reg_ln1_trans_0;     uint32_t reg_ln1_trans_1;
	uint32_t reg_ln2_trans_0;     uint32_t reg_ln2_trans_1;
	uint32_t reg_ln3_trans_0;     uint32_t reg_ln3_trans_1;
	uint32_t reg_misc_phy_0;      uint32_t reg_misc_phy_1;
	uint32_t reg_fia_0;           uint32_t reg_fia_1;
	uint32_t reg_fia_2;           uint32_t reg_fia_3;
	uint32_t reg_fia_4;           uint32_t reg_fia_5;
	uint32_t reg_fia_6;           uint32_t reg_fia_7;
	uint32_t reg_fia_8;           uint32_t reg_fia_9;
	uint32_t reg_fia_10;          uint32_t reg_fia_11;
	uint32_t reg_fia_12;          uint32_t reg_fia_13;
	uint32_t reg_fia_14;          uint32_t reg_fia_15;
	uint32_t reg_fia_16;          uint32_t reg_fia_17;
	uint32_t reg_fia_18;          uint32_t reg_fia_19;
	uint32_t reg_fia_20;          uint32_t reg_fia_21;
};
const uint8_t fLinkTrainingDriveTable[16] = {
	0x00, 0x04, 0x07, 0x09,
	0x01, 0x05, 0x08, 0x09,
	0x02, 0x06, 0x08, 0x09,
	0x03, 0x06, 0x08, 0x09
};

const LaneStatusMasks gLaneStatusMasks = {
	0x000F, 0x00FF, 0xFF0F, 0xFFFF,
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0xF1, 0xA1, 0xDD, 0x7C, 0xAC, 0x06, 0x4E, 0x78,
	  0x1F, 0xD6, 0x84, 0x38, 0x05, 0x2D, 0x6E, 0xB4 }
};

static const uint8_t gDPVcc0_85V[10][28]={
	// Level 0: (0,0)
	{ 0x90, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
	  0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },

	// Level 1: (1,0)
	{ 0x90, 0x01, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x15, 0x30, 0x00, 0x00,
	  0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },

	// Level 2: (1,1)
	{ 0x90, 0x01, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x12, 0x60, 0x00, 0x00,
	  0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },

	// Level 3: (2,0)
	{ 0x90, 0x01, 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0F, 0x90, 0x00, 0x00,
	  0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },

	// Level 4: (2,1)
	{ 0x58, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
	  0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },

	// Level 5: (2,2)
	{ 0x58, 0x02, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x15, 0x30, 0x00, 0x00,
	  0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },

	// Level 6: (3,0)
	{ 0x58, 0x02, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x12, 0x60, 0x00, 0x00,
	  0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },

	// Level 7: (3,1)
	{ 0x20, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },

	// Level 8: (3,2)
	{ 0x20, 0x03, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x15, 0x30, 0x00, 0x00,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },

	// Level 9: (3,3)
	{ 0xB0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
	  0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};


typedef unsigned char    byte;
#define DMC_DEFAULT_FW_OFFSET		0xFFFFFFFF
#define PACKAGE_MAX_FW_INFO_ENTRIES	20
#define PACKAGE_V2_MAX_FW_INFO_ENTRIES	32
#define DMC_V1_MAX_MMIO_COUNT		8
#define DMC_V3_MAX_MMIO_COUNT		20
#define DMC_V1_MMIO_START_RANGE		0x80000

struct PACKED intel_css_header
{
	uint32_t module_type;
	uint32_t header_len;
	uint32_t header_ver;
	uint32_t module_id;
	uint32_t module_vendor;
	uint32_t date;
	uint32_t size;
	uint32_t key_size;
	uint32_t modulus_size;
	uint32_t exponent_size;
	uint32_t reserved1[0xc];
	uint32_t version;
	uint32_t reserved2[0x8];
	uint32_t kernel_header_info;
};

struct PACKED intel_package_header
{
	uint8_t header_len;
	uint8_t header_ver;
	uint8_t reserved[0xa];
	uint32_t num_entries;
};

struct PACKED intel_fw_info
{
	uint8_t reserved1;
	uint8_t dmc_id;
	char stepping;
	char substepping;
	uint32_t offset;
	uint32_t reserved2;
};


struct PACKED intel_dmc_header_base {
	uint32_t signature;

	uint8_t header_len;

	uint8_t header_ver;

	uint16_t dmcc_ver;

	uint32_t project;

	uint32_t fw_size;

	uint32_t fw_version;
};

struct PACKED intel_dmc_header_v1 {
	struct intel_dmc_header_base base;

	uint32_t mmio_count;

	uint32_t mmioaddr[DMC_V1_MAX_MMIO_COUNT];

	uint32_t mmiodata[DMC_V1_MAX_MMIO_COUNT];

	char dfile[32];

	uint32_t reserved1[2];
};

struct PACKED intel_dmc_header_v3 {
	struct intel_dmc_header_base base;

	uint32_t start_mmioaddr;

	uint32_t reserved[9];

	char dfile[32];

	uint32_t mmio_count;

	uint32_t mmioaddr[DMC_V3_MAX_MMIO_COUNT];

	uint32_t mmiodata[DMC_V3_MAX_MMIO_COUNT];
};

struct PACKED DPCD_BranchOuiData {
	/* DPCD 0x400 - 0x402: Intel OUI (00:10:FA) */
	byte oui_0;          // local_51: Expected 0x00
	byte oui_1;          // local_50: Expected 0x10
	byte oui_2;          // local_4f: Expected 0xFA (-6 as signed char)

	/* DPCD 0x403 - 0x408: Vendor Specific Data / Panel ID */
	byte panel_char_1;   // local_4e: Expected 'e'
	byte panel_char_2;   // local_4d: Expected 'D'
	byte panel_hex_byte; // local_4c: Printed as <0x%02x>
	byte panel_char_3;   // local_4b: Expected 'f' (BanksiaL) or 'g' (Banksia)
	byte panel_char_4;   // local_4a
	byte panel_char_5;   // local_49
} ;

struct PACKED crtparams
{
	int32_t TRANS_CLK_SEL;
	int32_t TRANS_DDI_FUNC_CTL;
	int32_t TRANS_DDI_FUNC_CTL2;
	int32_t TRANS_MSA_MISC;
	int32_t TRANS_HTOTAL;
	int32_t TRANS_HBLANK;
	int32_t TRANS_HSYNC;
	int32_t TRANS_VTOTAL;
	int32_t TRANS_VBLANK;
	int32_t TRANS_VSYNC;
	int32_t PIPE_SRCSZ;
	int32_t TRANS_CONF;
	int32_t PIPE_MISC;
	int32_t TRANS_VSYNCSHIFT;
	int32_t TRANS_DATAM1;
	 char _3c[4];
	int32_t TRANS_LINKM1;
	 char _44[8];
	int32_t field_4c;
	int32_t field_50;
};

#define POPCOUNT(x) ({ \
	uint32_t _v = (x); \
	uint32_t _c = 0; \
	while (_v) { \
		_c++; \
		_v &= _v - 1; \
	} \
	_c; \
})
struct PACKED VoltageConfig {
	uint64_t pad;
	uint8_t* voltageData;
};

struct AGDCFBOnline_t {
	int32_t                 id;
	uint32_t                state;
};

enum ConnectorType : uint32_t {
	ConnectorZero       = 0x0,
	ConnectorDummy      = 0x1,
	ConnectorLVDS       = 0x2,
	ConnectorDigitalDVI = 0x4,
	ConnectorSVID       = 0x8,
	ConnectorVGA        = 0x10,
	ConnectorDP         = 0x400,
	ConnectorHDMI       = 0x800,
	ConnectorAnalogDVI  = 0x2000
};


union ConnectorFlags {
	struct PACKED ConnectorFlagBits {
		/* 0x1 */
		/* Lets aperture memory to be not required AppleIntelFramebuffer::isApertureMemoryRequired */
		uint8_t CNAlterAppertureRequirements :1;  /* 0x1 */
		
		/* 0x2 - Fixed Name */
		/* Commonly indicates a secondary port or auxiliary connector. */
		uint8_t CNFlagSecondary             :1;  /* 0x2 */
		
		/* 0x4 - Fixed Name */
		/* Forces the driver to assume an EDID is present or validate strictly. */
		uint8_t CNFlagForceEDID             :1;  /* 0x4 */
		
		/* 0x8 */
		/* Normally set for LVDS displays (i.e. built-in displays) */
		uint8_t CNConnectorAlwaysConnected   :1;  /* 0x8 */
		
		/* 0x10 - Fixed Name */
		/* AppleIntelFramebuffer::maxSupportedDepths checks this and returns 2 IODisplayModeInformation::maxDepthIndex.
		 * This enables 32-bit color depth support (High Color). */
		uint8_t CNSupport32BPP              :1;  /* 0x10 */
		
		/* 0x20 - Fixed Name */
		/* Often indicates legacy port or specific audio enablement requirements on older generations. */
		uint8_t CNFlagLegacy                :1;  /* 0x20 */
		
		/* 0x40 */
		/* Disable blit translation table? AppleIntelFramebufferController::ConfigureBufferTranslation */
		uint8_t CNDisableBlitTranslationTable:1;  /* 0x40 */
		
		/* 0x80 */
		/* Used in AppleIntelFramebufferController::setPowerWellState */
		/* Activates MISC IO power well (SKL_DISP_PW_MISC_IO) */
		uint8_t CNUseMiscIoPowerWell        :1;  /* 0x80 */
		
		/* 0x100 */
		/* Used in AppleIntelFramebufferController::setPowerWellState */
		/* Activates Power Well 2 usage (SKL_PW_CTL_IDX_PW_2) */
		uint8_t CNUsePowerWell2             :1;  /* 0x100 */
		
		/* 0x200 - Fixed Name */
		/* Detected in patchConnectorType logic. Indicates HDMI connector type. */
		uint8_t CNFlagHDMI                  :1;  /* 0x200 */
		
		/* 0x400 - Fixed Name */
		/* Detected in patchConnectorType logic. Indicates DisplayPort connector type. */
		uint8_t CNFlagDP                    :1;  /* 0x400 */
		
		/* 0x800 */
		/* Sets fAvailableLaneCount to 30 instead of 20 when specified. Required for 4K resolutions. */
		uint8_t CNIncreaseLaneCount         :1;  /* 0x800 */
		
		/* 0x1000 - Fixed Name */
		/* Commonly associated with Dual-Link DVI or high-bandwidth modes. */
		uint8_t CNFlagDualLink              :1;  /* 0x1000 */
		
		/* 0x2000 - Fixed Name */
		/* Often set for ports that require specific hot-plug handling or are defined as "Internal" on newer platforms. */
		uint8_t CNFlagInternalOverride      :1;  /* 0x2000 */
		
		/* 0x4000 - Fixed Name */
		/* Sometimes associated with specific encoder types (e.g., FDI or specific Transcoder links). */
		uint8_t CNFlagEncoderSpecific       :1;  /* 0x4000 */
		
		/* 0x8000 - Fixed Name */
		/* Indicates that Hot Plug Detect (HPD) is not connected or should be ignored. */
		uint8_t CNFlagNoHPD                 :1;  /* 0x8000 */
		
		uint16_t CNUnknownZeroFlags;
	} bits;
	uint32_t value;
};


struct PACKED ConnectorEntry {
	uint32_t index;
	uint32_t busId;
	uint32_t pipe;
	uint32_t pad;
	uint32_t type;
	uint32_t flags;
};

struct PACKED PlatformInfo {

		uint32_t fInfoPlatformID;
		uint32_t fPchType;
		uint64_t* fModelNameAddr;
		uint8_t fMobile;
		uint8_t fPipeCount;
		uint8_t fInfoPortCount;
		uint8_t fInfoFramebufferCount;
		uint32_t fInfoFramebufferMemorySize;
		uint32_t fInfoFBCompressionMemorySize;
		uint32_t fUnifiedMemorySize;
		struct ConnectorEntry connectors[0x9];
		uint32_t fInfoFlags;
		uint32_t pad0;
		struct VoltageConfig voltages[0x9];
		uint32_t cameliav;
		uint32_t MCLK;
		uint32_t VCLK;
		uint32_t TCONfbindex;
		uint32_t BeaconLoc;
		uint32_t MinFrmTim;
		uint32_t MaxFrmTim;
		uint32_t fInfoDynamicFBCThreshold;
		uint32_t fVideoTurboFreq;
		uint32_t fSliceCount;
		uint32_t fmaxEuCount;
		uint32_t fsubslices;

};

union FramebufferFlags {
	struct FramebufferFlagBits {
		/* Discovered in AppleIntelFBController::LinkTraining. Disables the use of FastLinkTraining.
		 * According to joevt with zero SKL link training happens at 450 MHz else at 540 MHz */
		uint8_t FBAvoidFastLinkTraining      :1;  /* 0x1 */
		uint8_t FBUnknownFlag_2              :1;  /* 0x2 */
		/* Discovered in AppleIntelFramebufferController::getFeatureControl.
		 * This is equivalent to setting FBC=1 in the plist FeatureControl section. */
		uint8_t FBFramebufferCompression     :1;  /* 0x4 */
		/* Discovered in AppleIntelFramebufferController::getFeatureControl.
		 * This is equivalent to setting SliceSDEnable=1, EUSDEnable=1, DynamicSliceSwitch=1 in the plist FeatureControl section.
		 */
		uint8_t FBEnableSliceFeatures        :1;  /* 0x8 */
		/* Discovered in AppleIntelFramebufferController::getFeatureControl.
		 * This is equivalent to setting DynamicFBCEnable=1 in the plist FeatureControl section.
		 */
		uint8_t FBDynamicFBCEnable           :1;  /* 0x10 */
		/* This sets fUseVideoTurbo=1 and loads GPU turbo frequency from the specific field.
		 * Defaults to 14, can be overridden by VideoTurboFreq in the plist FeatureControl section.
		 */
		uint8_t FBUseVideoTurbo              :1;  /* 0x20 */
		/* Discovered in AppleIntelFramebuffer::getDisplayStatus.
		 * Enforces display power reset even on always connected displays (see connector flags CNConnectorAlwaysConnected).
		 */
		uint8_t FBForcePowerAlwaysConnected  :1;  /* 0x40 */
		/* According to joevt this enforces High Bitrate mode 1, which limits DP bitrate to 8.64 Gbit/s instead of normal 17.28 Gbit/s (HBR2).
		 * I do not think this is used on Skylake any longer.
		 */
		uint8_t FBDisableHighBitrateMode2    :1;  /* 0x80 */
		/* This bit is not used on Broadwell and newer but set when fPortCount > 0, i.e. for all online framebuffers.
		 * On Haswell it is used by AppleIntelFramebuffer::GetOnlineInfo and is set only on 0x0D260007 (MacBookPro11,3) and 0x0D26000E, which are top models.
		 * This appears to boost pixel frequency limit (aka pixel clock) to 540000000 Hz (from the usual 216000000, 320000000, 360000000, 450000000).
		 */
		uint8_t FBBoostPixelFrequencyLimit   :1;  /* 0x100 */
		/* Discovered in AppleIntelFramebuffer::ValidateSourceSize.
		 * Limits source size to 4096x4096.
		 */
		uint8_t FBLimit4KSourceSize          :1;  /* 0x200 */
		/* Discovered in AppleIntelFramebufferController::start.
		 * These bits appear to be entirely equivalent and could be used interchangeably. Result in setting:
		 * - PCH_LP_PARTITION_LEVEL_DISABLE (1 << 12) bit in SOUTH_DSPCLK_GATE_D (0xc2020)
		 * - LPT_PWM_GRANULARITY (1 << 5) bit in SOUTH_CHICKEN2 (0xc2004)
		 * See Linux driver sources (lpt_init_clock_gating, lpt_enable_backlight).
		 * Since these bits are setting backlight pulse width modularity, there is no sense in setting them without a built-in display (i.e. on desktop).
		 */
		uint8_t FBAlternatePWMIncrement1     :1;  /* 0x400 */
		uint8_t FBAlternatePWMIncrement2     :1;  /* 0x800 */
		/* Discovered in Broadwell AppleIntelFBController::start / AppleIntelFBController::getFeatureControl.
		 * This is equivalent to setting DisableFeatureIPS=1 in the plist FeatureControl section.
		 * IPS stands for Intermediate Pixel Storage
		 */
		uint8_t FBDisableFeatureIPS          :1;  /* 0x1000 */
		uint8_t FBUnknownFlag_2000           :1;  /* 0x2000 */
		/* Discovered in Broadwell AppleIntelFBController::getOSInformation.
		 * Used by AppleIntelFramebufferController::LinkTraining for camellia version 2.
		 * Can be overridden by -notconrecover boot-arg, which effectively unsets this bit.
		 */
		uint8_t FBAllowConnectorRecover      :1;  /* 0x4000 */
		uint8_t FBUnknownFlag_8000           :1;  /* 0x8000 */
		uint8_t FBUnknownFlag_10000          :1;  /* 0x10000 */
		uint8_t FBUnknownFlag_20000          :1;  /* 0x20000 */
		/* Discovered in AppleIntelFramebufferController::getFeatureControl.
		 * This takes its effect only if GFMPPFM in the plist FeatureControl section is set to 2, otherwise GFMPPFM is off.
		 */
		uint8_t FBDisableGFMPPFM             :1;  /* 0x40000 */
		uint8_t FBUnknownFlag_80000          :1;  /* 0x80000 */
		uint8_t FBUnknownFlag_100000         :1;  /* 0x100000 */
		/* Discovered in AppleIntelFramebufferController::getFeatureControl.
		 * This takes its effect only if SupportDynamicCDClk in the plist FeatureControl section is set to 1, otherwise off.
		 * Also requires dc6config to be set to 3 (default).
		 */
		uint8_t FBEnableDynamicCDCLK         :1;  /* 0x200000 */
		uint8_t FBUnknownFlag_400000         :1;  /* 0x400000 */
		/* Discovered in AppleIntelFramebuffer::enableController.
		 * Used by AppleIntelFramebuffer::ValidateSourceSize.
		 * Setting this bit increases the maximum source size from 4096x4096 to 5120x5120.
		 * Most likely this enables 5K support via Intel HD.
		 */
		uint8_t FBSupport5KSourceSize        :1;  /* 0x800000 */
		uint8_t FBUknownZeroFlags;
	} bits;
	uint32_t value;
};


enum FramebufferFlags2
{
	
	// 0x1
	FB_FLAG_AVOID_FAST_LINK_TRAINING = 0x1,

	// 0x2 (Context: fInfoFlags[2])
	// Used to force backlight control via CPU registers rather than standard PWM
	FB_FLAG_ENABLE_BACKLIGHT_REG_CONTROL = 0x2,

	// 0x4
	FB_FLAG_FRAMEBUFFER_COMPRESSION = 0x4,

	// 0x8
	FB_FLAG_ENABLE_SLICE_FEATURES = 0x8,

	// 0x10
	FB_FLAG_DYNAMIC_FBC_ENABLE = 0x10,

	// 0x20
	FB_FLAG_USE_VIDEO_TURBO = 0x20,

	// 0x40
	FB_FLAG_FORCE_POWER_ALWAYS_CONNECTED = 0x40,

	// 0x80
	FB_FLAG_DISABLE_HIGH_BITRATE_MODE2 = 0x80,

	// 0x100
	FB_FLAG_BOOST_PIXEL_FREQUENCY_LIMIT = 0x100,

	// 0x200
	FB_FLAG_LIMIT_4K_SOURCE_SIZE = 0x200,

	// 0x400
	FB_FLAG_ALTERNATE_PWM_INCREMENT1 = 0x400,

	// 0x800
	FB_FLAG_ALTERNATE_PWM_INCREMENT2 = 0x800,

	// 0x1000
	FB_FLAG_DISABLE_FEATURE_IPS = 0x1000,

	// 0x2000 (Formerly UNKNOWN_2000)
	// Enables dithering to fix color banding gradients
	FB_FLAG_ENABLE_DITHERING = 0x2000,

	// 0x4000
	FB_FLAG_ALLOW_CONNECTOR_RECOVER = 0x4000,

	// 0x8000 (Formerly UNKNOWN_8000)
	// Critical flag: Disables pipe scrambling to enable 4K/5K output on older Intel gens
	FB_FLAG_DISABLE_PIPE_SCRAMBLE = 0x8000,

	// 0x10000 (Formerly UNKNOWN_10000)
	FB_FLAG_disable3d = 0x10000,

	// 0x20000 (Formerly UNKNOWN_20000)
	// Enables Audio packet transmission over HDMI/DP
	FB_FLAG_ENABLE_HDMI_AUDIO = 0x20000,

	// 0x40000
	FB_FLAG_DISABLE_GFMP_PFM = 0x40000,

	// 0x80000 (Formerly UNKNOWN_80000)
	// Enables Panel Self Refresh (power saving)
	FB_FLAG_ENABLE_PSR = 0x80000,

	// 0x100000 (Formerly UNKNOWN_100000)
	// Enables Panel Self Refresh 2 (deeper power saving)
	FB_FLAG_ENABLE_PSR2 = 0x100000,

	// 0x200000
	FB_FLAG_ENABLE_DYNAMIC_CDCLK = 0x200000,

	// 0x400000 (Formerly UNKNOWN_400000)
	// Allows 4K resolution at 60Hz on limited configs
	FB_FLAG_SUPPORT_4K_60HZ = 0x400000,

	// 0x800000
	FB_FLAG_SUPPORT_5K_SOURCE_SIZE = 0x800000
};

enum  FeatureControlOption {
	// 4KVideoDownscale is invalid in C++, renamed to VideoDownscale4K
	VideoDownscale4K,        // "4KVideoDownscale"
	AGPMLogControl,          // "AGPMLogControl"
	ASFU,                    // "ASFU"
	ASRDisable,              // "ASRDisable"
	CachedEDIDDisable,       // "CachedEDIDDisable"
	Dither,                  // "Dither"
	DitherDisable,           // "DitherDisable"
	DPM,                     // "DPM"
	DPDownSpreadDisable,     // "DPDownSpreadDisable"
	DSCCapReporting,         // "DSCCapReporting"
	DSCSupport,              // "DSCSupport"
	DBUFOptimizeTime,        // "DBUFOptimizeTime"
	DisableApertureAccess,   // "DisableApertureAccess"
	DisableDFB,              // "DisableDFB"
	DisableHigherWMLevels,   // "DisableHigherWMLevels"
	DisableInternalPanel,    // "DisableInternalPanel"
	DynamicFBCEnable,        // "DynamicFBCEnable"
	DynamicFBCFrameCount,    // "DynamicFBCFrameCount"
	DynamicRingTables,       // "DynamicRingTables"
	DynamicSliceSwitch,      // "DynamicSliceSwitch"
	EDPDownSpreadDisable,    // "EDPDownSpreadDisable"
	EUSDEnable,              // "EUSDEnable"
	Enable8K,                // "Enable8K"
	EnableDFB,               // "EnableDFB"
	EnableDeepDisplay,       // "EnableDeepDisplay"
	EnableLPVPonExternal,    // "EnableLPVPonExternal"
	EnableMultipipeScaler,   // "EnableMultipipeScaler"
	EnablePlaneRotation,     // "EnablePlaneRotation"
	EnableTconScaler,        // "EnableTconScaler"
	FBCEnableOnBypass,       // "FBCEnableOnBypass"
	FECSupport,              // "FECSupport"
	ForceAccelBacked,        // "ForceAccelBacked"
	GFMPPFM,                 // "GFMPPFM"
	GPU420Support,           // "GPU420Support"
	GPUInterruptHandling,    // "GPUInterruptHandling"
	Gamma,                   // "Gamma"
	IgnoreHMD,               // "IgnoreHMD"
	IPCEnable,               // "IPCEnable"
	LoadRingOverridePerSlice,// "LoadRingOverridePerSlice"
	LogSyncAsyncFlipChanges, // "LogSyncAsyncFlipChanges"
	NoAsyncWithVRR,          // "NoAsyncWithVRR"
	NoDoubleBufferWithVRR,   // "NoDoubleBufferWithVRR"
	NoResidency,             // "NoResidency"
	NV12Enable,              // "NV12Enable"
	NumTransactionsThreshold,// "NumTransactionsThreshold"
	P010Enable,              // "P010Enable"
	PStateOccupancyReportUsed, // "PStateOccupancyReportUsed"
	PStateResidencyWAEnable, // "PStateResidencyWAEnable"
	PowerStates,             // "PowerStates"
	RCxEIModeEnable,         // "RCxEIModeEnable"
	RenderStandby,           // "RenderStandby"
	SetRC6Voltage,           // "SetRC6Voltage"
	SliceSDEnable,           // "SliceSDEnable"
	SliceSDEnableOld,        // "SliceSDEnableOld"
	SubSliceSDEnable,        // "SubSliceSDEnable"
	SupportDynamicCDClk,     // "SupportDynamicCDClk"
	TTNFBuffDistribution,    // "TTNFBuffDistribution"
	ThreadingMode,           // "ThreadingMode"
	TracePointEnable,        // "TracePointEnable"
	TransactionCountTime,    // "TransactionCountTime"
	TransitionAmount,        // "TransitionAmount"
	TransCapEnable,          // "transcapenable"
	UseGucSliceSwitch,       // "UseGucSliceSwitch"
	VRRSupport,              // "VRRSupport"
	VideoBusyTimer,          // "VideoBusyTimer"
	VideoTurboFreq,          // "VideoTurboFreq"
	YTiling                  // "YTiling"
};

#define   IECPUNIT_CLKGATE_DIS			REG_BIT(22)
#define VDBOX_CGCTL3F10(base)			((base) + 0x3f10)
#define GEN11_GT_VEBOX_VDBOX_DISABLE		(0x9140)
#define GEN11_EU_DISABLE			(0x9134)
#define GEN11_GT_SLICE_ENABLE			(0x9138)
#define GEN11_GT_SUBSLICE_DISABLE		(0x913c)
#define RPM_CONFIG0				(0xd00)
#define   GEN11_GT_VDBOX_DISABLE_MASK		0xff
#define   GEN11_GT_VEBOX_DISABLE_SHIFT		0x10
#define   GEN11_GT_VEBOX_DISABLE_MASK		(0x0f << GEN11_GT_VEBOX_DISABLE_SHIFT)

#define   I915_ERROR_INSTRUCTION			(1 << 0)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + 1)
#define RING_START(base)			((base) + 0x38)
#define RING_CTL(base)				((base) + 0x3c)
#define RING_EIR(base)				((base) + 0xb0)
#define RING_EMR(base)				((base) + 0xb4)
#define RING_TAIL(base)				((base) + 0x30)
#define RING_HEAD(base)				((base) + 0x34)
#define RING_HWS_PGA(base)			((base) + 0x80)
#define RING_HWSTAM(base)			((base) + 0x98)
#define RING_MI_MODE(base)			((base) + 0x9c)
#define RING_MODE_GEN7(base)		((base) + 0x29c)
#define   GEN11_GFX_DISABLE_LEGACY_MODE		(1 << 3)
#define   STOP_RING				REG_BIT(8)
#define RENDER_RING_BASE	0x02000
#define GEN12_COMPUTE0_RING_BASE	0x1a000
#define BLT_RING_BASE		0x22000
#define GEN11_BSD_RING_BASE	0x1c0000
#define GEN11_BSD3_RING_BASE	0x1d0000
#define GEN11_VEBOX_RING_BASE	0x1c8000

struct drm_i915_mocs_entry {
	uint32_t control_value;
	uint16_t l3cc_value;
	uint16_t used;
};

struct drm_i915_mocs_table {
	unsigned int size;
	unsigned int n_entries;
	const struct drm_i915_mocs_entry *table;
	uint8_t uc_index;
	uint8_t wb_index; /* Only used on HAS_L3_CCS_READ() platforms */
	uint8_t unused_entries_index;
};

enum i915_mocs_table_index {
	/*
	 * Not cached anywhere, coherency between CPU and GPU accesses is
	 * guaranteed.
	 */
	I915_MOCS_UNCACHED,
	/*
	 * Cacheability and coherency controlled by the kernel automatically
	 * based on the DRM_I915_GEM_SET_CACHING IOCTL setting and the current
	 * usage of the surface (used for display scanout or not).
	 */
	I915_MOCS_PTE,
	/*
	 * Cached in all GPU caches available on the platform.
	 * Coherency between CPU and GPU accesses to the surface is not
	 * guaranteed without extra synchronization.
	 */
	I915_MOCS_CACHED,
};

/* Defines for the tables (XXX_MOCS_0 - XXX_MOCS_63) */
#define _LE_CACHEABILITY(value)	((value) << 0)
#define _LE_TGT_CACHE(value)	((value) << 2)
#define LE_LRUM(value)		((value) << 4)
#define LE_AOM(value)		((value) << 6)
#define LE_RSC(value)		((value) << 7)
#define LE_SCC(value)		((value) << 8)
#define LE_PFM(value)		((value) << 11)
#define LE_SCF(value)		((value) << 14)
#define LE_COS(value)		((value) << 15)
#define LE_SSE(value)		((value) << 17)

/* Defines for the tables (GLOB_MOCS_0 - GLOB_MOCS_16) */
#define _L4_CACHEABILITY(value)	((value) << 2)
#define IG_PAT(value)		((value) << 8)

/* Defines for the tables (LNCFMOCS0 - LNCFMOCS31) - two entries per word */
#define L3_ESC(value)		((value) << 0)
#define L3_SCC(value)		((value) << 1)
#define _L3_CACHEABILITY(value)	((value) << 4)
#define L3_GLBGO(value)		((value) << 6)
#define L3_LKUP(value)		((value) << 7)

/* Helper defines */
#define GEN9_NUM_MOCS_ENTRIES	64  /* 63-64 are reserved, but configured. */
#define MTL_NUM_MOCS_ENTRIES	16

/* (e)LLC caching options */
/*
 * Note: LE_0_PAGETABLE works only up to Gen11; for newer gens it means
 * the same as LE_UC
 */
#define LE_0_PAGETABLE		_LE_CACHEABILITY(0)
#define LE_1_UC			_LE_CACHEABILITY(1)
#define LE_2_WT			_LE_CACHEABILITY(2)
#define LE_3_WB			_LE_CACHEABILITY(3)

/* Target cache */
#define LE_TC_0_PAGETABLE	_LE_TGT_CACHE(0)
#define LE_TC_1_LLC		_LE_TGT_CACHE(1)
#define LE_TC_2_LLC_ELLC	_LE_TGT_CACHE(2)
#define LE_TC_3_LLC_ELLC_ALT	_LE_TGT_CACHE(3)

/* L3 caching options */
#define L3_0_DIRECT		_L3_CACHEABILITY(0)
#define L3_1_UC			_L3_CACHEABILITY(1)
#define L3_2_RESERVED		_L3_CACHEABILITY(2)
#define L3_3_WB			_L3_CACHEABILITY(3)

/* L4 caching options */
#define L4_0_WB			_L4_CACHEABILITY(0)
#define L4_1_WT			_L4_CACHEABILITY(1)
#define L4_2_RESERVED		_L4_CACHEABILITY(2)
#define L4_3_UC			_L4_CACHEABILITY(3)

#define MOCS_ENTRY(__idx, __control_value, __l3cc_value) \
	[__idx] = { \
		.control_value = __control_value, \
		.l3cc_value = __l3cc_value, \
		.used = 1, \
	}


#define GEN11_MOCS_ENTRIES \
/* Entries 0 and 1 are defined per-platform */ \
/* Base - L3 + LLC */ \
MOCS_ENTRY(2, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(3), \
	   L3_3_WB), \
/* Base - Uncached */ \
MOCS_ENTRY(3, \
	   LE_1_UC | LE_TC_1_LLC, \
	   L3_1_UC), \
/* Base - L3 */ \
MOCS_ENTRY(4, \
	   LE_1_UC | LE_TC_1_LLC, \
	   L3_3_WB), \
/* Base - LLC */ \
MOCS_ENTRY(5, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(3), \
	   L3_1_UC), \
/* Age 0 - LLC */ \
MOCS_ENTRY(6, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(1), \
	   L3_1_UC), \
/* Age 0 - L3 + LLC */ \
MOCS_ENTRY(7, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(1), \
	   L3_3_WB), \
/* Age: Don't Chg. - LLC */ \
MOCS_ENTRY(8, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(2), \
	   L3_1_UC), \
/* Age: Don't Chg. - L3 + LLC */ \
MOCS_ENTRY(9, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(2), \
	   L3_3_WB), \
/* No AOM - LLC */ \
MOCS_ENTRY(10, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(3) | LE_AOM(1), \
	   L3_1_UC), \
/* No AOM - L3 + LLC */ \
MOCS_ENTRY(11, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(3) | LE_AOM(1), \
	   L3_3_WB), \
/* No AOM; Age 0 - LLC */ \
MOCS_ENTRY(12, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(1) | LE_AOM(1), \
	   L3_1_UC), \
/* No AOM; Age 0 - L3 + LLC */ \
MOCS_ENTRY(13, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(1) | LE_AOM(1), \
	   L3_3_WB), \
/* No AOM; Age:DC - LLC */ \
MOCS_ENTRY(14, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(2) | LE_AOM(1), \
	   L3_1_UC), \
/* No AOM; Age:DC - L3 + LLC */ \
MOCS_ENTRY(15, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(2) | LE_AOM(1), \
	   L3_3_WB), \
/* Bypass LLC - Uncached (EHL+) */ \
MOCS_ENTRY(16, \
	   LE_1_UC | LE_TC_1_LLC | LE_SCF(1), \
	   L3_1_UC), \
/* Bypass LLC - L3 (Read-Only) (EHL+) */ \
MOCS_ENTRY(17, \
	   LE_1_UC | LE_TC_1_LLC | LE_SCF(1), \
	   L3_3_WB), \
/* Self-Snoop - L3 + LLC */ \
MOCS_ENTRY(18, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(3) | LE_SSE(3), \
	   L3_3_WB), \
/* Skip Caching - L3 + LLC(12.5%) */ \
MOCS_ENTRY(19, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(3) | LE_SCC(7), \
	   L3_3_WB), \
/* Skip Caching - L3 + LLC(25%) */ \
MOCS_ENTRY(20, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(3) | LE_SCC(3), \
	   L3_3_WB), \
/* Skip Caching - L3 + LLC(50%) */ \
MOCS_ENTRY(21, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(3) | LE_SCC(1), \
	   L3_3_WB), \
/* Skip Caching - L3 + LLC(75%) */ \
MOCS_ENTRY(22, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(3) | LE_RSC(1) | LE_SCC(3), \
	   L3_3_WB), \
/* Skip Caching - L3 + LLC(87.5%) */ \
MOCS_ENTRY(23, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(3) | LE_RSC(1) | LE_SCC(7), \
	   L3_3_WB), \
/* HW Reserved - SW program but never use */ \
MOCS_ENTRY(62, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(3), \
	   L3_1_UC), \
/* HW Reserved - SW program but never use */ \
MOCS_ENTRY(63, \
	   LE_3_WB | LE_TC_1_LLC | LE_LRUM(3), \
	   L3_1_UC)

static const struct drm_i915_mocs_entry tgl_mocs_table[] = {

	MOCS_ENTRY(I915_MOCS_PTE,
		   LE_0_PAGETABLE | LE_TC_0_PAGETABLE,
		   L3_1_UC),
	GEN11_MOCS_ENTRIES,

	/* Implicitly enable L1 - HDC:L1 + L3 + LLC */
	MOCS_ENTRY(48,
		   LE_3_WB | LE_TC_1_LLC | LE_LRUM(3),
		   L3_3_WB),
	/* Implicitly enable L1 - HDC:L1 + L3 */
	MOCS_ENTRY(49,
		   LE_1_UC | LE_TC_1_LLC,
		   L3_3_WB),
	/* Implicitly enable L1 - HDC:L1 + LLC */
	MOCS_ENTRY(50,
		   LE_3_WB | LE_TC_1_LLC | LE_LRUM(3),
		   L3_1_UC),
	/* Implicitly enable L1 - HDC:L1 */
	MOCS_ENTRY(51,
		   LE_1_UC | LE_TC_1_LLC,
		   L3_1_UC),
	/* HW Special Case (CCS) */
	MOCS_ENTRY(60,
		   LE_3_WB | LE_TC_1_LLC | LE_LRUM(3),
		   L3_1_UC),
	/* HW Special Case (Displayable) */
	MOCS_ENTRY(61,
		   LE_1_UC | LE_TC_1_LLC,
		   L3_3_WB),
};

static uint32_t get_entry_control(const struct drm_i915_mocs_table *table,
				 unsigned int index)
{
	if (index < table->size && table->table[index].used)
		return table->table[index].control_value;
	return table->table[table->unused_entries_index].control_value;
}

static uint16_t get_entry_l3cc(const struct drm_i915_mocs_table *table,
			  unsigned int index)
{
	if (index < table->size && table->table[index].used)
		return table->table[index].l3cc_value;
	return table->table[table->unused_entries_index].l3cc_value;
}

static uint32_t l3cc_combine(uint16_t low, uint16_t high)
{
	return low | (uint32_t)high << 16;
}

#define for_each_l3cc(l3cc, t, i) \
	for (i = 0; \
		 i < ((t)->n_entries + 1) / 2 ? \
		 (l3cc = l3cc_combine(get_entry_l3cc((t), 2 * i), \
				  get_entry_l3cc((t), 2 * i + 1))), 1 : \
		 0; \
		 i++)

#define for_each_mocs(mocs, t, i) \
	for (i = 0; \
		 i < (t)->n_entries ? (mocs = get_entry_control((t), i)), 1 : 0;\
		 i++)

#define GEN12_GLOBAL_MOCS(i)			(0x4000 + (i) * 4)
#define GEN9_LNCFCMOCS(i)			(0xb020 + (i) * 4)






enum IGHwCsType
{
	kIGHwCsTypeRCS,     //  0
	kIGHwCsTypeCCS,     //  1
	kIGHwCsTypeBCS,     //  2
	kIGHwCsTypeVCS0,    //  3
	kIGHwCsTypeVCS2,    //  4
	kIGHwCsTypeVECS0,   //  5
};
struct IGHwCsDesc {
	IGHwCsType   type;
	uint32_t     csMask;
	const char * name;
	const char * label;
	uint32_t     mmioExecListSubmitPort;
	uint32_t     mmioExecListSubmitQueue;
	uint32_t     mmioExecListControl;
	uint32_t     mmioExecListStatus;
	uint32_t     mmioContextStatusPointer;
	uint32_t     mmioContextStatusBuffer;
	uint32_t     mmioContextStatusPort;
	uint32_t     mmioContextStatusFifoStatus;
	uint32_t     mmioGlobalStatusPage;
	uint32_t     mmioGfxMode;
	uint32_t     mmioResetCtrl;
	uint32_t     mmioErrorIdentity;
	uint32_t     mmioErrorMask;
	uint32_t     mmioTimeStamp;
	uint32_t     mmioGpr0;
	uint32_t     fuseMask;
	uint32_t     contextSizeBytes;
	int32_t      stampIndexRangeMin;
	int32_t      stampIndexRangeMax;
	int32_t      stampIndexRangeSize;
	IOSelect     contextSwitchInterruptType;
	IOSelect     flushNotifyInterruptType;
	IOSelect     errorInterruptType;
	uint32_t     mmioForcewakeReq;
	uint32_t     mmioForcewakeAck;
};




enum dbuf_slice {
	DBUF_S1,
	DBUF_S2,
	DBUF_S3,
	DBUF_S4,
	I915_MAX_DBUF_SLICES
};


//workar
#define _PSR_IMR_A				0x60814
#define _PSR_IIR_A				0x60818
#define DG1_MSTR_TILE_INTR		(0x190008)
#define   DG1_MSTR_IRQ			REG_BIT(31)
#define   DG1_MSTR_TILE(t)		REG_BIT(t)
#define  DBUF_POWER_REQUEST			REG_BIT(31)
#define SOUTH_DSPCLK_GATE_D	(0xc2020)
#define GEN11_CHICKEN_DCPR_2	(0x46434)
#define  PCH_DPMGUNIT_CLOCK_GATE_DISABLE (1 << 15)
#define   DCPR_MASK_MAXLATENCY_MEMUP_CLR	(1 << 27)
#define   DCPR_MASK_LPMODE			(1 << 26)
#define   DCPR_SEND_RESP_IMM			(1 << 25)
#define   DCPR_CLEAR_MEMSTAT_DIS		(1 << 24)
#define GEN8_CHICKEN_DCPR_1			(0x46430)
#define GEN9_CLKGATE_DIS_0		(0x46530)
#define   ICL_DELAY_PMRSP			REG_BIT(22)
#define   DARBF_GATING_DIS		(1 << 27)

#define DC_STATE_EN			_MMIO(0x45504)
#define  DC_STATE_DISABLE		0
#define  DC_STATE_EN_DC3CO		REG_BIT(30)
#define  DC_STATE_DC3CO_STATUS		REG_BIT(29)
#define  HOLD_PHY_CLKREQ_PG1_LATCH	REG_BIT(21)
#define  HOLD_PHY_PG1_LATCH		REG_BIT(20)
#define  DC_STATE_EN_UPTO_DC5		(1 << 0)
#define  DC_STATE_EN_DC9		(1 << 3)
#define  DC_STATE_EN_UPTO_DC6		(2 << 0)
#define  DC_STATE_EN_UPTO_DC5_DC6_MASK   0x3

#define CLKREQ_POLICY			(0x101038)
#define  CLKREQ_POLICY_MEM_UP_OVRD	(1 << 1)
#define GEN11_COMMON_SLICE_CHICKEN3		(0x7304)
#define   GEN12_DISABLE_CPS_AWARE_COLOR_PIPE	(1 << 9)
#define GEN8_CS_CHICKEN1			(0x2580)
#define   GEN9_PREEMPT_3D_OBJECT_LEVEL		(1 << 0)
#define   GEN9_PREEMPT_GPGPU_LEVEL(hi, lo)	(((hi) << 2) | ((lo) << 1))
#define   GEN9_PREEMPT_GPGPU_MID_THREAD_LEVEL	GEN9_PREEMPT_GPGPU_LEVEL(0, 0)
#define   GEN9_PREEMPT_GPGPU_THREAD_GROUP_LEVEL	GEN9_PREEMPT_GPGPU_LEVEL(0, 1)
#define   GEN9_PREEMPT_GPGPU_COMMAND_LEVEL	GEN9_PREEMPT_GPGPU_LEVEL(1, 0)
#define   GEN9_PREEMPT_GPGPU_LEVEL_MASK		GEN9_PREEMPT_GPGPU_LEVEL(1, 1)
#define   GEN8_MCR_SLICE(slice)			(((slice) & 3) << 26)
#define   GEN8_MCR_SLICE_MASK			GEN8_MCR_SLICE(3)
#define   GEN8_MCR_SUBSLICE(subslice)		(((subslice) & 3) << 24)
#define   GEN8_MCR_SUBSLICE_MASK		GEN8_MCR_SUBSLICE(3)
#define GEN8_MCR_SELECTOR			(0xfdc)
#define PS_INVOCATION_COUNT			(0x2348)
#define   RING_FORCE_TO_NONPRIV_ACCESS_RD	(1 << 28)
#define   RING_FORCE_TO_NONPRIV_ACCESS_WR	(2 << 28)
#define   RING_FORCE_TO_NONPRIV_ACCESS_INVALID	(3 << 28)
#define   RING_FORCE_TO_NONPRIV_ACCESS_MASK	(3 << 28)
#define   RING_FORCE_TO_NONPRIV_RANGE_1		(0 << 0)     /* CFL+ & Gen11+ */
#define   RING_FORCE_TO_NONPRIV_RANGE_4		(1 << 0)
#define GEN7_COMMON_SLICE_CHICKEN1		(0x7010)
#define HIZ_CHICKEN				(0x7018)
#define GEN7_FF_SLICE_CS_CHICKEN1		(0x20e0)
#define   GEN9_FFSC_PERCTX_PREEMPT_CTRL		(1 << 14)
#define _PICK_EVEN(__index, __a, __b) ((__a) + (__index) * ((__b) - (__a)))
#define _BW_BUDDY0_CTL			0x45130
#define _BW_BUDDY1_CTL			0x45140
#define BW_BUDDY_CTL(x)			(_PICK_EVEN(x, \
							 _BW_BUDDY0_CTL, \
							 _BW_BUDDY1_CTL))

#define BITS_PER_BYTE		8
#define BITS_PER_TYPE(type)	(sizeof(type) * BITS_PER_BYTE)

#define _BW_BUDDY0_PAGE_MASK		0x45134
#define _BW_BUDDY1_PAGE_MASK		0x45144
#define BW_BUDDY_PAGE_MASK(x)		(_PICK_EVEN(x, \
							 _BW_BUDDY0_PAGE_MASK, \
							 _BW_BUDDY1_PAGE_MASK))
#define   BW_BUDDY_DISABLE		( 1 << 31)
#define   BW_BUDDY_TLB_REQ_TIMER_MASK	REG_GENMASK(21, 16)
#define   BW_BUDDY_TLB_REQ_TIMER(x)	REG_FIELD_PREP(BW_BUDDY_TLB_REQ_TIMER_MASK, x)
#define GEN12_FF_MODE2				(0x6604)
#define XEHP_FF_MODE2				(0x6604)
#define   FF_MODE2_GS_TIMER_MASK		REG_GENMASK(31, 24)
#define   FF_MODE2_GS_TIMER_224			REG_FIELD_PREP(FF_MODE2_GS_TIMER_MASK, 224)
#define   FF_MODE2_TDS_TIMER_MASK		REG_GENMASK(23, 16)
#define   FF_MODE2_TDS_TIMER_128		REG_FIELD_PREP(FF_MODE2_TDS_TIMER_MASK, 4)
#define GEN7_MISCCPCTL				(0x9424)
#define   GEN12_DOP_CLOCK_GATE_RENDER_ENABLE	(1 << 1)
#define GEN9_CS_DEBUG_MODE1			(0x20ec)
#define   FF_DOP_CLOCK_GATE_DISABLE		(1 << 1)
#define GEN8_ROW_CHICKEN2			(0xe4f4)
#define   GEN12_DISABLE_READ_SUPPRESSION	(1 << 15)
#define   GEN12_DISABLE_EARLY_READ		(1 << 14)
#define GEN7_FF_THREAD_MODE		(0x20a0)
#define   GEN7_FF_SCHED_MASK		0x0077070
#define   GEN8_FF_DS_REF_CNT_FFME	(1 << 19)
#define   GEN12_FF_TESSELATION_DOP_GATE_DISABLE BIT(19)
#define GEN10_SAMPLER_MODE			(0xe18c)
#define   ENABLE_SMALLPL			(1 << 15)
#define   GEN12_PUSH_CONST_DEREF_HOLD_DIS	(1 << 8)
#define GEN9_ROW_CHICKEN4			(0xe48c)
#define   GEN12_DISABLE_TDL_PUSH		(1 << 9)
#define RING_PSMI_CTL(base)			((base) + 0x50)
#define RENDER_RING_BASE	0x02000
#define   GEN12_WAIT_FOR_EVENT_POWER_DOWN_DISABLE (1 << 7)
#define   GEN8_RC_SEMA_IDLE_MSG_DISABLE		(1 << 12)
#define _PICK_EVEN_2RANGES(__index, __c_index, __a, __b, __c, __d)		\
				   _PICK_EVEN((__index) - (__c_index), __c, __d)
#define _MBUS_ABOX0_CTL			0x45038
#define _MBUS_ABOX1_CTL			0x45048
#define _MBUS_ABOX2_CTL			0x4504C
#define MBUS_ABOX_CTL(x)							\
	(_PICK_EVEN_2RANGES(x, 2,						\
				 _MBUS_ABOX0_CTL, _MBUS_ABOX1_CTL,		\
				 _MBUS_ABOX2_CTL, _MBUS_ABOX2_CTL))
#define MBUS_ABOX_BW_CREDIT_MASK	(3 << 20)
#define MBUS_ABOX_BW_CREDIT(x)		((x) << 20)
#define MBUS_ABOX_B_CREDIT_MASK		(0xF << 16)
#define MBUS_ABOX_B_CREDIT(x)		((x) << 16)
#define MBUS_ABOX_BT_CREDIT_POOL2_MASK	(0x1F << 8)
#define MBUS_ABOX_BT_CREDIT_POOL2(x)	((x) << 8)
#define MBUS_ABOX_BT_CREDIT_POOL1_MASK	(0x1F << 0)
#define MBUS_ABOX_BT_CREDIT_POOL1(x)	((x) << 0)
#define BLT_RING_BASE		0x22000
#define RENDER_RING_BASE	0x02000
#define GEN12_COMPUTE0_RING_BASE	0x1a000
#define RING_CMD_CCTL(base)			((base) + 0xc4)
#define CMD_CCTL_WRITE_OVERRIDE_MASK REG_GENMASK(13, 7)
#define CMD_CCTL_READ_OVERRIDE_MASK REG_GENMASK(6, 0)
#define CMD_CCTL_MOCS_MASK (CMD_CCTL_WRITE_OVERRIDE_MASK | \
				CMD_CCTL_READ_OVERRIDE_MASK)
#define CMD_CCTL_MOCS_OVERRIDE(write, read)				      \
		(REG_FIELD_PREP(CMD_CCTL_WRITE_OVERRIDE_MASK, (write) << 1) | \
		 REG_FIELD_PREP(CMD_CCTL_READ_OVERRIDE_MASK, (read) << 1))
#define BLIT_CCTL(base)				((base) + 0x204)
#define   BLIT_CCTL_DST_MOCS_MASK		REG_GENMASK(14, 8)
#define   BLIT_CCTL_SRC_MOCS_MASK		REG_GENMASK(6, 0)
#define   BLIT_CCTL_MASK (BLIT_CCTL_DST_MOCS_MASK | \
			  BLIT_CCTL_SRC_MOCS_MASK)
#define   BLIT_CCTL_MOCS(dst, src)				       \
		(REG_FIELD_PREP(BLIT_CCTL_DST_MOCS_MASK, (dst) << 1) | \
		 REG_FIELD_PREP(BLIT_CCTL_SRC_MOCS_MASK, (src) << 1))
#define _DBUF_CTL_S0					0x45008
#define _DBUF_CTL_S1					0x44FE8
#define _DBUF_CTL_S2					0x44300
#define _DBUF_CTL_S3					0x44304

#define  DC_STATE_DEBUG                  _MMIO(0x45520)
#define  DC_STATE_DEBUG_MASK_CORES	(1 << 0)
#define  DC_STATE_DEBUG_MASK_MEMORY_UP	(1 << 1)
#define _MMIO(r) (r)
#define DBUF_CTL_S(slice)				_MMIO(_PICK(slice, \
									_DBUF_CTL_S0, \
									_DBUF_CTL_S1, \
									_DBUF_CTL_S2, \
									_DBUF_CTL_S3))


#define  DBUF_TRACKER_STATE_SERVICE_MASK	REG_GENMASK(23, 19)
#define  DBUF_TRACKER_STATE_SERVICE(x)		REG_FIELD_PREP(DBUF_TRACKER_STATE_SERVICE_MASK, x)
//end workar
#define  DBUF_POWER_STATE				REG_BIT(30)

#define GENMASK(high, low) \
	(((0xFFFFFFFF) << (low)) & (0xFFFFFFFF >> (32 - 1 - (high))))

#define REG_GENMASK(__high, __low)	GENMASK(__high, __low)

static inline unsigned long find_first_bit(const unsigned long *addr, unsigned long size)
{
	unsigned long idx;

	for (idx = 0; idx * 64 < size; idx++) {
		if (addr[idx])
			return min(idx * 64 + __builtin_ffs(addr[idx]), size);
	}

	return size;
}

static inline  unsigned long find_next_bit(const unsigned long *addr, unsigned long size,
				unsigned long offset)
{
		unsigned long val;

		if ((offset >= size))
			return size;

		val = *addr & GENMASK(size - 1, offset);
		return val ? __builtin_ffs(val) : size;
}


#define for_each_set_bit(bit, addr, size) \
	for ((bit) = find_first_bit((addr), (size));		\
		 (bit) < (size);					\
		 (bit) = find_next_bit((addr), (size), (bit) + 1))

#define __bf_shf(x) (__builtin_ffsll(x) - 1)
#define REG_FIELD_PREP(__mask, __val) \
((uint32_t)((__typeof(__mask))(__val) << __bf_shf(__mask)) & (__mask))

static inline unsigned int __ffs(u32 word) {
	unsigned int num = 0;
	if ((word & 0xffff) == 0) { num += 16; word >>= 16; }
	if ((word & 0xff) == 0) { num += 8; word >>= 8; }
	if ((word & 0xf) == 0) { num += 4; word >>= 4; }
	if ((word & 0x3) == 0) { num += 2; word >>= 2; }
	if ((word & 0x1) == 0) { num += 1; }
	return num;
}

#define REG_FIELD_GET(mask, val) (((val) & (mask)) >> (__ffs(mask)))

struct buddy_page_mask {
	uint32_t page_mask;
	uint8_t type;
	uint8_t num_channels;
};
enum intel_dram_type {
	INTEL_DRAM_UNKNOWN,
	INTEL_DRAM_DDR3,
	INTEL_DRAM_DDR4,
	INTEL_DRAM_LPDDR3,
	INTEL_DRAM_LPDDR4,
	INTEL_DRAM_DDR5,
	INTEL_DRAM_LPDDR5,
	INTEL_DRAM_GDDR,
} ;
static const struct buddy_page_mask tgl_buddy_page_masks[] = {
	{ .num_channels = 1, .type = INTEL_DRAM_DDR4,   .page_mask = 0xF },
	{ .num_channels = 1, .type = INTEL_DRAM_DDR5,	.page_mask = 0xF },
	{ .num_channels = 2, .type = INTEL_DRAM_LPDDR4, .page_mask = 0x1C },
	{ .num_channels = 2, .type = INTEL_DRAM_LPDDR5, .page_mask = 0x1C },
	{ .num_channels = 2, .type = INTEL_DRAM_DDR4,   .page_mask = 0x1F },
	{ .num_channels = 2, .type = INTEL_DRAM_DDR5,   .page_mask = 0x1E },
	{ .num_channels = 4, .type = INTEL_DRAM_LPDDR4, .page_mask = 0x38 },
	{ .num_channels = 4, .type = INTEL_DRAM_LPDDR5, .page_mask = 0x38 },
	{}
};

static constexpr uint32_t ICL_REG_CDCLK_CTL = 0x46000;

static constexpr uint32_t ICL_REG_DSSM = 0x51004;


enum ICLReferenceClockFrequency {
	
	// 24 MHz
	ICL_REF_CLOCK_FREQ_24_0 = 0x0,
	
	// 19.2 MHz
	ICL_REF_CLOCK_FREQ_19_2 = 0x1,
	
	// 38.4 MHz
	ICL_REF_CLOCK_FREQ_38_4 = 0x2
};


enum ICLCoreDisplayClockDecimalFrequency {
	
	// 172.8 MHz
	ICL_CDCLK_FREQ_172_8 = 0x158,
	
	// 180 MHz
	ICL_CDCLK_FREQ_180_0 = 0x166,
	
	// 192 MHz
	ICL_CDCLK_FREQ_192_0 = 0x17E,
	
	// 307.2 MHz
	ICL_CDCLK_FREQ_307_2 = 0x264,
	
	// 312 MHz
	ICL_CDCLK_FREQ_312_0 = 0x26E,
	
	// 552 MHz
	ICL_CDCLK_FREQ_552_0 = 0x44E,
	
	// 556.8 MHz
	ICL_CDCLK_FREQ_556_8 = 0x458,
	
	// 648 MHz
	ICL_CDCLK_FREQ_648_0 = 0x50E,
	
	// 652.8 MHz
	ICL_CDCLK_FREQ_652_8 = 0x518
};

static constexpr uint32_t ICL_CDCLK_DEC_FREQ_THRESHOLD = ICL_CDCLK_FREQ_648_0;


static constexpr uint32_t ICL_CDCLK_PLL_FREQ_REF_24_0 = 24000000 * 54;

static constexpr uint32_t ICL_CDCLK_PLL_FREQ_REF_19_2 = 19200000 * 68;

static constexpr uint32_t ICL_CDCLK_PLL_FREQ_REF_38_4 = 38400000 * 34;

//#define  BXT_CDCLK_CD2X_DIV_SEL_MASK	REG_GENMASK(23, 22)
#define  BXT_CDCLK_CD2X_DIV_SEL_MASK	(3 << 22)
#define  BXT_CDCLK_CD2X_DIV_SEL_1	(0 << 22)
#define  BXT_CDCLK_CD2X_DIV_SEL_1_5	(1 << 22)
#define  BXT_CDCLK_CD2X_DIV_SEL_2	(2 << 22)
#define  BXT_CDCLK_CD2X_DIV_SEL_4	(3 << 22)
#define  BXT_CDCLK_CD2X_PIPE(pipe)	((pipe) << 20)
#define  CDCLK_DIVMUX_CD_OVERRIDE	(1 << 19)
#define  BXT_CDCLK_CD2X_PIPE_NONE	BXT_CDCLK_CD2X_PIPE(3)
#define  ICL_CDCLK_CD2X_PIPE_NONE	(7 << 19)
#define  BXT_CDCLK_SSA_PRECHARGE_ENABLE	(1 << 16)
#define  CDCLK_FREQ_DECIMAL_MASK	(0x7ff)
#define SKL_DSSM				(0x51004)
#define ICL_DSSM_CDCLK_PLL_REFCLK_MASK		(7 << 29)
#define ICL_DSSM_CDCLK_PLL_REFCLK_24MHz		(0 << 29)
#define ICL_DSSM_CDCLK_PLL_REFCLK_19_2MHz	(1 << 29)
#define ICL_DSSM_CDCLK_PLL_REFCLK_38_4MHz	(2 << 29)
#define BXT_DE_PLL_ENABLE		(0x46070)
#define   BXT_DE_PLL_PLL_ENABLE		(1 << 31)
#define   BXT_DE_PLL_LOCK		(1 << 30)
#define   BXT_DE_PLL_FREQ_REQ		(1 << 23)
#define   BXT_DE_PLL_FREQ_REQ_ACK	(1 << 22)
#define   ICL_CDCLK_PLL_RATIO(x)	(x)
#define   ICL_CDCLK_PLL_RATIO_MASK	0xff
#define CDCLK_CTL			(0x46000)

#define DIV_ROUND_CLOSEST(x, divisor)(			\
{							\
__typeof(x) __x = x;				\
__typeof(divisor) __d = divisor;			\
	(((__typeof(x))-1) > 0 ||				\
	 ((__typeof(divisor))-1) > 0 || (__x) > 0) ?	\
		(((__x) + ((__d) / 2)) / (__d)) :	\
		(((__x) - ((__d) / 2)) / (__d));	\
}							\
)

#define GEN11_GFX_MSTR_IRQ		(0x190010)

#define SOUTH_CHICKEN1		(0xc2000)
#define GEN8_MASTER_IRQ		(0x44200)
#define GEN11_DISPLAY_INT_CTL	(0x44200)

#define PCH_PORT_HOTPLUG		(0xc4030)
#define SHOTPLUG_CTL_DDI		(0xc4030)
#define SHOTPLUG_CTL_TC			(0xc4034)
#define SHPD_FILTER_CNT			(0xc4038)

#define SDEIMR (0xc4004)
#define SDEIIR (0xc4008)
#define SDEIER (0xc400c)

#define GEN11_DE_HPD_ISR		(0x44470)
#define GEN11_DE_HPD_IMR		(0x44474)
#define GEN11_DE_HPD_IIR		(0x44478)
#define GEN11_DE_HPD_IER		(0x4447c)

#define GEN8_DE_MISC_ISR (0x44460)
#define GEN8_DE_MISC_IMR  (0x44464)
#define GEN8_DE_MISC_IIR  (0x44468)
#define GEN8_DE_MISC_IER  (0x4446c)

#define GEN8_DE_PIPE_ISR_A  (0x44400)
#define GEN8_DE_PIPE_IMR_A  (0x44404)
#define GEN8_DE_PIPE_IIR_A  (0x44408)
#define GEN8_DE_PIPE_IER_A  (0x4440c)

#define GEN8_DE_PIPE_ISR_B  (0x44410)
#define GEN8_DE_PIPE_IMR_B  (0x44414)
#define GEN8_DE_PIPE_IIR_B  (0x44418)
#define GEN8_DE_PIPE_IER_B  (0x4441c)

#define GEN8_DE_PIPE_ISR_C  (0x44420)
#define GEN8_DE_PIPE_IMR_C  (0x44424)
#define GEN8_DE_PIPE_IIR_C  (0x44428)
#define GEN8_DE_PIPE_IER_C  (0x4442c)



constexpr uint32_t MCHBAR_MIRROR_BASE_SNB = 0x140000;
constexpr uint32_t GEN6_RP_STATE_CAP = MCHBAR_MIRROR_BASE_SNB + 0x5998;

constexpr uint32_t GEN9_FREQUENCY_SHIFT = 23;
constexpr uint32_t GEN9_FREQ_SCALER  = 3;

constexpr uint32_t FORCEWAKE_KERNEL_FALLBACK = 1 << 15;

constexpr uint32_t FORCEWAKE_ACK_TIMEOUT_MS = 50;

constexpr uint32_t FORCEWAKE_MEDIA_GEN9 = 0xa270;
constexpr uint32_t FORCEWAKE_RENDER_GEN9 = 0xa278;
constexpr uint32_t FORCEWAKE_BLITTER_GEN9 = 0xa188;

constexpr uint32_t FORCEWAKE_ACK_MEDIA_GEN9 = 0x0D88;
constexpr uint32_t FORCEWAKE_ACK_RENDER_GEN9 = 0x0D84;
constexpr uint32_t FORCEWAKE_ACK_BLITTER_GEN9 = 0x130044;

#define FORCEWAKE_MEDIA_VDBOX_GEN11(n)		(0xa540 + (n) * 4)
#define FORCEWAKE_MEDIA_VEBOX_GEN11(n)		(0xa560 + (n) * 4)
#define FORCEWAKE_ACK_MEDIA_VDBOX_GEN11(n)	(0xd50 + (n) * 4)
#define FORCEWAKE_ACK_MEDIA_VEBOX_GEN11(n)	(0xd70 + (n) * 4)
#define FORCEWAKE_GT_GEN9			(0xa188)
#define FORCEWAKE_ACK_GT_GEN9			(0x130044)
#define FORCEWAKE_REQ_GSC			(0xa618)
#define FORCEWAKE_ACK_GSC			(0xdf8)



enum FORCEWAKE_DOM_BITS : unsigned {
	DOM_RENDER = 0b001,
	DOM_MEDIA = 0b010,
	DOM_BLITTER = 0b100,
	DOM_LAST = DOM_BLITTER,
	DOM_FIRST = DOM_RENDER
};
/*
enum forcewake_domain_id {
	FW_DOMAIN_ID_RENDER = 0,
	FW_DOMAIN_ID_BLITTER,
	FW_DOMAIN_ID_MEDIA,
	FW_DOMAIN_ID_MEDIA_VDBOX0,
	FW_DOMAIN_ID_MEDIA_VDBOX1,
	FW_DOMAIN_ID_MEDIA_VDBOX2,
	FW_DOMAIN_ID_MEDIA_VDBOX3,
	FW_DOMAIN_ID_MEDIA_VEBOX0,
	FW_DOMAIN_ID_MEDIA_VEBOX1,

	FW_DOMAIN_ID_COUNT
};*/

enum forcewake_domain_id {
	FW_DOMAIN_ID_RENDER = 0,
	FW_DOMAIN_ID_GT,        /* also includes blitter engine */
	FW_DOMAIN_ID_MEDIA,
	FW_DOMAIN_ID_MEDIA_VDBOX0,
	FW_DOMAIN_ID_MEDIA_VDBOX1,
	FW_DOMAIN_ID_MEDIA_VDBOX2,
	FW_DOMAIN_ID_MEDIA_VDBOX3,
	FW_DOMAIN_ID_MEDIA_VDBOX4,
	FW_DOMAIN_ID_MEDIA_VDBOX5,
	FW_DOMAIN_ID_MEDIA_VDBOX6,
	FW_DOMAIN_ID_MEDIA_VDBOX7,
	FW_DOMAIN_ID_MEDIA_VEBOX0,
	FW_DOMAIN_ID_MEDIA_VEBOX1,
	FW_DOMAIN_ID_MEDIA_VEBOX2,
	FW_DOMAIN_ID_MEDIA_VEBOX3,
	FW_DOMAIN_ID_GSC,

	FW_DOMAIN_ID_COUNT
};


enum forcewake_domains {
	FORCEWAKE_RENDER	= BIT(FW_DOMAIN_ID_RENDER),
	FORCEWAKE_GT		= BIT(FW_DOMAIN_ID_GT),
	FORCEWAKE_MEDIA		= BIT(FW_DOMAIN_ID_MEDIA),
	FORCEWAKE_MEDIA_VDBOX0	= BIT(FW_DOMAIN_ID_MEDIA_VDBOX0),
	FORCEWAKE_MEDIA_VDBOX1	= BIT(FW_DOMAIN_ID_MEDIA_VDBOX1),
	FORCEWAKE_MEDIA_VDBOX2	= BIT(FW_DOMAIN_ID_MEDIA_VDBOX2),
	FORCEWAKE_MEDIA_VDBOX3	= BIT(FW_DOMAIN_ID_MEDIA_VDBOX3),
	FORCEWAKE_MEDIA_VDBOX4	= BIT(FW_DOMAIN_ID_MEDIA_VDBOX4),
	FORCEWAKE_MEDIA_VDBOX5	= BIT(FW_DOMAIN_ID_MEDIA_VDBOX5),
	FORCEWAKE_MEDIA_VDBOX6	= BIT(FW_DOMAIN_ID_MEDIA_VDBOX6),
	FORCEWAKE_MEDIA_VDBOX7	= BIT(FW_DOMAIN_ID_MEDIA_VDBOX7),
	FORCEWAKE_MEDIA_VEBOX0	= BIT(FW_DOMAIN_ID_MEDIA_VEBOX0),
	FORCEWAKE_MEDIA_VEBOX1	= BIT(FW_DOMAIN_ID_MEDIA_VEBOX1),
	FORCEWAKE_MEDIA_VEBOX2	= BIT(FW_DOMAIN_ID_MEDIA_VEBOX2),
	FORCEWAKE_MEDIA_VEBOX3	= BIT(FW_DOMAIN_ID_MEDIA_VEBOX3),
	FORCEWAKE_GSC		= BIT(FW_DOMAIN_ID_GSC),

	FORCEWAKE_ALL = BIT(FW_DOMAIN_ID_COUNT) - 1,
};






enum intel_engine_id {
	RCS0 = 0,
	BCS0,
	BCS1,
	BCS2,
	BCS3,
	BCS4,
	BCS5,
	BCS6,
	BCS7,
	BCS8,
#define _BCS(n) (BCS0 + (n))
	VCS0,
	VCS1,
	VCS2,
	VCS3,
	VCS4,
	VCS5,
	VCS6,
	VCS7,
#define _VCS(n) (VCS0 + (n))
	VECS0,
	VECS1,
	VECS2,
	VECS3,
#define _VECS(n) (VECS0 + (n))
	CCS0,
	CCS1,
	CCS2,
	CCS3,
#define _CCS(n) (CCS0 + (n))
	GSC0,
	I915_NUM_ENGINES
#define INVALID_ENGINE ((enum intel_engine_id)-1)
};
#define __HAS_ENGINE(engine_mask, id) ((engine_mask) & BIT(id))

#define NEEDS_FORCE_WAKE(reg) ({ \
	u32 __reg = (reg); \
	__reg < 0x40000 || __reg >= 0x116000; \
})

#define GEN_FW_RANGE(s, e, d) \
	{ .start = (s), .end = (e), .domains = (d) }

#define GEN11_GACB_PERF_CTRL			(0x4b80)
#define   GEN11_HASH_CTRL_MASK			(0x3 << 12 | 0xf << 0)
#define   GEN11_HASH_CTRL_BIT0			(1 << 0)
#define   GEN11_HASH_CTRL_BIT4			(1 << 12)
#define GEN11_LSN_UNSLCVC			(0xb43c)
#define   GEN11_LSN_UNSLCVC_GAFS_HALF_CL2_MAXALLOC	(1 << 9)
#define   GEN11_LSN_UNSLCVC_GAFS_HALF_SF_MAXALLOC	(1 << 7)
#define GEN8_GAMW_ECO_DEV_RW_IA			(0x4080)
#define   GAMW_ECO_ENABLE_64K_IPS_FIELD		0xF
#define   GAMW_ECO_DEV_CTX_RELOAD_DISABLE	(1 << 7)
#define GAMT_CHKN_BIT_REG			(0x4ab8)
#define   GAMT_CHKN_DISABLE_L3_COH_PIPE		(1 << 31)
#define   GAMT_CHKN_DISABLE_DYNAMIC_CREDIT_SHARING	(1 << 28)
#define   GAMT_CHKN_DISABLE_I2M_CYCLE_ON_WR_PORT	(1 << 24)
#define UNSLICE_UNIT_LEVEL_CLKGATE2		(0x94e4)
#define   VSUNIT_CLKGATE_DIS_TGL		BIT(19)
#define   PSDUNIT_CLKGATE_DIS			BIT(5)
#define UNSLICE_UNIT_LEVEL_CLKGATE		(0x9434)
#define   VFUNIT_CLKGATE_DIS			BIT(20)
#define   CG3DDISCFEG_CLKGATE_DIS		BIT(17) /* DG2 */
#define   GAMEDIA_CLKGATE_DIS			BIT(11)
#define   HSUNIT_CLKGATE_DIS			BIT(8)
#define   VSUNIT_CLKGATE_DIS			BIT(3)
#define GEN10_DFR_RATIO_EN_AND_CHICKEN		(0x9550)
#define   DFR_DISABLE				(1 << 9)
#define GEN11_SUBSLICE_UNIT_LEVEL_CLKGATE	(0x9524)
#define   DSS_ROUTER_CLKGATE_DIS		BIT(28)
#define   GWUNIT_CLKGATE_DIS			BIT(16)

#define HSW_NDE_RSTWRN_OPT	(0x46408)
#define  MTL_RESET_PICA_HANDSHAKE_EN	BIT(6)
#define  RESET_PCH_HANDSHAKE_ENABLE	BIT(4)

#define GEN9_GAMT_ECO_REG_RW_IA (0x4ab0)
#define   GAMT_ECO_ENABLE_IN_PLACE_DECOMPRESS	(1 << 18)


struct intel_forcewake_range {
	uint32_t start;
	uint32_t end;

	enum forcewake_domains domains;
};

const struct intel_forcewake_range __gen11_fw_ranges[] = {
	//GEN_FW_RANGE(0x0, 0x1fff, 0), /* uncore range */
	GEN_FW_RANGE(0x2000, 0x26ff, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0x2700, 0x2fff, FORCEWAKE_GT),
	GEN_FW_RANGE(0x3000, 0x3fff, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0x4000, 0x51ff, FORCEWAKE_GT),
	GEN_FW_RANGE(0x5200, 0x7fff, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0x8000, 0x813f, FORCEWAKE_GT),
	GEN_FW_RANGE(0x8140, 0x815f, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0x8160, 0x82ff, FORCEWAKE_GT),
	GEN_FW_RANGE(0x8300, 0x84ff, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0x8500, 0x87ff, FORCEWAKE_GT),
	//GEN_FW_RANGE(0x8800, 0x8bff, 0),
	GEN_FW_RANGE(0x8c00, 0x8cff, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0x8d00, 0x94cf, FORCEWAKE_GT),
	GEN_FW_RANGE(0x94d0, 0x955f, FORCEWAKE_RENDER),
	//GEN_FW_RANGE(0x9560, 0x95ff, 0),
	GEN_FW_RANGE(0x9600, 0xafff, FORCEWAKE_GT),
	GEN_FW_RANGE(0xb000, 0xb47f, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0xb480, 0xdeff, FORCEWAKE_GT),
	GEN_FW_RANGE(0xdf00, 0xe8ff, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0xe900, 0x16dff, FORCEWAKE_GT),
	GEN_FW_RANGE(0x16e00, 0x19fff, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0x1a000, 0x23fff, FORCEWAKE_GT),
	//GEN_FW_RANGE(0x24000, 0x2407f, 0),
	GEN_FW_RANGE(0x24080, 0x2417f, FORCEWAKE_GT),
	GEN_FW_RANGE(0x24180, 0x242ff, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0x24300, 0x243ff, FORCEWAKE_GT),
	GEN_FW_RANGE(0x24400, 0x24fff, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0x25000, 0x3ffff, FORCEWAKE_GT),
	//GEN_FW_RANGE(0x40000, 0x1bffff, 0),
	GEN_FW_RANGE(0x1c0000, 0x1c3fff, FORCEWAKE_MEDIA_VDBOX0),
	//GEN_FW_RANGE(0x1c4000, 0x1c7fff, 0),
	GEN_FW_RANGE(0x1c8000, 0x1cffff, FORCEWAKE_MEDIA_VEBOX0),
	GEN_FW_RANGE(0x1d0000, 0x1d3fff, FORCEWAKE_MEDIA_VDBOX2),
	//GEN_FW_RANGE(0x1d4000, 0x1dbfff, 0)
};
#define  HSW_EDRAM_CAP				(0x120010)
#define    EDRAM_NUM_BANKS(cap)			(((cap) >> 1) & 0xf)
#define    EDRAM_WAYS_IDX(cap)			(((cap) >> 5) & 0x7)
#define    EDRAM_SETS_IDX(cap)			(((cap) >> 8) & 0x3)
#define   FORCEWAKE_KERNEL			BIT(0)


#define GEN11_GT_INTR_DW0		(0x190018)
#define  GEN11_CSME			(31)
#define  GEN11_GUNIT			(28)
#define  GEN11_GUC			(25)
#define  GEN11_WDPERF			(20)
#define  GEN11_KCR			(19)
#define  GEN11_GTPM			(16)
#define  GEN11_BCS			(15)
#define  GEN11_RCS0			(0)

#define GEN11_GT_INTR_DW1		(0x19001c)
#define  GEN11_VECS(x)			(31 - (x))
#define  GEN11_VCS(x)			(x)

#define GEN11_GT_INTR_DW(x)		(0x190018 + ((x) * 4))

#define GEN11_INTR_IDENTITY_REG0	(0x190060)
#define GEN11_INTR_IDENTITY_REG1	(0x190064)
#define  GEN11_INTR_DATA_VALID		(1 << 31)
//#define  GEN11_INTR_ENGINE_CLASS(x)	(((x) & GENMASK(18, 16)) >> 16)
//#define  GEN11_INTR_ENGINE_INSTANCE(x)	(((x) & GENMASK(25, 20)) >> 20)
#define  GEN11_INTR_ENGINE_INTR(x)	((x) & 0xffff)
/* irq instances for OTHER_CLASS */
#define OTHER_GUC_INSTANCE	0
#define OTHER_GTPM_INSTANCE	1

#define GEN11_INTR_IDENTITY_REG(x)	(0x190060 + ((x) * 4))

#define GEN11_IIR_REG0_SELECTOR		(0x190070)
#define GEN11_IIR_REG1_SELECTOR		(0x190074)

#define GEN11_IIR_REG_SELECTOR(x)	(0x190070 + ((x) * 4))

#define GEN11_RENDER_COPY_INTR_ENABLE	(0x190030)
#define GEN11_VCS_VECS_INTR_ENABLE	(0x190034)
#define GEN11_GUC_SG_INTR_ENABLE	(0x190038)
#define GEN11_GPM_WGBOXPERF_INTR_ENABLE	(0x19003c)
#define GEN11_CRYPTO_RSVD_INTR_ENABLE	(0x190040)
#define GEN11_GUNIT_CSME_INTR_ENABLE	(0x190044)

#define GEN11_RCS0_RSVD_INTR_MASK	(0x190090)
#define GEN11_BCS_RSVD_INTR_MASK	(0x1900a0)
#define GEN11_VCS0_VCS1_INTR_MASK	(0x1900a8)
#define GEN11_VCS2_VCS3_INTR_MASK	(0x1900ac)
#define GEN12_VCS4_VCS5_INTR_MASK	(0x1900b0)
#define GEN12_VCS6_VCS7_INTR_MASK	(0x1900b4)
#define GEN11_VECS0_VECS1_INTR_MASK	(0x1900d0)
#define GEN12_VECS2_VECS3_INTR_MASK	(0x1900d4)
#define GEN11_GUC_SG_INTR_MASK		(0x1900e8)
#define GEN11_GPM_WGBOXPERF_INTR_MASK	(0x1900ec)
#define GEN11_CRYPTO_RSVD_INTR_MASK	(0x1900f0)
#define GEN11_GUNIT_CSME_INTR_MASK	(0x1900f4)

#define GT_BLT_FLUSHDW_NOTIFY_INTERRUPT		(1 << 26)
#define GT_BLT_CS_ERROR_INTERRUPT		(1 << 25)
#define GT_BLT_USER_INTERRUPT			(1 << 22)
#define GT_BSD_CS_ERROR_INTERRUPT		(1 << 15)
#define GT_BSD_USER_INTERRUPT			(1 << 12)
#define GT_RENDER_L3_PARITY_ERROR_INTERRUPT_S1	(1 << 11) /* hsw+; rsvd on snb, ivb, vlv */
#define GT_CONTEXT_SWITCH_INTERRUPT		(1 <<  8)
#define GT_RENDER_L3_PARITY_ERROR_INTERRUPT	(1 <<  5) /* !snb */
#define GT_RENDER_PIPECTL_NOTIFY_INTERRUPT	(1 <<  4)
#define GT_RENDER_CS_MASTER_ERROR_INTERRUPT	(1 <<  3)
#define GT_RENDER_SYNC_STATUS_INTERRUPT		(1 <<  2)
#define GT_RENDER_DEBUG_INTERRUPT		(1 <<  1)
#define GT_RENDER_USER_INTERRUPT		(1 <<  0)

#define GEN11_GFX_MSTR_IRQ		(0x190010)
#define  GEN11_MASTER_IRQ		(1 << 31)
#define  GEN11_PCU_IRQ			(1 << 30)
#define  GEN11_GU_MISC_IRQ		(1 << 29)
#define  GEN11_DISPLAY_IRQ		(1 << 16)
#define  GEN11_GT_DW_IRQ(x)		(1 << (x))
#define  GEN11_GT_DW1_IRQ		(1 << 1)
#define  GEN11_GT_DW0_IRQ		(1 << 0)

#define GEN11_DISPLAY_INT_CTL		(0x44200)
#define  GEN11_DISPLAY_IRQ_ENABLE	(1 << 31)
#define  GEN11_AUDIO_CODEC_IRQ		(1 << 24)
#define  GEN11_DE_PCH_IRQ		(1 << 23)
#define  GEN11_DE_MISC_IRQ		(1 << 22)
#define  GEN11_DE_HPD_IRQ		(1 << 21)
#define  GEN11_DE_PORT_IRQ		(1 << 20)
#define  GEN11_DE_PIPE_C		(1 << 18)
#define  GEN11_DE_PIPE_B		(1 << 17)
#define  GEN11_DE_PIPE_A		(1 << 16)

#define GEN11_GT_INTR_DW0		(0x190018)
#define  GEN11_CSME			(31)
#define  GEN11_GUNIT			(28)
#define  GEN11_GUC			(25)
#define  GEN11_WDPERF			(20)
#define  GEN11_KCR			(19)
#define  GEN11_GTPM			(16)
#define  GEN11_BCS			(15)
#define  GEN11_RCS0			(0)

#define GEN11_GT_INTR_DW1		(0x19001c)
#define  GEN11_VECS(x)			(31 - (x))
#define  GEN11_VCS(x)			(x)
#define GT_CS_MASTER_ERROR_INTERRUPT		(3)
#define GT_WAIT_SEMAPHORE_INTERRUPT		(11)

enum ack_type {
	ACK_CLEAR = 0,
	ACK_SET
};

struct intel_rps_freq_caps {
	uint8_t rp0_freq;
	uint8_t rp1_freq;
	uint8_t min_freq;
};

constexpr uint32_t regForDom(unsigned d) {
	
	
	if (d == FORCEWAKE_GT)
		return FORCEWAKE_GT_GEN9;
	if (d == FORCEWAKE_RENDER)
		return FORCEWAKE_RENDER_GEN9;
	
		
	if (d == FORCEWAKE_MEDIA_VDBOX0)
			return FORCEWAKE_MEDIA_VDBOX_GEN11(0);
	if (d == FORCEWAKE_MEDIA_VDBOX1)
			return FORCEWAKE_MEDIA_VDBOX_GEN11(1);
	if (d == FORCEWAKE_MEDIA_VDBOX2)
			return FORCEWAKE_MEDIA_VDBOX_GEN11(2);
	if (d == FORCEWAKE_MEDIA_VDBOX3)
			return FORCEWAKE_MEDIA_VDBOX_GEN11(3);
	if (d == FORCEWAKE_MEDIA_VDBOX4)
			return FORCEWAKE_MEDIA_VDBOX_GEN11(4);
	if (d == FORCEWAKE_MEDIA_VDBOX5)
			return FORCEWAKE_MEDIA_VDBOX_GEN11(5);
	if (d == FORCEWAKE_MEDIA_VDBOX6)
			return FORCEWAKE_MEDIA_VDBOX_GEN11(6);
	if (d == FORCEWAKE_MEDIA_VDBOX7)
			return FORCEWAKE_MEDIA_VDBOX_GEN11(7);


	if (d == FORCEWAKE_MEDIA_VEBOX0)
			return FORCEWAKE_MEDIA_VEBOX_GEN11(0);
	if (d == FORCEWAKE_MEDIA_VEBOX1)
			return FORCEWAKE_MEDIA_VEBOX_GEN11(1);
	if (d == FORCEWAKE_MEDIA_VEBOX2)
			return FORCEWAKE_MEDIA_VEBOX_GEN11(2);
	if (d == FORCEWAKE_MEDIA_VEBOX3)
			return FORCEWAKE_MEDIA_VEBOX_GEN11(3);
	
	if (d == FORCEWAKE_GSC)
		return FORCEWAKE_REQ_GSC;

	assertf(false, "Unknown force wake domain %d", d);
	return 0;
}

constexpr uint32_t ackForDom(unsigned d) {
	if (d == FORCEWAKE_GT)
		return FORCEWAKE_ACK_GT_GEN9;
	if (d == FORCEWAKE_RENDER)
		return FORCEWAKE_ACK_RENDER_GEN9;
	
	if (d == FORCEWAKE_MEDIA_VDBOX0)
			return FORCEWAKE_ACK_MEDIA_VDBOX_GEN11(0);
	if (d == FORCEWAKE_MEDIA_VDBOX1)
			return FORCEWAKE_ACK_MEDIA_VDBOX_GEN11(1);
	if (d == FORCEWAKE_MEDIA_VDBOX2)
			return FORCEWAKE_ACK_MEDIA_VDBOX_GEN11(2);
	if (d == FORCEWAKE_MEDIA_VDBOX3)
			return FORCEWAKE_ACK_MEDIA_VDBOX_GEN11(3);
	if (d == FORCEWAKE_MEDIA_VDBOX4)
			return FORCEWAKE_ACK_MEDIA_VDBOX_GEN11(4);
	if (d == FORCEWAKE_MEDIA_VDBOX5)
			return FORCEWAKE_ACK_MEDIA_VDBOX_GEN11(5);
	if (d == FORCEWAKE_MEDIA_VDBOX6)
			return FORCEWAKE_ACK_MEDIA_VDBOX_GEN11(6);
	if (d == FORCEWAKE_MEDIA_VDBOX7)
			return FORCEWAKE_ACK_MEDIA_VDBOX_GEN11(7);


	if (d == FORCEWAKE_MEDIA_VEBOX0)
			return FORCEWAKE_ACK_MEDIA_VEBOX_GEN11(0);
	if (d == FORCEWAKE_MEDIA_VEBOX1)
			return FORCEWAKE_ACK_MEDIA_VEBOX_GEN11(1);
	if (d == FORCEWAKE_MEDIA_VEBOX2)
			return FORCEWAKE_ACK_MEDIA_VEBOX_GEN11(2);
	if (d == FORCEWAKE_MEDIA_VEBOX3)
			return FORCEWAKE_ACK_MEDIA_VEBOX_GEN11(3);
	
	if (d == FORCEWAKE_GSC)
		return FORCEWAKE_ACK_GSC;
	
	assertf(false, "Unknown force wake domain %d", d);
	return 0;
}

constexpr const char* const strForDom(unsigned d) {
	if (d == DOM_RENDER)
		return "Render";
	if (d == DOM_MEDIA)
		return "Media";
	if (d == DOM_BLITTER)
		return "Blitter";
	return "(unk)";
}

constexpr uint32_t masked_field(uint32_t mask, uint32_t value) {
	return (mask << 16) | value;
}

constexpr uint32_t fw_set(uint32_t v) {
	return masked_field(v, v);
}

constexpr uint32_t fw_clear(uint32_t v) {
	return masked_field(v, 0);
}



enum intel_pch {
	PCH_NOP = -1,	/* PCH without south display */
	PCH_NONE = 0,	/* No PCH present */
	PCH_IBX,	/* Ibexpeak PCH */
	PCH_CPT,	/* Cougarpoint/Pantherpoint PCH */
	PCH_LPT,	/* Lynxpoint/Wildcatpoint PCH */
	PCH_SPT,        /* Sunrisepoint/Kaby Lake PCH */
	PCH_CNP,        /* Cannon/Comet Lake PCH */
	PCH_ICP,	/* Ice Lake/Jasper Lake PCH */
	PCH_TGP,	/* Tiger Lake/Mule Creek Canyon PCH */
	PCH_ADP,	/* Alder Lake PCH */

	/* Fake PCHs, functionality handled on the same PCI dev */
	PCH_DG1 = 1024,
	PCH_DG2,
	PCH_MTL,
	PCH_LNL,
};


class Gen11 {
	friend class Genx;

private:
	
	
	static UInt8 wrapLoadGuCBinary(void *that);
	mach_vm_address_t orgLoadGuCBinary {};
	
	static bool wrapLoadFirmware(void *that);
	static bool wrapInitSchedControl(void *that);
	static void *wrapIgBufferWithOptions(void *accelTask, void* size, unsigned int type, unsigned int flags);
	static UInt64 wrapIgBufferGetGpuVirtualAddress(void *that);
	
	static int wrapPmNotifyWrapper(unsigned int a0, unsigned int a1, unsigned long long *a2, unsigned int *freq);
	mach_vm_address_t orgPmNotifyWrapper {};
	
	static bool start(void *that,void  *param_1);
	mach_vm_address_t ostart {};
	
	static bool patchRCSCheck(mach_vm_address_t& start);
	static void forceWake(void *that, bool set, uint32_t dom, uint8_t ctx);
	mach_vm_address_t oforceWake {};
	static bool pollRegister(uint32_t reg, uint32_t val, uint32_t mask, uint32_t timeout);
	static bool forceWakeWaitAckFallback(uint32_t d, uint32_t val, uint32_t mask);
	
	static void * serviceInterrupts(void *param_1);
	mach_vm_address_t oserviceInterrupts {};
	
	static int isConflictRegister();
	
	static void dovoid();
	static bool dotrue();
	
	static inline uint32_t get_le32(const uint8_t *b)
	{
		return (uint32_t)b[0] | (uint32_t)b[1] << 8 | (uint32_t)b[2] << 16 | (uint32_t)b[3] << 24;
	}
	
	void *framecont;
	
	mach_vm_address_t orgSubmitExecList {};
	mach_vm_address_t orgLoadFirmware {};
	mach_vm_address_t orgInitSchedControl {};
	mach_vm_address_t orgIgBufferWithOptions {};
	mach_vm_address_t orgIgBufferGetGpuVirtualAddress {};
	
	mach_vm_address_t _gSysctlVariables {};
	
	void* gPlatformInformationList{nullptr};
	
	uint32_t freq_max {0};
	uint8_t *gKmGen9GuCBinary {nullptr};
	uint8_t *signaturePointer {nullptr};
	uint32_t *firmwareSizePointer {nullptr};
	uint8_t *dummyFirmwareBuffer {nullptr};
	uint8_t *realFirmwareBuffer {nullptr};
	uint32_t realBinarySize {};
	
	static uint8_t validateModeDepth(void *that,void *param_1,uint param_2);
	mach_vm_address_t ovalidateModeDepth {};
	
	
	bool performingFirmwareLoad {false};
	
	static int handleLinkIntegrityCheck();
	
	static void wrapSystemWillSleep(void *that);
	static void wrapSystemDidWake(void *that);
	

	static uint64_t  getOSInformation(void *that);
	mach_vm_address_t ogetOSInformation {};
	
	static uint64_t setPanelPowerState(void *that,uint param_1);
	mach_vm_address_t osetPanelPowerState {};
	
	static uint32_t enableVDDForAux(void *that,void *param_1);
	mach_vm_address_t oenableVDDForAux {};
	
	static uint32_t disableVDDForAux(void *that);
	mach_vm_address_t odisableVDDForAux {};
	
	static void  prepareToEnterWake(void *that);
	mach_vm_address_t oprepareToEnterWake {};
	
	static void  initBlitUsage(void *that);
	mach_vm_address_t oinitBlitUsage {};
	static void  markBlitUsage(void *that);
	mach_vm_address_t omarkBlitUsage {};
	
	static uint32_t SetFbStatusOnNextProbe(void *that,AGDCFBOnline_t *param_1);
	mach_vm_address_t oSetFbStatusOnNextProbe {};
	
	static unsigned long  isHPDLow(void *that);
	mach_vm_address_t oisHPDLow {};
	
	static void hwWriteMailbox(void *that,uint param_1,uint param_2,bool param_3);
	mach_vm_address_t ohwWriteMailbox {};
	
	static void  enableComboPhy(void *that);
	mach_vm_address_t oenableComboPhy {};
	
	static uint32_t  IGAccelSegmentResourceListprepare(void *that);
	mach_vm_address_t oIGAccelSegmentResourceListprepare {};
	
	static uint64_t setupPort(void *that);
	mach_vm_address_t osetupPort {};
	
	static void  initPlatformWorkarounds(void *that);
	mach_vm_address_t oinitPlatformWorkarounds {};
	
	static void  getGPUInfo(void *this_ptr);
	mach_vm_address_t ogetGPUInfo {};
	
	static void  enableSWDC6(void *that,bool param_1);
	mach_vm_address_t oenableSWDC6 {};
	
	static uint32_t setLinkTrainingPattern(void *that,uint8_t param_1);
	mach_vm_address_t osetLinkTrainingPattern {};
	
	
	static unsigned long fastLinkTraining(void *that);
	mach_vm_address_t ofastLinkTraining {};
	
	static IOReturn wrapFBClientDoAttribute(void *fbclient, uint32_t attribute, unsigned long *unk1, unsigned long unk2, unsigned long *unk3, unsigned long *unk4, void *externalMethodArguments);
	mach_vm_address_t orgFBClientDoAttribute {};
	
	static uint32_t
	IntelFBClientControldoAttribute
			  (void *that,uint param_1,unsigned long *param_2,unsigned long param_3,unsigned long *param_4,
			   unsigned long *param_5,void *param_6);
	mach_vm_address_t oIntelFBClientControldoAttribute {};
	
	static IOReturn wrapPavpSessionCallback(void *intelAccelerator, int32_t sessionCommand, uint32_t sessionAppId, uint32_t *a4, bool flag);
	mach_vm_address_t orgPavpSessionCallback {};
	
	const uint8_t *_CSR_PATCH_B0plus;
	const uint8_t *_CSR_PATCH_AX;
	
	static uint64_t enableController(void *that);
	mach_vm_address_t oenableController {};
	
	static void  displayModeDidChange(void *that);
	mach_vm_address_t odisplayModeDidChange {};
	
	static uint32_t  AppleIntelPortinit(void *that,void *param_1);
	mach_vm_address_t oAppleIntelPortinit {};
	
	static uint64_t getLinkConfig(void *that,IOFBDPLinkConfig *param_1);
	mach_vm_address_t ogetLinkConfig {};
	
	static bool  IsThisCamellia2(void *that);
	mach_vm_address_t oIsThisCamellia2 {};
	
	static uint64_t  probeDisplayPipe(void *that);
	mach_vm_address_t oprobeDisplayPipe {};
	
	static uint8_t connectionChanged(void *that);
	mach_vm_address_t oconnectionChanged {};
	
	static uint32_t getFreeJoinablePathCount(void);
	
	static int  connectionProbe(void *that);
	mach_vm_address_t oconnectionProbe {};
	
	static void wrapWriteRegister32(void *controller, uint32_t address, uint32_t value);
	mach_vm_address_t owrapWriteRegister32 {};
	
	static uint32_t wrapReadRegister32(void *controller, uint32_t address);
	mach_vm_address_t owrapReadRegister32 {};
	
	
	static uint64_t ssetFramebufferPowerState(void *that,uint param_1);
	mach_vm_address_t ossetFramebufferPowerState {};
	
	static int64_t ddpsmNotify(void *that,uint *param_1);
	mach_vm_address_t oddpsmNotify {};
	
	mach_vm_address_t kIGHwCsDesc {};
	mach_vm_address_t g_timestampFreqPerDenominator {};
	
	static uint8_t beginReset(void *that);
    mach_vm_address_t obeginReset {};
												
	static void endReset(void *that);
	mach_vm_address_t oendReset {};
	
	static void* getFirstPathByPort(void *that);
	mach_vm_address_t ogetFirstPathByPort {};
	
	static void  setDCState(void *that,bool param_1);
	mach_vm_address_t osetDCState {};
												
	static void hwInitializeCState(void *that);
	mach_vm_address_t ohwInitializeCState {};
	
	static uint32_t DisplaySleepCallback(void *that,uint32_t param_1,uint param_2,uint param_3);
	mach_vm_address_t oDisplaySleepCallback {};
	
	static void hwConfigureCustomAUX(void *that,bool param_1);
	mach_vm_address_t ohwConfigureCustomAUX {};
	
	static void FastWriteRegister32(void *that,unsigned long param_1,uint32_t param_2);
	mach_vm_address_t oFastWriteRegister32 {};
	
	static void checkWOPCMSettings(void *that,unsigned long param_1,void *param_2);
	mach_vm_address_t ocheckWOPCMSettings {};
	
	static void  setAsyncSliceCount(void *that,uint32_t configRaw);
	mach_vm_address_t osetAsyncSliceCount {};
	
	static void  setAsyncSliceCount2(void *that,uint32_t configRaw);
	mach_vm_address_t osetAsyncSliceCount2 {};
	
	static int  enablePorth(void *that);
	mach_vm_address_t oenablePorth {};
	
	static unsigned long  loadGuCBinary(void *that);
	mach_vm_address_t oloadGuCBinary {};
	
	static uint8_t setDisplayMode(void *that,int param_1,int param_2);
	mach_vm_address_t osetDisplayMode {};
	
	static unsigned long hwRegsNeedUpdate
			  (void *that,void *param_1,
			   void *param_2,void *param_3,void *param_4,
			   void *param_5);
	mach_vm_address_t ohwRegsNeedUpdate {};
	
	static void blit3d_submit_rectlist(void *param_1,void *param_2,void *param_3);
	mach_vm_address_t oblit3d_submit_rectlist {};
	
	static IOReturn wrapICLReadAUX(void *that, uint32_t address, void *buffer, uint32_t length);
	mach_vm_address_t orgICLReadAUX {};
	
	static uint32_t CallBackAGDC(void *that,uint32_t param_1,unsigned long param_2, uint param_3);
	mach_vm_address_t oCallBackAGDC {};
	
	static uint64_t  linkTraining(void *that,void *param_1);
	mach_vm_address_t olinkTraining {};
	
	static int getPlatformID();
	mach_vm_address_t ogetPlatformID {};
	
	static void logStateInRegistry(void *that,uint param_1);
	mach_vm_address_t ologStateInRegistry {};
	
	static bool	AppleIntelFramebufferinit(void *frame,void *cont,uint32_t param_2);
	mach_vm_address_t oAppleIntelFramebufferinit {};
	
	static uint64_t  AppleIntelPlaneinit(void *that,uint8_t param_1);
	mach_vm_address_t oAppleIntelPlaneinit {};
	
	static unsigned long AppleIntelScalerinit(void *that,uint8_t param_1);
	mach_vm_address_t oAppleIntelScalerinit {};
	
	static int  handleEnableHDCP(void *that);
	mach_vm_address_t ohandleEnableHDCP {};
	
	static void  enablePlane(void *that,bool param_1);
	mach_vm_address_t oenablePlane {};
	
	
	static void  disablePowerWellPG(void *that,uint param_1);
	mach_vm_address_t odisablePowerWellPG {};
	
	static uint64_t setVoltageSwingAndPreEmphasis(void *that, uint32_t voltageSwing, uint32_t preEmphasis, PortDriveConfig *customDriveConfig);
	mach_vm_address_t osetVoltageSwingAndPreEmphasis {};
	
	static void  enablePowerWellPG(void *that,uint param_1);
	mach_vm_address_t oenablePowerWellPG {};
	
	static uint32_t hwReadMailbox(void *that,uint param_1);
	mach_vm_address_t ohwReadMailbox {};
	
	static void  overridePowerWellsState(void *that,bool param_1);
	mach_vm_address_t ooverridePowerWellsState {};
	
	static void hwSetPowerWellStatePG(void *that,bool param_1,uint param_2);
	mach_vm_address_t ohwSetPowerWellStatePG {};
	
	static void hwSetPowerWellStateDDI(void *that,bool param_1,uint param_2);
	mach_vm_address_t ohwSetPowerWellStateDDI {};
	
	static void hwSetPowerWellStateAux(void *that,bool param_1,uint param_2);
	mach_vm_address_t ohwSetPowerWellStateAux {};
	
	static void SetupParams (void *that,void *param_1,void *param_2,void *param_3,void *param_4);
	mach_vm_address_t oSetupParams {};
	
	static void AppleIntelPowerWellinit(void *that,void *param_1);
	mach_vm_address_t oAppleIntelPowerWellinit {};
	
	static int hasExternalDispla();
	

	static void disableDisplayEngine(void *that);
	mach_vm_address_t odisableDisplayEngine {};
	
	static int writeAUX(void *that,uint param_1,void *param_2,uint param_3);
	mach_vm_address_t owriteAUX {};
	
	static int readAUX(void *that,uint param_1,void *param_2,uint param_3);
	mach_vm_address_t oreadAUX {};
	
	static uint32_t  getDPCDInfo(void *that);
	mach_vm_address_t ogetDPCDInfo {};
	
	static void  disableScaler(void *that,bool param_1);
	mach_vm_address_t odisableScaler {};
	
	static void  IGScheduler5resume(void *that);
	mach_vm_address_t oIGScheduler5resume {};
	
	static void initializeLogging(void *that);
	mach_vm_address_t oinitializeLogging {};
	
	static void sanitizeCDClockFrequency(void *that);
	static uint32_t wrapProbeCDClockFrequency(void *that);
	
	static void  readAndClearInterrupts(void *that,void *param_1);
	mach_vm_address_t oreadAndClearInterrupts {};
	
	static void initCDClock(void *that);
	mach_vm_address_t oinitCDClock {};
	
	static void setCDClockFrequencyOnHotplug(void *that);
	mach_vm_address_t osetCDClockFrequencyOnHotplug {};
	
	static void disableCDClock(void *that);
	mach_vm_address_t odisableCDClock {};
	
	uint32_t (*orgProbeCDClockFrequency)(void *) {nullptr};
	void (*orgDisableCDClock)(void *) {nullptr};
	void (*orgSetCDClockFrequency)(void *, unsigned long long) {nullptr};
	
	static void * wprobe(void *that,void *param_1,int *param_2);
	mach_vm_address_t owprobe {};
	
	static bool  tgstart(void *that,void *param_1);
	mach_vm_address_t otgstart {};
	
	static int hwSetMode
			  (void *that,void *param_1,
			   void *param_2,void *param_3);
	mach_vm_address_t ohwSetMode {};
	
	static void enablePipe
			  (void *that,void *param_1,
			   void *param_2,void *param_3);
	mach_vm_address_t oenablePipe {};
	
	
	mach_vm_address_t IntelFBClientControl11doAttribut {};

	static uint32_t probePortMode();
	mach_vm_address_t oprobePortMode {};
	
	static uint32_t wdepthFromAttribute(void *that,uint param_1);
	
	static uint32_t raReadRegister32(void *that,unsigned long param_1);
	mach_vm_address_t oraReadRegister32 {};
	
	static unsigned long raReadRegister32b(void *that,void *param_1,unsigned long param_2);
	mach_vm_address_t oraReadRegister32b {};
	
	static void SafeForceWake(void *that,bool param_1,uint param_2);
	mach_vm_address_t oSafeForceWake {};
	
	static void SafeForceWake2(void *that,bool param_1,uint param_2);
	mach_vm_address_t oSafeForceWake2 {};
	
	
	static unsigned long resetGraphicsEngine(void *that,void *param_1);
	mach_vm_address_t oresetGraphicsEngine {};
	
	static uint64_t raReadRegister64(void *that,unsigned long param_1);
	mach_vm_address_t oraReadRegister64 {};
	
	static uint64_t raReadRegister64b(void *that,void *param_1,unsigned long param_2);
	
	static void radWriteRegister32(void *that,unsigned long param_1, UInt32 param_2);
	mach_vm_address_t oradWriteRegister32 {};
	static void radWriteRegister32f(void *that,unsigned long param_1, UInt32 param_2);
	mach_vm_address_t oradWriteRegister32f {};
	
	static void raWriteRegister32(void *that,unsigned long param_1, UInt32 param_2);
	mach_vm_address_t oraWriteRegister32 {};
	
	static void raWriteRegister32b(void *that,void *param_1,unsigned long param_2, UInt32 param_3);
	
	static void raWriteRegister32f(void *that,unsigned long param_1, UInt32 param_2);
	mach_vm_address_t oraWriteRegister32f {};
	
	static void raWriteRegister64(void *that,unsigned long param_1,UInt64 param_2);
	mach_vm_address_t oraWriteRegister64 {};
	
	static void raWriteRegister64b(void *that,void *param_1,unsigned long param_2,UInt64 param_3);
	
	static void setupPlanarSurfaceDBUF();
	mach_vm_address_t osetupPlanarSurfaceDBUF {};
	
	static void updateDBUF(void *that,uint param_1,uint param_2,bool param_3);
	
	static void FBMemMgr_Init(void *that);
	mach_vm_address_t oFBMemMgr_Init {};
	
	static int blit3d_supported(void *param_1,void *param_2);
	mach_vm_address_t oblit3d_supported {};
	
	static int LightUpEDP(void *that,void *param_1, void *param_2,void *param_3);
	mach_vm_address_t oLightUpEDP {};
	
	/*
	mach_vm_address_t PowerWell {};
	mach_vm_address_t PortHAL {};
	mach_vm_address_t PortHALDiags {};
	mach_vm_address_t AppleIntelPort {};
	*/
	static void IGHardwareBlit3DContextinitialize(void *that);
	mach_vm_address_t oIGHardwareBlit3DContextinitialize {};
	
	static bool  initHardwareCaps(void *this_ptr);
	mach_vm_address_t oinitHardwareCaps {};
	
	static uint8_t IGMappedBuffergetMemory(void *that);
	mach_vm_address_t oIGMappedBuffergetMemory {};
	
	static void *  IGHardwareBlit3DContextoperatornew(void *that,unsigned long param_1);
	mach_vm_address_t oIGHardwareBlit3DContextoperatornew {};
	
	static uint8_t blit3d_init_ctx(void *that);
	mach_vm_address_t oblit3d_init_ctx {};
	
	static void blit3d_initialize_scratch_space(void *that);
	mach_vm_address_t oblit3d_initialize_scratch_space {};
	
	static uint8_t	IGHardwareExtendedContextinitWithOptions
			  (void *that,void *param_1,
			   void *param_2);
	mach_vm_address_t oIGHardwareExtendedContextinitWithOptions {};
	
	mach_vm_address_t ExtendedCtxParams {};
	mach_vm_address_t Blit2DExtendedCtxParams {};
	mach_vm_address_t Blit3DExtendedCtxParams {};
	
	static uint8_t isPanelPowerOn(void *that);
	mach_vm_address_t oisPanelPowerOn {};
	
	static uint8_t setupAdditionalDataStructs();
	mach_vm_address_t osetupAdditionalDataStructs {};
	
	mach_vm_address_t ZN15AppleIntelPlaneC1Ev {};
	mach_vm_address_t ZN16AppleIntelScalerC1Ev {};
	
	mach_vm_address_t ZN16AppleIntelScaler10gMetaClassE {};
	mach_vm_address_t ZN15AppleIntelPlane10gMetaClassE {};
	
	static void * getBlit3DContext(void *that,bool param_1);
	mach_vm_address_t ogetBlit3DContext {};
	
	
	static unsigned long  allocateDisplayResources(void *that);
	mach_vm_address_t oallocateDisplayResources {};
	
	static void  enableDisplayEngine(void *that);
	mach_vm_address_t oenableDisplayEngine {};
	
	static void * getBlit2DContext(void *that,bool param_1);
	mach_vm_address_t ogetBlit2DContext {};
	
	static void * ExtendedContextWithOptions(void *param_1);
	mach_vm_address_t oExtendedContextWithOptions {};
	
	static void  AppleIntelPlanec1(void *that);
	static void  AppleIntelScalerc1(void *that);
	
	static void * AppleIntelScalernew(unsigned long param_1);
	mach_vm_address_t oAppleIntelScalernew {};
	
	
	static void programPipeScaler(void *that,void *param_1);
	mach_vm_address_t oprogramPipeScaler {};
	
	static void AppleIntelScalerupdateRegisterCache(void *that);
	mach_vm_address_t oAppleIntelScalerupdateRegisterCache {};
	
	static void AppleIntelPlaneupdateRegisterCache(void *that);
	mach_vm_address_t oAppleIntelPlaneupdateRegisterCache {};
	
	static void * AppleIntelPlanenew(unsigned long param_1);
	mach_vm_address_t oAppleIntelPlanenew {};
	
	static void uupdateDBUF(void *that,uint param_1,uint param_2,bool param_3);
		
	static long getPortByDDI(uint param_1);
	mach_vm_address_t ogetPortByDDI {};
	
	static void PowerWellinit(void *that,void *param_1);
	mach_vm_address_t oPowerWellinit {};
	
	static uint8_t  setPortMode(void *that,uint32_t param_1);
	mach_vm_address_t osetPortMode {};
	
	
public:

	void init();
	static Gen11 *callback;
	bool processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size);
	
	static void tWriteRegister32(unsigned long a, unsigned int b);
	static void tWriteRegister64(void volatile* a, unsigned long b, unsigned long long c);
	static unsigned int tReadRegister32(unsigned long a);
	static unsigned long long tReadRegister64(void volatile* a, unsigned long b);
	static uint64_t tgetPMTNow();
	static bool thwSetupDSBMemory();
	static uint32_t tprobePortMode(void * that);
	
	
};

#endif /* kern_gen8_hpp */


