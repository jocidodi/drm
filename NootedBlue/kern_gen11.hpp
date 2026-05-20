//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#ifndef kern_gen11_hpp
#define kern_gen11_hpp
#include "kern_nblue.hpp"
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




struct PACKED VoltageConfig {
	uint64_t pad;
	uint8_t* voltageData;
};

struct AGDCFBOnline_t {
	int32_t                 id;
	uint32_t                state;
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


struct PACKED FramebufferCNLCurrents {
	uint32_t value1;
	uint32_t pad;
	uint64_t valu2;
};

struct PACKED FramebufferICLLP {
	uint32_t framebufferId;

	uint32_t fPchType;
	uint64_t *fModelNameAddr;
	uint8_t  fMobile;
	uint8_t  fPipeCount;
	uint8_t  fPortCount;
	uint8_t  fInfoFramebufferCount;
	uint32_t fStolenMemorySize;
	uint32_t fFramebufferMemorySize;
	uint32_t fUnifiedMemorySize;
	ConnectorInfo connectors[6];
	uint32_t flags;
	uint32_t unk2;
	FramebufferCNLCurrents currents[3];
	uint32_t unk3[2];
	uint32_t camelliaVersion;
	uint32_t unk4[3];
	uint32_t fNumTransactionsThreshold;
	uint32_t fVideoTurboFreq;
	uint32_t fSliceCount;
	uint32_t fEuCount;
	uint32_t fSubSliceCount;
	uint8_t unk6;
	uint8_t pad[3];
};





enum IGHwCsType
{
	kIGHwCsTypeRCS,     //  0
	kIGHwCsTypeCCS,     //  1
	kIGHwCsTypeBCS,     //  2
	kIGHwCsTypeVCS0,    //  3
	kIGHwCsTypeVCS2,    //  4
	kIGHwCsTypeVECS0,   //  5
};

//size is 0x79 do not pack this thing as its size will be 0x80 and bug apple code
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




class Gen11 {

private:
	
	void* gPlatformInformationList{nullptr};
	void* kIGHwCsDesc {};
	void* gPlatformInformationList2{nullptr};
	
	static void  initPlatformWorkarounds(void *that);
	mach_vm_address_t oinitPlatformWorkarounds {};

	static uint64_t  getOSInformation(void *that);
	mach_vm_address_t ogetOSInformation {};
	
	static IOReturn wrapPavpSessionCallback(void *intelAccelerator, int32_t sessionCommand, uint32_t sessionAppId, uint32_t *a4, bool flag);
	mach_vm_address_t orgPavpSessionCallback {};
	
	static uint32_t	AppleIntelFramebufferinit(void *frame,void *cont,uint param_2);
	mach_vm_address_t oAppleIntelFramebufferinit {};
	
	static uint64_t  AppleIntelPlaneinit(void *that,uint param_1);
	mach_vm_address_t oAppleIntelPlaneinit {};
	
	static void FBMemMgr_Init(void *that);
	mach_vm_address_t oFBMemMgr_Init {};
	
	static unsigned long AppleIntelScalerinit(void *that,uint param_1);
	mach_vm_address_t oAppleIntelScalerinit {};
	
	static void  initPlatformWorkarounds2(void *that);
	mach_vm_address_t oinitPlatformWorkarounds2 {};

	static uint64_t  getOSInformation2(void *that);
	mach_vm_address_t ogetOSInformation2 {};

	
	
public:

	void init();
	static Gen11 *callback;
	bool processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size);
	static void dovoid();
	static bool dotrue();
	static bool dofalse();
	static int dozero();
	
};

#endif


