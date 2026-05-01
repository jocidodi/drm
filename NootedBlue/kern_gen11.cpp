//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.
#include "kern_gen11.hpp"
#include <Headers/kern_api.hpp>

static const char *pathG11FB = "/System/Library/Extensions/AppleIntelICLLPGraphicsFramebuffer.kext/Contents/MacOS/"
                               "AppleIntelICLLPGraphicsFramebuffer";
static const char *pathG11FBT = "/Library/Extensions/AppleIntelTGLGraphicsFramebuffer.kext/Contents/MacOS/"
							   "AppleIntelTGLGraphicsFramebuffer";

static const char *pathG11HW =
    "/System/Library/Extensions/AppleIntelICLGraphics.kext/Contents/MacOS/AppleIntelICLGraphics";
static const char *pathG11HWT =
	"/System/Library/Extensions/AppleIntelTGLGraphics.kext/Contents/MacOS/AppleIntelTGLGraphics";
static const char *pathG11HWTe =
	"/Library/Extensions/AppleIntelTGLGraphics.kext/Contents/MacOS/AppleIntelTGLGraphics";

static KernelPatcher::KextInfo kextG11FB {"com.apple.driver.AppleIntelICLLPGraphicsFramebuffer", &pathG11FB, 1, {}, {},
    KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextG11FBT {"com.xxxxx.driver.AppleIntelTGLGraphicsFramebuffer", &pathG11FBT, 1, {}, {},
	KernelPatcher::KextInfo::Unloaded};

static KernelPatcher::KextInfo kextG11HW {"com.apple.driver.AppleIntelICLGraphics", &pathG11HW, 1, {}, {},
    KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextG11HWT {"com.apple.driver.AppleIntelTGLGraphics", &pathG11HWT, 1, {}, {},
	KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextG11HWTe {"com.xxxxx.driver.AppleIntelTGLGraphics", &pathG11HWTe, 1, {}, {},
	KernelPatcher::KextInfo::Unloaded};



Gen11 *Gen11::callback = nullptr;

void Gen11::init() {
	callback = this;
	
    lilu.onKextLoadForce(&kextG11FB);
	lilu.onKextLoadForce(&kextG11FBT);
	
    lilu.onKextLoadForce(&kextG11HW);
	lilu.onKextLoadForce(&kextG11HWT);
	lilu.onKextLoadForce(&kextG11HWTe);
}



bool Gen11::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
	
	if (kextG11FB.loadIndex == index) {

		NBlue::callback->setRMMIOIfNecessary();
		
		/*SolveRequestPlus solveRequests[] = {
			{"__ZN31AppleIntelFramebufferController14disableCDClockEv", this->orgDisableCDClock},
			{"__ZN31AppleIntelFramebufferController19setCDClockFrequencyEy", this->orgSetCDClockFrequency},
		};
		PANIC_COND(!SolveRequestPlus::solveAll(patcher, index, solveRequests, address, size), "nblue",	"Failed to resolve symbols");*/
		
		RouteRequestPlus requests[] = {

			//{"__ZN31AppleIntelFramebufferController21probeCDClockFrequencyEv",wrapProbeCDClockFrequency,	this->orgProbeCDClockFrequency},
			//{"__ZN31AppleIntelFramebufferController14ReadRegister32Em",wrapReadRegister32,	this->owrapReadRegister32},
			{"__ZN21AppleIntelFramebuffer13SaveNVRAMModeEv",handleLinkIntegrityCheck},
			{"__ZN21AppleIntelFramebuffer18prepareToEnterWakeEv",dovoid},
			{"__ZN21AppleIntelFramebuffer17prepareToExitWakeEv",dovoid},
			{"__ZN21AppleIntelFramebuffer18prepareToExitSleepEv",dovoid},
			{"__ZN21AppleIntelFramebuffer19prepareToEnterSleepEv",dovoid},
			{"__ZN20IntelFBClientControl11doAttributeEjPmmS0_S0_P25IOExternalMethodArguments",wrapFBClientDoAttribute,	this->orgFBClientDoAttribute},

		};
		PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route symbols");
		
		static const uint8_t f15[]= {0x00,0x02, 0x00, 0x5c, 0x8a};
		static const uint8_t r15[]= {0x00,0x00, 0x00, 0x49, 0x9a};
		
		
		// AppleIntelFramebufferController::hwSetMode skip hwRegsNeedUpdate
		static const uint8_t f2[] = {0xE8, 0x31, 0xE5, 0xFF, 0xFF, 0x84, 0xC0, 0x74, 0x3D};
		static const uint8_t r2[] = {0xE8, 0x31, 0xE5, 0xFF, 0xFF, 0x84, 0xC0, 0xEB, 0x3D};
		
		//sonoma
		static const uint8_t f2b[] = {0xE8, 0x54, 0xEA, 0xFF, 0xFF, 0x84, 0xC0, 0x74, 0x5C};
		static const uint8_t r2b[] = {0xE8, 0x54, 0xEA, 0xFF, 0xFF, 0x84, 0xC0, 0xeb, 0x5C};
		
		//sequoia
		static const uint8_t f2c[] = {0xE8, 0x74, 0xEA, 0xFF, 0xFF, 0x84, 0xC0, 0x74, 0x5C};
		static const uint8_t r2c[] = {0xE8, 0x74, 0xEA, 0xFF, 0xFF, 0x84, 0xC0, 0xeb, 0x5C};
		
		if (getKernelVersion() <= KernelVersion::Ventura) {
			KernelPatcher::LookupPatch patch { &kextG11FB, f2, r2, sizeof(f2), 1 };
			patcher.applyLookupPatch(&patch);
		}
		
		if (getKernelVersion() == KernelVersion::Sonoma) {
			KernelPatcher::LookupPatch patchb { &kextG11FB, f2b, r2b, sizeof(f2b), 1 };
			patcher.applyLookupPatch(&patchb);
		}
		
		if (getKernelVersion() >= KernelVersion::Sequoia) {
			KernelPatcher::LookupPatch patchc { &kextG11FB, f2c, r2c, sizeof(f2c), 1 };
			patcher.applyLookupPatch(&patchc);
		}
		
		

		LookupPatchPlus const patches[] = {

			{&kextG11FB, f15, r15, arrsize(f15),    1},
		};
		
		
		
		PANIC_COND(!LookupPatchPlus::applyAll(patcher, patches , address, size), "nblue", "kextG11FB Failed to apply patches!");

		DBGLOG("nblue", "Loaded AppleIntelICLLPGraphicsFramebuffer!");
		return true;
		
		
	}	else if (kextG11FBT.loadIndex == index) {
		NBlue::callback->setRMMIOIfNecessary();
		SYSLOG("nblue", "AppleIntelTGLGraphicsFramebuffer");
		
		bool isprod=false;
		auto prod=patcher.solveSymbol(index, "__ZN24AppleIntelBaseController5startEP9IOService", address, size);
		if (!prod) isprod=true;
		
		if (isprod) {
			SolveRequestPlus solveRequests[] = {
				//{"__ZN31AppleIntelFramebufferController19setCDClockFrequencyEy", this->orgSetCDClockFrequency},
				{"_gPlatformInformationList", this->gPlatformInformationList},
			};
			PANIC_COND(!SolveRequestPlus::solveAll(patcher, index, solveRequests, address, size), "nblue",	"Failed to resolve symbols");
		}
		else
		{
			SolveRequestPlus solveRequests[] = {
				//{"__ZN24AppleIntelBaseController19setCDClockFrequencyEy", this->orgSetCDClockFrequency},
				{"_gPlatformInformationList", this->gPlatformInformationList},
				
			};
			PANIC_COND(!SolveRequestPlus::solveAll(patcher, index, solveRequests, address, size), "nblue",	"Failed to resolve symbols");
		}

		
		RouteRequestPlus requests[] = {
			
			{"__ZN16AppleIntelScaler4initE10IGScalerID", AppleIntelScalerinit,this->oAppleIntelScalerinit},
			{"__ZN15AppleIntelPlane4initE9IGPlaneID", AppleIntelPlaneinit,this->oAppleIntelPlaneinit},

			
			/*{"__ZN16AppleIntelScaler13disableScalerEb",disableScaler, this->odisableScaler},
			
			{"__ZN16AppleIntelScaler17programPipeScalerEP21AppleIntelDisplayPath",programPipeScaler, this->oprogramPipeScaler},
			{"__ZN15AppleIntelPlane19updateRegisterCacheEv",AppleIntelPlaneupdateRegisterCache, this->oAppleIntelPlaneupdateRegisterCache},
			{"__ZN16AppleIntelScaler19updateRegisterCacheEv",AppleIntelScalerupdateRegisterCache, this->oAppleIntelScalerupdateRegisterCache},
			{"__ZN19AppleIntelPowerWell20disableDisplayEngineEv",disableDisplayEngine, this->odisableDisplayEngine},
			
			{"__ZN19AppleIntelPowerWell22hwSetPowerWellStateAuxEbj",hwSetPowerWellStateAux, this->ohwSetPowerWellStateAux},
			{"__ZN19AppleIntelPowerWell22hwSetPowerWellStateDDIEbj",hwSetPowerWellStateDDI, this->ohwSetPowerWellStateDDI},*/
			//{"__ZN31AppleIntelRegisterAccessManager19FastWriteRegister32Emj",FastWriteRegister32, this->oFastWriteRegister32},
			//{"__ZN21AppleIntelFramebuffer24allocateDisplayResourcesEv",allocateDisplayResources, this->oallocateDisplayResources},
			
			//{"__ZN19AppleIntelPowerWell19enableDisplayEngineEv",enableDisplayEngine, this->oenableDisplayEngine},
			
			
			{"__ZN24AppleIntelBaseController13hwReadMailboxEj",hwReadMailbox, this->ohwReadMailbox},
			{"__ZN24AppleIntelBaseController14hwWriteMailboxEjjb",hwWriteMailbox, this->ohwWriteMailbox},
			//{"__ZN19AppleIntelPowerWell22hwSetPowerWellStateAuxEbj",hwSetPowerWellStateAux, this->ohwSetPowerWellStateAux},
			{"__ZN17AppleIntelPortHAL14enableComboPhyEv",enableComboPhy, this->oenableComboPhy},
			
			//{"__ZN19AppleIntelPowerWell21hwSetPowerWellStatePGEbj",hwSetPowerWellStatePG, this->ohwSetPowerWellStatePG},
			//{"__ZN19AppleIntelPowerWell23overridePowerWellsStateEb",overridePowerWellsState, this->ooverridePowerWellsState},
			
			
			
			//{"__ZN21AppleIntelFramebuffer14setDisplayModeEii",setDisplayMode, this->osetDisplayMode},

			//{"__ZN20IntelFBClientControl11doAttributeEjPmmS0_S0_P25IOExternalMethodArguments",IntelFBClientControldoAttribute,	this->oIntelFBClientControldoAttribute},

			//{"__ZN24AppleIntelBaseController10enablePipeEP21AppleIntelFramebufferP21AppleIntelDisplayPathPK29IODetailedTimingInformationV2",enablePipe, this->oenablePipe},
			
			//{"__ZN21AppleIntelFramebuffer17connectionChangedEv",connectionChanged, this->oconnectionChanged},

			//{"__ZN12CamelliaBase15IsThisCamellia2Ev",IsThisCamellia2, this->oIsThisCamellia2},

			//
			//{"__ZN21AppleIntelDisplayPath13getLinkConfigEP16IOFBDPLinkConfig",getLinkConfig, this->ogetLinkConfig},

			//{"__ZN24AppleIntelBaseController12CallBackAGDCE31kAGDCRegisterLinkControlEvent_tmj",CallBackAGDC,	this->oCallBackAGDC},
			
			//{"__ZN24AppleIntelBaseController21hwSetPanelPowerConfigEj",dovoid},

			//{"__ZN24AppleIntelBaseController11enableSWDC6Eb",enableSWDC6, this->oenableSWDC6},
			//{"__ZN24AppleIntelBaseController10dpsmNotifyEPj",ddpsmNotify, this->oddpsmNotify},
			//{"__ZN24AppleIntelBaseController10setDCStateEb",setDCState, this->osetDCState},
			
			//{"__ZN21AppleIntelFramebuffer16enableControllerEv",enableController, this->oenableController},
			//{"__ZN15AppleIntelHDCP116handleEnableHDCPEv",handleEnableHDCP, this->ohandleEnableHDCP},
			//{"__ZN21AppleIntelFramebuffer24setFramebufferPowerStateEj",ssetFramebufferPowerState, this->ossetFramebufferPowerState},
			
			//{"__ZN24AppleIntelBaseController11SetupParamsEP21AppleIntelFramebufferP21AppleIntelDisplayPathP10CRTCParamsPK29IODetailedTimingInformationV2",SetupParams,	this->oSetupParams},
			//{"__ZN14AppleIntelPort16fastLinkTrainingEv",fastLinkTraining, this->ofastLinkTraining},
			
			//{"__ZN24AppleIntelBaseController10LightUpEDPEP21AppleIntelFramebufferP21AppleIntelDisplayPathPK29IODetailedTimingInformationV2",LightUpEDP,	this->oLightUpEDP},
			
			//{"__ZN24AppleIntelBaseController20DisplaySleepCallbackE17DisplaySleepCmd_tjj",DisplaySleepCallback, this->oDisplaySleepCallback},
			/*
			{"__ZN14AppleIntelPort15checkLinkStatusEv",handleLinkIntegrityCheck},
			
			*/
			
			
			
			//{"__ZN14AppleIntelPort9setupPortEv",setupPort, this->osetupPort},
			//{"__ZN21AppleIntelDisplayPath24getFreeJoinablePathCountEv",getFreeJoinablePathCount},
			//{"__ZN24AppleIntelBaseController20RegisterAGDCCallbackEP25AGDCRegisterLinkControl_t",hasExternalDispla},
			
			//{"__ZN21AppleIntelFramebuffer15connectionProbeEv",connectionProbe, this->oconnectionProbe},
			
			//{"__ZN21AppleIntelFramebuffer14disableDisplayEb",dovoid},
			
			/*{"__ZN14AppleIntelPort15checkLinkStatusEv",handleLinkIntegrityCheck},
			{"__ZN17AppleIntelPortHAL10enablePortEv",enablePorth, this->oenablePorth},
			{"__ZN14AppleIntelPort12linkTrainingEP18AGDCDPPortConfig_t",linkTraining, this->olinkTraining},
			{"__ZN17AppleIntelPortHAL29setVoltageSwingAndPreEmphasisEjjP15PortDriveConfig",setVoltageSwingAndPreEmphasis,	this->osetVoltageSwingAndPreEmphasis},
			{"__ZN21AppleIntelDisplayPath22setLinkTrainingPatternE19LinkTrainingPattern",setLinkTrainingPattern, this->osetLinkTrainingPattern},
			{"__ZN21AppleIntelDisplayPath18getFirstPathByPortEP14AppleIntelPort",getFirstPathByPort, this->ogetFirstPathByPort},
			{"__ZN14AppleIntelPort8writeAUXEjPvj",writeAUX, this->owriteAUX},
			{"__ZN14AppleIntelPort7readAUXEjPvj",readAUX, this->oreadAUX},*/
			{"__ZN14AppleIntelPort8isHPDLowEv",isHPDLow, this->oisHPDLow},
			{"__ZL13SafeForceWakebj", SafeForceWake2, this->oSafeForceWake2},
			
			//{"__ZN14AppleIntelPort11getDPCDInfoEv",getDPCDInfo, this->ogetDPCDInfo},
			
			//{"__ZN15AppleIntelPlane11enablePlaneEb",enablePlane, this->oenablePlane},
			
			//{"__ZN24AppleIntelBaseController22SetFbStatusOnNextProbeEP14AGDCFBOnline_t",SetFbStatusOnNextProbe, this->oSetFbStatusOnNextProbe},

			//{"__ZN31AppleIntelRegisterAccessManager14ReadRegister32EPVvm",raReadRegister32b},
			
			//{"__ZN31AppleIntelRegisterAccessManager14ReadRegister32Em",raReadRegister32, this->oraReadRegister32},
			//{"__ZN31AppleIntelRegisterAccessManager15WriteRegister32Emj",raWriteRegister32, this->oraWriteRegister32},
			
			//{"__ZN31AppleIntelRegisterAccessManager15WriteRegister32EPVvmj",raWriteRegister32b},
			
			//{"__ZN21AppleIntelFramebuffer18prepareToEnterWakeEv",prepareToEnterWake,this->oprepareToEnterWake},
			/*{"__ZN21AppleIntelFramebuffer17prepareToExitWakeEv",dovoid},
			{"__ZN21AppleIntelFramebuffer18prepareToExitSleepEv",dovoid},
			{"__ZN21AppleIntelFramebuffer19prepareToEnterSleepEv",dovoid},
			{"__ZN21AppleIntelFramebuffer18prepareToEnterWakeEv",dovoid},*/
			
		};
		PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route dp symbols");
		
		if (isprod) {
			RouteRequestPlus requests[] = {
				{"__ZN31AppleIntelFramebufferController18setAsyncSliceCountE13IGSliceConfig", setAsyncSliceCount2, this->osetAsyncSliceCount2},
				//{"__ZN31AppleIntelFramebufferController14disableDisplayEP21AppleIntelFramebufferP21AppleIntelDisplayPathh",dovoid},
				{"__ZN31AppleIntelFramebufferController18hasExternalDisplayEv",hasExternalDispla},// external wrong detection
				//{"__ZN31AppleIntelFramebufferController9hwSetModeEP21AppleIntelFramebufferP21AppleIntelDisplayPathPK29IODetailedTimingInformationV2",hwSetMode, this->ohwSetMode},
				//{"__ZN31AppleIntelFramebufferController16hwRegsNeedUpdateEP21AppleIntelFramebufferP21AppleIntelDisplayPathP10CRTCParamsPK29IODetailedTimingInformationV2PN16AppleIntelScaler12SCALERPARAMSE",hwRegsNeedUpdate, this->ohwRegsNeedUpdate},
				
				{"__ZN21AppleIntelFramebuffer4initEP31AppleIntelFramebufferControllerj",AppleIntelFramebufferinit, this->oAppleIntelFramebufferinit},
				//{"__ZN31AppleIntelFramebufferController10hwShutdownEP21AppleIntelFramebuffer",handleLinkIntegrityCheck},
				{"__ZN31AppleIntelFramebufferController18hwInitializeCStateEv",hwInitializeCState, this->ohwInitializeCState},
				//{"__ZN31AppleIntelFramebufferController20hwConfigureCustomAUXEb",hwConfigureCustomAUX, this->ohwConfigureCustomAUX},
				{"__ZN31AppleIntelFramebufferController13FBMemMgr_InitEv", FBMemMgr_Init,this->oFBMemMgr_Init},
				{"__ZN31AppleIntelFramebufferController23initPlatformWorkaroundsEv",initPlatformWorkarounds, this->oinitPlatformWorkarounds},
				{"__ZN31AppleIntelFramebufferController16getOSInformationEv",getOSInformation, this->ogetOSInformation},
				{"__ZN19AppleIntelPowerWell4initEP31AppleIntelFramebufferController",AppleIntelPowerWellinit, this->oAppleIntelPowerWellinit},
				//{"__ZN31AppleIntelFramebufferController21probeCDClockFrequencyEv",wrapProbeCDClockFrequency,	this->orgProbeCDClockFrequency},
				{"__ZN31AppleIntelFramebufferController11initCDClockEv",initCDClock, this->oinitCDClock},
				/*{"__ZN31AppleIntelFramebufferController28setCDClockFrequencyOnHotplugEv",setCDClockFrequencyOnHotplug, this->osetCDClockFrequencyOnHotplug},
				{"__ZN31AppleIntelFramebufferController14disableCDClockEv",disableCDClock,this->odisableCDClock},*/
				
				{"__ZN31AppleIntelFramebufferController16disableVDDForAuxEv",disableVDDForAux, this->odisableVDDForAux},
				{"__ZN31AppleIntelFramebufferController15enableVDDForAuxEP14AppleIntelPort",enableVDDForAux, this->oenableVDDForAux},
				
			};
			PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route p symbols");
			
		} else //debug version
		{
			RouteRequestPlus requests[] = {
				{"__ZN24AppleIntelBaseController18setAsyncSliceCountE13IGSliceConfig", setAsyncSliceCount2, this->osetAsyncSliceCount2},
				//{"__ZN24AppleIntelBaseController14disableDisplayEP21AppleIntelFramebufferP21AppleIntelDisplayPathh",dovoid},
				{"__ZN24AppleIntelBaseController18hasExternalDisplayEv",hasExternalDispla},// external wrong detection
				//{"__ZN24AppleIntelBaseController9hwSetModeEP21AppleIntelFramebufferP21AppleIntelDisplayPathPK29IODetailedTimingInformationV2",hwSetMode, this->ohwSetMode},
				//{"__ZN24AppleIntelBaseController16hwRegsNeedUpdateEP21AppleIntelFramebufferP21AppleIntelDisplayPathP10CRTCParamsPK29IODetailedTimingInformationV2PN16AppleIntelScaler12SCALERPARAMSE",hwRegsNeedUpdate, this->ohwRegsNeedUpdate},
				
				{"__ZN21AppleIntelFramebuffer4initEP24AppleIntelBaseControllerj",AppleIntelFramebufferinit, this->oAppleIntelFramebufferinit},
				//{"__ZN24AppleIntelBaseController10hwShutdownEP21AppleIntelFramebuffer",handleLinkIntegrityCheck},
				{"__ZN24AppleIntelBaseController18hwInitializeCStateEv",hwInitializeCState, this->ohwInitializeCState},
				//{"__ZN24AppleIntelBaseController20hwConfigureCustomAUXEb",hwConfigureCustomAUX, this->ohwConfigureCustomAUX},
				{"__ZN24AppleIntelBaseController13FBMemMgr_InitEv", FBMemMgr_Init,this->oFBMemMgr_Init},
				{"__ZN24AppleIntelBaseController23initPlatformWorkaroundsEv",initPlatformWorkarounds, this->oinitPlatformWorkarounds},
				{"__ZN24AppleIntelBaseController16getOSInformationEv",getOSInformation, this->ogetOSInformation},
				{"__ZN19AppleIntelPowerWell4initEP24AppleIntelBaseController",AppleIntelPowerWellinit, this->oAppleIntelPowerWellinit},
				//{"__ZN24AppleIntelBaseController21probeCDClockFrequencyEv",wrapProbeCDClockFrequency,	this->orgProbeCDClockFrequency},
				{"__ZN24AppleIntelBaseController11initCDClockEv",initCDClock, this->oinitCDClock},
				/*{"__ZN24AppleIntelBaseController28setCDClockFrequencyOnHotplugEv",setCDClockFrequencyOnHotplug, this->osetCDClockFrequencyOnHotplug},
				{"__ZN24AppleIntelBaseController14disableCDClockEv",disableCDClock,this->odisableCDClock},*/
				
			//{"__ZN24AppleIntelBaseController15hwSetPanelPowerEj",setPanelPowerState, this->osetPanelPowerState},
				
				{"__ZN24AppleIntelBaseController16disableVDDForAuxEv",disableVDDForAux, this->odisableVDDForAux},
				{"__ZN24AppleIntelBaseController15enableVDDForAuxEP14AppleIntelPort",enableVDDForAux, this->oenableVDDForAux},
				
				
				//{"__ZN24AppleIntelBaseController14isPanelPowerOnEv",isPanelPowerOn, this->oisPanelPowerOn},
				
			};
			PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route d symbols");
			
		}
		
		
	
		
		//CamelliaTcon2
		static const uint8_t f1b[] = {0x11, 0x0a, 0x84, 0x41};
		static const uint8_t r1b[] = {0x11, 0x0a, 0x82, 0x41};
		
		//BanksiaTcon3
		static const uint8_t f1c[] = {0x12, 0x14, 0xc4, 0x41};
		static const uint8_t r1c[] = {0x11, 0x0a, 0x82, 0x41};
		
		static const uint8_t f1p[]= {0xe8, 0x66, 0xb0, 0xfe, 0xff, 0x89, 0x45, 0xc8, 0x3d, 0xff, 0xff, 0x00, 0x00, 0x74, 0x45};
		static const uint8_t r1p[]= {0xe8, 0x66, 0xb0, 0xfe, 0xff, 0x89, 0x45, 0xc8, 0x3d, 0xff, 0xff, 0x00, 0x00, 0xeb, 0x45};
		
		
		

		//panel tim
		static const uint8_t f6[]= {0x41, 0xbc, 0xc4, 0x09, 0xe4, 0x02, 0xbb, 0x34, 0x08, 0x7d, 0x00, 0x41, 0xbf, 0x60, 0x00, 0x00, 0x00};
		static const uint8_t r6[]= {0x41, 0xbc, 0x01, 0x00, 0xf4, 0x01, 0xbb, 0x01, 0x00, 0xd0, 0x07, 0x41, 0xbf, 0x60, 0x00, 0x00, 0x00};
//edp
		static const uint8_t f6a[]= {0x74, 0x2a, 0x83, 0xf8, 0x01, 0x74, 0x43, 0x85, 0xc0, 0x75, 0x60};
		static const uint8_t r6a[]= {0x90, 0x90, 0x83, 0xf8, 0x01, 0x90, 0x90, 0x85, 0xc0, 0x90, 0x90};

		static const uint8_t f6ap[]= {0x74, 0x2a, 0x83, 0xf8, 0x01, 0x74, 0x43, 0x85, 0xc0, 0x75, 0x60};
		static const uint8_t r6ap[]= {0x90, 0x90, 0x83, 0xf8, 0x01, 0x90, 0x90, 0x85, 0xc0, 0x90, 0x90};
		
		//ReadRegister64
		static const uint8_t f7[]= {0x83, 0xc0, 0xfc, 0x48, 0x39, 0xf0, 0x76, 0x11, 0x48, 0x8b, 0x47, 0x50, 0x48, 0xff, 0x05, 0xca, 0xf5, 0x0c, 0x00};
		static const uint8_t r7[]= {0x83, 0xc0, 0xf8, 0x48, 0x39, 0xf0, 0x76, 0x11, 0x48, 0x8b, 0x47, 0x50, 0x48, 0xff, 0x05, 0xca, 0xf5, 0x0c, 0x00};
		
		static const uint8_t f7p[]= {0x83, 0xc0, 0xfc, 0x48, 0x39, 0xf0, 0x76, 0x11, 0x48, 0x8b, 0x47, 0x50, 0x48, 0xff, 0x05, 0x84, 0x40, 0x08, 0x00};
		static const uint8_t r7p[]= {0x83, 0xc0, 0xf8, 0x48, 0x39, 0xf0, 0x76, 0x11, 0x48, 0x8b, 0x47, 0x50, 0x48, 0xff, 0x05, 0x84, 0x40, 0x08, 0x00};


		
		
		//probeportmode
		static const uint8_t f13[]= {0xff, 0x91, 0x90, 0x01, 0x00, 0x00, 0x83, 0xf8, 0x02, 0x0f, 0x84, 0xec, 0x00, 0x00, 0x00};
		static const uint8_t r13[]= {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
		
		static const uint8_t f13b[]= {0xff, 0x90, 0x90, 0x01, 0x00, 0x00, 0x49, 0x8b, 0x0e, 0x4c, 0x89, 0xf7, 0x89, 0xc6, 0xff, 0x91, 0x38, 0x01, 0x00, 0x00};
		static const uint8_t r13b[]= {0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x49, 0x8b, 0x0e, 0x4c, 0x89, 0xf7, 0x89, 0xc6, 0xff, 0x91, 0x38, 0x01, 0x00, 0x00};

		//probeportmode
		static const uint8_t f13p[]= {0xff, 0x91, 0x78, 0x01, 0x00, 0x00, 0x83, 0xf8, 0x02, 0x74, 0x64};
		static const uint8_t r13p[]= {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
		
		static const uint8_t f13pb[]= {0xff, 0x90, 0x78, 0x01, 0x00, 0x00, 0x49, 0x8b, 0x0e, 0x4c, 0x89, 0xf7, 0x89, 0xc6, 0xff, 0x91, 0x38, 0x01, 0x00, 0x00};
		static const uint8_t r13pb[]= {0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x49, 0x8b, 0x0e, 0x4c, 0x89, 0xf7, 0x89, 0xc6, 0xff, 0x91, 0x38, 0x01, 0x00, 0x00};

		
		//getPathByPipe logs
		static const uint8_t f15[]= {0x74, 0x36, 0x48, 0xff, 0x05, 0x7e, 0x51, 0x08, 0x00, 0x44, 0x89, 0x3c, 0x24, 0x48, 0x8d, 0x15, 0x4d, 0x88, 0x03, 0x00, 0x4c, 0x8d, 0x05, 0x28, 0x8a, 0x03, 0x00};
		static const uint8_t r15[]= {0xeb, 0x36, 0x48, 0xff, 0x05, 0x7e, 0x51, 0x08, 0x00, 0x44, 0x89, 0x3c, 0x24, 0x48, 0x8d, 0x15, 0x4d, 0x88, 0x03, 0x00, 0x4c, 0x8d, 0x05, 0x28, 0x8a, 0x03, 0x00};
		

		//getHPDState
		static const uint8_t f19[]= {0xbe, 0x70, 0x44, 0x04, 0x00};
		static const uint8_t r19[]= {0xbe, 0xa0, 0x38, 0x16, 0x00};
		
		//savenvram
		static const uint8_t f20[]= {0xff, 0x90, 0xf8, 0x09, 0x00, 0x00, 0x41, 0x89, 0xc6, 0x48, 0x85, 0xdb, 0x74, 0x17};
		static const uint8_t r20[]= {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x48, 0x85, 0xdb, 0x74, 0x17};
		
		static const uint8_t f20p[]= {0xff, 0x90, 0xf8, 0x09, 0x00, 0x00, 0x41, 0x89, 0xc6, 0x48, 0x85, 0xdb, 0x74, 0x17};
		static const uint8_t r20p[]= {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x48, 0x85, 0xdb, 0x74, 0x17};

		
		//regff
		static const uint8_t f24bp[]= {0x83, 0x78, 0x08, 0x00, 0x75, 0x0c};
		static const uint8_t r24bp[]= {0x83, 0x78, 0x08, 0x00, 0xeb, 0x0c};
		
		static const uint8_t f24cp[]= {0x00, 0x4c, 0x89, 0xea, 0x75, 0x12};
		static const uint8_t r24cp[]= {0x00, 0x4c, 0x89, 0xea, 0xeb, 0x12};
		
		static const uint8_t f24dp[]= {0x83, 0x78, 0x08, 0x00, 0x75, 0x0d};
		static const uint8_t r24dp[]= {0x83, 0x78, 0x08, 0x00, 0xeb, 0x0d};
		
		//regff
		static const uint8_t f24b[]= {0x83, 0x78, 0x08, 0x00, 0x75, 0x0c};
		static const uint8_t r24b[]= {0x83, 0x78, 0x08, 0x00, 0xeb, 0x0c};
		
		static const uint8_t f24c[]= {0x48, 0x8b, 0x55, 0xd0, 0x75, 0x13};
		static const uint8_t r24c[]= {0x48, 0x8b, 0x55, 0xd0, 0xeb, 0x13};
		
		static const uint8_t f24d[]= {0x83, 0x78, 0x08, 0x00, 0x75, 0x0d};
		static const uint8_t r24d[]= {0x83, 0x78, 0x08, 0x00, 0xeb, 0x0d};
		
		//link
		static const uint8_t f25[]= {0x77, 0x77, 0x00, 0x00};
		static const uint8_t r25[]= {0x33, 0x00, 0x00, 0x00};

		//slice
		static const uint8_t f26[]= {0x49, 0x8b, 0x47, 0x28, 0x48, 0x83, 0xf8, 0x03, 0x0f, 0x84, 0x33, 0x01, 0x00, 0x00};
		static const uint8_t r26[]= {0x49, 0x8b, 0x47, 0x28, 0x48, 0x83, 0xf8, 0x03, 0x48, 0xe9, 0xdb, 0x03, 0x00, 0x00};
		
		static const uint8_t f26p[]= {0x49, 0x8b, 0x47, 0x28, 0x48, 0x83, 0xf8, 0x03, 0x0f, 0x84, 0xee, 0x00, 0x00, 0x00};
		static const uint8_t r26p[]= {0x49, 0x8b, 0x47, 0x28, 0x48, 0x83, 0xf8, 0x03, 0x48, 0xe9, 0x32, 0x03, 0x00, 0x00};
		


		
		if (isprod){
			LookupPatchPlus const patchesp[] = {// tgl production kext

				//{&kextG11FBT, f1p, r1p, arrsize(f1p),	1},
				//{&kextG11FBT, f2dp, r2dp, arrsize(f2dp),	1},
				//{&kextG11FBT, f3, r3, arrsize(f3),	1},
				/*{&kextG11FBT, f4, r4, arrsize(f4),	11},
				{&kextG11FBT, f4a, r4a, arrsize(f4a),	11},
				{&kextG11FBT, f4b, r4b, arrsize(f4b),	2},
				{&kextG11FBT, f4c, r4c, arrsize(f4c),	2},*/
				
				{&kextG11FBT, f6ap, r6ap, arrsize(f6ap),	1},
				{&kextG11FBT, f7p, r7p, arrsize(f7p),	1},
				//{&kextG11FBT, f10p, r10p, arrsize(f10p),	1},
				{&kextG11FBT, f13p, r13p, arrsize(f13p),	1},
				{&kextG11FBT, f13pb, r13pb, arrsize(f13pb),	1},
				//{&kextG11FBT, f16p, r16p, arrsize(f16p),	1},
				{&kextG11FBT, f19, r19, arrsize(f19),	1},
				{&kextG11FBT, f20p, r20p, arrsize(f20p),	1},
				
				{&kextG11FBT, f24bp, r24bp, arrsize(f24bp),	14},
				{&kextG11FBT, f24cp, r24cp, arrsize(f24cp),	1},
				{&kextG11FBT, f24dp, r24dp, arrsize(f24dp),	4},
				
				
				{&kextG11FBT, f25, r25, arrsize(f25),	6},
				
				{&kextG11FBT, f26p, r26p, arrsize(f26),	1},


			};
			
			PANIC_COND(!LookupPatchPlus::applyAll(patcher, patchesp , address, size), "nblue", "kextG11FBT Failed to apply production patches!");
		}
		else {
			LookupPatchPlus const patches[] = {// tgl debug kext
				//{&kextG11FBT, f1, r1, arrsize(f1),	1},
				//{&kextG11FBT, f1a, r1a, arrsize(f1a),	1},
				
				//{&kextG11FBT, f1b, r1b, arrsize(f1b),	4},
				//{&kextG11FBT, f1c, r1c, arrsize(f1c),	1},
				
				//{&kextG11FBT, f2, r2, arrsize(f2),	1},
				//{&kextG11FBT, f2b, r2b, arrsize(f2b),	1},
				// {&kextG11FBT, f2c, r2c, arrsize(f2c),	1},
				// {&kextG11FBT, f2d, r2d, arrsize(f2d),	1},

				//{&kextG11FBT, f3, r3, arrsize(f3),	1},
				
				/*{&kextG11FBT, f4, r4, arrsize(f4),	12},
				{&kextG11FBT, f4a, r4a, arrsize(f4a),	11},
				{&kextG11FBT, f4b, r4b, arrsize(f4b),	2},
				{&kextG11FBT, f4c, r4c, arrsize(f4c),	2},*/
				
			/*	{&kextG11FBT, f5, r5, arrsize(f5),	1},
				{&kextG11FBT, f5b, r5b, arrsize(f5b),	1},
				{&kextG11FBT, f5c, r5c, arrsize(f5c),	1},*/
				
				//{&kextG11FBT, f6, r6, arrsize(f6),	1},
				{&kextG11FBT, f6a, r6a, arrsize(f6a),	1},
				
				
				{&kextG11FBT, f7, r7, arrsize(f7),	1},
				
				{&kextG11FBT, f19, r19, arrsize(f19),	1},
				{&kextG11FBT, f20, r20, arrsize(f20),	1},
				
				{&kextG11FBT, f13, r13, arrsize(f13),	1},
				{&kextG11FBT, f13b, r13b, arrsize(f13b),	1},
				{&kextG11FBT, f15, r15, arrsize(f15),	1},

				{&kextG11FBT, f24b, r24b, arrsize(f24b),	11},
				{&kextG11FBT, f24c, r24c, arrsize(f24c),	1},
				{&kextG11FBT, f24d, r24d, arrsize(f24d),	6},
				
				{&kextG11FBT, f25, r25, arrsize(f25),	6},
				
				{&kextG11FBT, f26, r26, arrsize(f26),	1},


			};
			
			PANIC_COND(!LookupPatchPlus::applyAll(patcher, patches , address, size), "nblue", "kextG11FBT Failed to apply dbg patches!");
		}
			

		return true;
		
		
	}     else if (kextG11HW.loadIndex == index) {
		NBlue::callback->setRMMIOIfNecessary();

		/*SolveRequestPlus solveRequests[] = {
			
		};
		SYSLOG_COND(!SolveRequestPlus::solveAll(patcher, index, solveRequests, address, size), "nblue",	"Failed to resolve symbols");
		*/
		 
		 RouteRequestPlus requests[] = {
			 {"__ZN16IntelAccelerator19PAVPCommandCallbackE22PAVPSessionCommandID_tjPjb", wrapPavpSessionCallback, this->orgPavpSessionCallback},
		 };
		SYSLOG_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route symbols");
		
		//sku bypass IntelAccelerator::getGPUInfo
		static const uint8_t f2[] = {
			0x0F, 0x87, 0x17, 0x01, 0x00, 0x00, 0x48, 0x8D, 0x0D, 0x96, 0x02, 0x00, 0x00
		};
		static const uint8_t r2[] = {
			0xe9, 0x8b, 0x00, 0x00, 0x00, 0x90, 0x48, 0x8D, 0x0D, 0x96, 0x02, 0x00, 0x00
		};
    
		LookupPatchPlus const patches[] = {
			{&kextG11HW, f2, r2, arrsize(f2),	1},
		};
		

		SYSLOG_COND(!LookupPatchPlus::applyAll(patcher, patches , address, size), "nblue", "kextG11HW Failed to apply patches!");

		return true;
		
		
    } else if (kextG11HWT.loadIndex == index || kextG11HWTe.loadIndex == index) {
		NBlue::callback->setRMMIOIfNecessary();
		auto kext=kextG11HWT.loadIndex == index ? kextG11HWT: kextG11HWTe;
		
		SolveRequestPlus solveRequests[] = {
			
			{"__ZN23IGHardwareBlit2DContext17ExtendedCtxParamsE", this->Blit2DExtendedCtxParams},
			{"__ZN23IGHardwareBlit3DContext17ExtendedCtxParamsE", this->Blit3DExtendedCtxParams},
			{"__ZN24IGHardwareResolveContext17ExtendedCtxParamsE", this->ExtendedCtxParams},
			{"__ZL11kIGHwCsDesc", this->kIGHwCsDesc},
			{"_g_timestampFreqPerDenominator", this->g_timestampFreqPerDenominator},
			
		};
		//PANIC_COND(!SolveRequestPlus::solveAll(patcher, index, solveRequests, address, size), "nblue",	"Failed to resolve symbols");
		 
		//todo: check gen8_gmch_probe(ggtt);
		
		 RouteRequestPlus requests[] = {
			 
			 
			 {"__ZN16IntelAccelerator19PAVPCommandCallbackE22PAVPSessionCommandID_tjPjb", wrapPavpSessionCallback, this->orgPavpSessionCallback},
			 
			 {"__ZN16IntelAccelerator10getGPUInfoEv", getGPUInfo, this->ogetGPUInfo},
			 {"__ZN16IntelAccelerator16initHardwareCapsEv", initHardwareCaps, this->oinitHardwareCaps},
			 {"__ZN16IntelAccelerator13SafeForceWakeEbj", SafeForceWake, this->oSafeForceWake},
			 
			// {"__ZN17IGInterruptBridge25readAndClearCCSInterruptsER8IGBitSetILm46EEj", dovoid},
			 {"__ZN17IGInterruptBridge25readAndClearGuCInterruptsER8IGBitSetILm46EEj", dovoid},
			 
			 //{"__ZN26IGHardwareCommandStreamer519setWorkQueueBindingEib", dovoid},
			 //{"__ZN26IGHardwareCommandStreamer530contextSwitchInterruptOccurredEv", dovoid},
			 
			 {"__ZN16IntelAccelerator18setAsyncSliceCountE13IGSliceConfig", setAsyncSliceCount, this->osetAsyncSliceCount},
			 //{"__ZN13IGHardwareGuC15releaseDoorbellEP35UK_GEN11_GUC_CONTEXT_DESCRIPTOR_REC", dovoid},
			 
			// {"__ZN18IGAccelDisplayPipe20displayModeDidChangeEv", displayModeDidChange, this->odisplayModeDidChange},
			// {"__ZN18IGAccelDisplayPipe16probeDisplayPipeEv", probeDisplayPipe, this->oprobeDisplayPipe},
			 
			 
			 //{"__ZN13IGHardwareGuC13loadGuCBinaryEv", loadGuCBinary, this->oloadGuCBinary},
			 //{"__ZN13IGHardwareGuC18checkWOPCMSettingsEmR14IOVirtualRange", checkWOPCMSettings, this->ocheckWOPCMSettings},
			
			/* {"__ZN11IGAccelTask16getBlit2DContextEb", getBlit2DContext, this->ogetBlit2DContext},
			 
			 {"__ZN11IGAccelTask16getBlit3DContextEb", getBlit3DContext, this->ogetBlit3DContext},
			 {"__ZN23IGHardwareBlit3DContextnwEm", IGHardwareBlit3DContextoperatornew, this->oIGHardwareBlit3DContextoperatornew},
			 {"__ZN25IGHardwareExtendedContext15initWithOptionsEP11IGAccelTaskRK31IGHardwareExtendedContextParams",IGHardwareExtendedContextinitWithOptions, this->oIGHardwareExtendedContextinitWithOptions},*/
			 
			// {"__ZN20IGHardwareRingBuffer19resetGraphicsEngineEP17IGHardwareContext", resetGraphicsEngine, this->oresetGraphicsEngine},
			// {"__ZN12IGScheduler56resumeEv", IGScheduler5resume, this->oIGScheduler5resume},
			 
			// {"__ZN26IGAccelSegmentResourceList13initBlitUsageEv", initBlitUsage, this->oinitBlitUsage},
			// {"__ZN26IGAccelSegmentResourceList13markBlitUsageEv", markBlitUsage, this->omarkBlitUsage},
			 
			//{"__ZN19IGAccelCommandQueue21beginCoalescedSegmentEv", dovoid},
			 //{"__ZN26IGAccelSegmentResourceList7prepareEv", IGAccelSegmentResourceListprepare, this->oIGAccelSegmentResourceListprepare},
			 
			/* {"__Z31blit3d_initialize_scratch_spaceP16IGAccelSysMemory", blit3d_initialize_scratch_space, this->oblit3d_initialize_scratch_space},
			 {"__Z15blit3d_init_ctxP23IGHardwareBlit3DContext", blit3d_init_ctx, this->oblit3d_init_ctx},
			 {"__ZN23IGHardwareBlit3DContext10initializeEv", IGHardwareBlit3DContextinitialize, this->oIGHardwareBlit3DContextinitialize},
			 {"__ZNK14IGMappedBuffer9getMemoryEv", IGMappedBuffergetMemory, this->oIGMappedBuffergetMemory},*/
			 
			//{"__Z16blit3d_supportedPK15blit3d_params_tRK8IGVectorI11rect_pair_t25IGIOMallocAllocatorPolicyE", blit3d_supported, this->oblit3d_supported},
			 
			 
		 };
		PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route symbols");
		
		//sku
		static const uint8_t f3[] = {
			0x8b, 0x3e, 0x81, 0xff, 0xee, 0xbe, 0xaf, 0xde, 0x7f, 0x15, 0x81, 0xff, 0x86, 0x80, 0x40, 0x9a, 0x74, 0x2d
		};
		static const uint8_t r3[] = {
			0x8b, 0x3e, 0x81, 0xff, 0xee, 0xbe, 0xaf, 0xde, 0x90, 0x90, 0x81, 0xff, 0x86, 0x80, 0x40, 0x9a, 0xeb, 0x2d
		};
		
		static const uint8_t f3a[] = {
			0x83, 0xfe, 0x01, 0x75, 0x59, 0x83, 0xfa, 0x0c
		};
		static const uint8_t r3a[] = {
			0x83, 0xfe, 0x01, 0x75, 0x59, 0x83, 0xfa, 0x0a
		};
		


		
		//pavp
		static const uint8_t f6[] = {
			0xb9, 0x06, 0x00, 0x00, 0x00, 0x21, 0xc8, 0x39, 0xc8, 0x0f, 0x94, 0x05, 0x52, 0x1f, 0x12, 0x00
		};
		static const uint8_t r6[] = {
			0xb9, 0x06, 0x00, 0x00, 0x00, 0x21, 0xc8, 0x39, 0xc8, 0xC6, 0x05, 0x52, 0x1f, 0x12, 0x00, 0x00
		};
		
		//great patch
		static const uint8_t f5[] = {0x40, 0xd2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		static const uint8_t r5[] = {0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		
		//guc
		static const uint8_t f7a[] = {0x74, 0x6f, 0x83, 0xfa, 0x60, 0x74, 0x6a, 0x4c, 0x89, 0xff, 0x31, 0xf6, 0xba};
		static const uint8_t r7a[] = {0x74, 0x3f, 0x83, 0xfa, 0x60, 0x74, 0x3a, 0x4c, 0x89, 0xff, 0x31, 0xf6, 0xba};
		
		static const uint8_t f7b[] = {0xe0, 0xff, 0xc3, 0x74, 0x3d};
		static const uint8_t r7b[] = {0xe0, 0xff, 0xc3, 0x74, 0x0d};
		
			LookupPatchPlus const patches[] = {
				{&kext, f3, r3, arrsize(f3),	1},
				{&kext, f3a, r3a, arrsize(f3a),	1},
				{&kext, f5, r5, arrsize(f5),	1},
				//{&kext, f7a, r7a, arrsize(f7a),	1},
				//{&kext, f7b, r7b, arrsize(f7b),	1},
			};
			PANIC_COND(!LookupPatchPlus::applyAll(patcher, patches , address, size), "nblue", "kextG11HWT Failed to apply patches!");

		
		DBGLOG("nblue", "Loaded AppleIntelTGLGraphics!");

		return true;
	}

    return false;
}



void  Gen11::setAsyncSliceCount(void *that,uint32_t configRaw)
{
	uint8_t sliceCount     = (configRaw >> 0) & 0xFF;
	uint8_t euCount  = (configRaw >> 8) & 0xFF;
	uint8_t subsliceCount        = (configRaw >> 0x10) & 0xFF;

	uint uVar1;
	
	uVar1 = 1;
	if ((char)configRaw != '\x01') {
	  uVar1 = (uint)((char)configRaw == '\x02') * 2;
	}
	switch((char)(configRaw >> 0x10)) {
	case '\x01':
	  uVar1 = uVar1 | 0x100;
	  break;
	case '\x02':
	  uVar1 = uVar1 | 0x200;
	  break;
	case '\x03':
	  uVar1 = uVar1 | 0x300;
	  break;
	case '\x04':
	uVar1 = uVar1 | 0x400; break;
	case '\x05': uVar1 = uVar1 | 0x500; break;
		default: uVar1 = uVar1 | 0x500;
		  break;
	}
	switch((char)(configRaw >> 8)) {
	case '\x02':
	  uVar1 = uVar1 | 0x20;
	  break;
		default: uVar1 = uVar1 | 0xA0;
	  break;
	case '\x04':
	  uVar1 = uVar1 | 0x40;
	  break;
	case '\x06':
	  uVar1 = uVar1 | 0x60;
	  break;
	case '\b':
		uVar1 = uVar1 | 0x80;  break;
	case '\n':   uVar1 = uVar1 | 0xA0; break;
	}
	
	getMember<uint32_t>(that, 0x12a0) = configRaw;
	
	SafeForceWake(that,true,4);
	NBlue::callback->writeReg32(0xa204,uVar1);
	SafeForceWake(that,false,4);
	
}

void  Gen11::setAsyncSliceCount2(void *that,uint32_t configRaw)
{
		uint8_t sliceCount     = (configRaw >> 0) & 0xFF;
		uint8_t euCount  = (configRaw >> 8) & 0xFF;
		uint8_t subsliceCount        = (configRaw >> 0x10) & 0xFF;

	uint uVar1;

	uVar1 = 1;
	if ((char)configRaw != '\x01') {
	  uVar1 = (uint)((char)configRaw == '\x02') * 2;
	}
	switch((char)(configRaw >> 0x10)) {
	case '\x01':
	  uVar1 = uVar1 | 0x100;
	  break;
	case '\x02':
	  uVar1 = uVar1 | 0x200;
	  break;
	case '\x03':
	  uVar1 = uVar1 | 0x300;
	  break;
	case '\x04':
	uVar1 = uVar1 | 0x400; break;
	case '\x05': uVar1 = uVar1 | 0x500; break;
		default: uVar1 = uVar1 | 0x500;
		  break;
	}
	switch((char)(configRaw >> 8)) {
	case '\x02':
	  uVar1 = uVar1 | 0x20;
	  break;
		default: uVar1 = uVar1 | 0xA0;
	  break;
	case '\x04':
	  uVar1 = uVar1 | 0x40;
	  break;
	case '\x06':
	  uVar1 = uVar1 | 0x60;
	  break;
	case '\b':
		uVar1 = uVar1 | 0x80;  break;
	case '\n':   uVar1 = uVar1 | 0xA0; break;
	}
	
	//getMember<uint32_t>(that, 0x12a0) = configRaw;
	
	SafeForceWake2(that,true,4);
	NBlue::callback->writeReg32(0xa204,uVar1);
	SafeForceWake2(that,false,4);
	
}

void  Gen11::initPlatformWorkarounds(void *that)
{
	//PlatformWorkarounds
	getMember<volatile uint32_t>(that, 0xc5c)=
	FB_FLAG_ALTERNATE_PWM_INCREMENT1|FB_FLAG_ENABLE_SLICE_FEATURES|
	FB_FLAG_FORCE_POWER_ALWAYS_CONNECTED|FB_FLAG_AVOID_FAST_LINK_TRAINING| FB_FLAG_ENABLE_BACKLIGHT_REG_CONTROL/*|FB_FLAG_LIMIT_4K_SOURCE_SIZE*/;
	
	//ig boot flags
	getMember<volatile uint32_t>(that, 0xc58)=/*FB_FLAG_ENABLE_BACKLIGHT_REG_CONTROL|*/FB_FLAG_BOOST_PIXEL_FREQUENCY_LIMIT;
	
}
uint64_t  Gen11::getOSInformation(void *that)
{
	
	struct PlatformInfo *pinfo =static_cast<PlatformInfo *>(callback->gPlatformInformationList);
	
	
	pinfo[1].fInfoFlags=
	FB_FLAG_DISABLE_PIPE_SCRAMBLE|FB_FLAG_FRAMEBUFFER_COMPRESSION|FB_FLAG_ALLOW_CONNECTOR_RECOVER
	/*|FB_FLAG_ENABLE_BACKLIGHT_REG_CONTROL*/|FB_FLAG_FORCE_POWER_ALWAYS_CONNECTED|FB_FLAG_AVOID_FAST_LINK_TRAINING
	/*|FB_FLAG_USE_VIDEO_TURBO|FB_FLAG_ALTERNATE_PWM_INCREMENT2*/;
	
	
		pinfo[1].cameliav=3;
		//CamelliaTcon2=2 BanksiaTcon=3
	
	
		pinfo[1].fMobile=1;
		pinfo[1].fPipeCount=3;
		pinfo[1].fInfoPortCount=3;
		pinfo[1].fInfoFramebufferCount=2;

		pinfo[1].fSliceCount=1;
		pinfo[1].fmaxEuCount=8;
		pinfo[1].fsubslices=5;
	
	//pinfo[1].fInfoFBCompressionMemorySize=	0xc00000;
	pinfo[1].fVideoTurboFreq=270000000;
	pinfo[1].VCLK=1000*0x438;
	
	
	pinfo[1].connectors[0].index=0;//DDI0
	pinfo[1].connectors[0].busId=0;
	pinfo[1].connectors[0].pipe=0;//fix wrong register use patch or set = 1
	pinfo[1].connectors[0].pad=0;
	pinfo[1].connectors[0].type=ConnectorLVDS;
	pinfo[1].connectors[0].flags=0x8+0x400;
	
	pinfo[1].connectors[1].index=1;
	pinfo[1].connectors[1].busId=1;
	pinfo[1].connectors[1].pipe=2;
	pinfo[1].connectors[1].pad=0;
	pinfo[1].connectors[1].type=ConnectorHDMI;
	pinfo[1].connectors[1].flags=0x1+0x800;
	
	pinfo[1].connectors[2].index=2;
	pinfo[1].connectors[2].busId=2;
	pinfo[1].connectors[2].pipe=2;
	pinfo[1].connectors[2].pad=0;
	pinfo[1].connectors[2].type=ConnectorDummy;
	pinfo[1].connectors[2].flags=0;
	
	pinfo[1].connectors[3].index=3;
	pinfo[1].connectors[3].busId=3;
	pinfo[1].connectors[3].pipe=3;
	pinfo[1].connectors[3].pad=0;
	pinfo[1].connectors[3].type=ConnectorDummy;
	pinfo[1].connectors[3].flags=0;
	
	auto ret=FunctionCast(getOSInformation, callback->ogetOSInformation)(that );
	return ret;
}

	static const uint8_t DAT_000b0bb0[] = {
		0x00, 0x36, 0x6e, 0x01, 0x00, 0xf8, 0x24, 0x01,
		0x00, 0xf0, 0x49, 0x02, 0x40, 0x78, 0x7d, 0x01
	};


void Gen11::getGPUInfo(void *this_ptr) {
	//getMember<uint32_t>(this_ptr, 0x1120)=1;
	FunctionCast(getGPUInfo, callback->ogetGPUInfo)(this_ptr);
	getMember<uint32_t>(this_ptr, 0xddc) = 5;
	getMember<uint32_t>(this_ptr, 0x1158) =5;
	return;
	
	
		struct IGHwCsDesc *kIGHwCsDesc=(struct IGHwCsDesc*)callback->kIGHwCsDesc;
	
		// 1. Read MMIO Base
	uint64_t mmioBase = getMember<uint64_t>(this_ptr, 0x1240);
		
		// 2. Wake GPU
		//SafeForceWake(this_ptr, true, 5);
	
		// 3. Read Fuses & Registers
		uint32_t sliceFuseReg     = *(uint32_t *)(mmioBase+ 0x9138);
		uint32_t dssFuseReg       = *(uint32_t *)(mmioBase+ 0x913c);
		uint32_t euDisableFuseReg = *(uint32_t *)(mmioBase+ 0x9134);
		uint32_t mediaFuseReg     = *(uint32_t *)(mmioBase+ 0x9140);
		uint32_t freqStatusReg    = *(uint32_t *)(mmioBase+ 0xd00);

		// 4. Sleep GPU
		//SafeForceWake(this_ptr, false, 5);

	//getMember<uint32_t>(this_ptr, 0x1120)=1;
		// --- Log SKU ---
		uint32_t gpuSku = getMember<uint32_t>(this_ptr, 0x1120);

		// --- Process Media Fuses ---
		uint32_t calculatedMediaMask = ~mediaFuseReg & 0x000f00ff;

		// --- Filter Command Streamers ---
		uint64_t engineMask = getMember<uint64_t>(this_ptr, 0x1300);
		/*if (engineMask != 0) {
		uint64_t tempMask = engineMask;
		while (tempMask != 0) {
			uint32_t idx = 0;
			while (((tempMask >> idx) & 1ULL) == 0) {
				idx++;
			}

			if (idx >= 3 && idx < 6) {
				if ((kIGHwCsDesc[idx].fuseMask & calculatedMediaMask) == 0) {
					uint8_t type = kIGHwCsDesc[idx].type & 0x3f;
					uint64_t &maskRef = getMember<uint64_t>(this_ptr, 0x1300 + (type >> 6) * 8);
					maskRef &= ~((1ULL << type) | (0xFFFFFFFFFFFFFFFEULL >> (0x40 - type)));
				}
			}

			if (idx > 4) break;
			
			// Clear the current bit to find the next one
			tempMask &= ~(1ULL << idx);
		}
		}*/
	
		// --- Calculate Topology: Slices ---
		getMember<uint32_t>(this_ptr, 0x115c) = 0;
		int numSlices = 0;
		for (int i = 0; i < 8; i++) {
			numSlices += (sliceFuseReg >> i) & 1;
		}
	
	numSlices=1;
		getMember<uint32_t>(this_ptr, 0x115c) = numSlices;

		// --- Calculate Topology: SubSlices ---
	int numSubSlices = POPCOUNT(dssFuseReg) * 2;
	
	numSubSlices=10;
	
		getMember<uint32_t>(this_ptr, 0x1158) = numSubSlices;

		// --- Calculate Topology: EUs ---
	uint32_t disabledEUs = POPCOUNT(euDisableFuseReg & 0xFF);
		int maxEUPerSubSlice = 8 - disabledEUs;
	
	maxEUPerSubSlice=8;
	
		int totalEUs = numSubSlices * maxEUPerSubSlice;

		getMember<uint32_t>(this_ptr, 0x116c) = maxEUPerSubSlice;
		getMember<uint32_t>(this_ptr, 0x1124) = totalEUs;


		// --- Frequency Defaults ---
		getMember<uint64_t>(this_ptr, 0x1150) = 0x1c2000003e8ULL; // Min: 450, Max: 1000

		// --- SKU Checks (Modified for 5 DSS) ---
		uint64_t capabilities = getMember<uint64_t>(this_ptr, 0x1190);
		
		if (numSlices > 1) {
			capabilities |= 0x8;
		} else {
			capabilities &= ~0x8;
		}
		getMember<uint64_t>(this_ptr, 0x1190) = capabilities;

		getMember<uint32_t>(this_ptr, 0xdd8) = numSlices;
		getMember<uint32_t>(this_ptr, 0xddc) = numSubSlices;

		if (gpuSku == 1) {
			// TGLHP
			getMember<uint32_t>(this_ptr, 0x1164) = 8; // L3 Bank Count

			// --- MODIFIED SECTION START ---
			if ((numSlices != 1) || (numSubSlices != 0xA)) {

			}
			// --- MODIFIED SECTION END ---
		}
		else {
			if (gpuSku != 2) {
				return;
			}
			
			// TGLLP
			//if (numSlices == 1 && numSubSlices == 0xC) {
				getMember<uint32_t>(this_ptr, 0x1164) = 8;
			/*}
			else*/ if (numSlices == 1 && ((capabilities & 0x20) != 0 && numSubSlices == 4)) {
				getMember<uint32_t>(this_ptr, 0x1164) = 4;
			}
			else {

			}
		}

		// --- Timestamp Frequency Calculation ---
		uint32_t freqIndex = (freqStatusReg >> 3) & 7;
		uint32_t rawFreq = 24000000; // Default 24MHz
		
		if (freqIndex < 4) {
			// Use getMember to access the static table as if it were a struct
			rawFreq = getMember<uint32_t>((void*)DAT_000b0bb0, freqIndex * 4);
		}
		
		// Divider logic:
		// Reg bits 00 -> /8, 01 -> /4, 10 -> /2, 11 -> /1
		// Logic: (~Reg >> 1) & 3
		uint32_t divider = 1 << ((~(freqStatusReg >> 1)) & 3);
		uint32_t finalFreq = rawFreq / divider;
		
		// Global timestamp scaling
		*(uint64_t*)callback->g_timestampFreqPerDenominator = (finalFreq >> 5) / 0xc35;
		getMember<uint32_t>(this_ptr, 0x1188) = finalFreq;


		initHardwareCaps(this_ptr);
	//FunctionCast(initHardwareCaps, callback->oinitHardwareCaps)(this_ptr);
}


bool Gen11::initHardwareCaps(void *this_ptr) {
	getMember<uint32_t>(this_ptr, 0x1158) =5;
	return FunctionCast(initHardwareCaps, callback->oinitHardwareCaps)(this_ptr);;
	
/*
		bool success = false;
		uint32_t activeUnitCount;
		uint32_t unitSize;
		uint32_t unitMultiplier;
		uint32_t maxUnitCount;

		if (m_sku == 2) {
			// --- TGL GT2 Low Power (LP) Constants ---
			m_L3Config_Reg0 = 0xC0;
			m_L3Config_Reg1 = 0x222;
			m_L3Config_Reg2 = 0x150;
			m_L3Config_Reg3 = 0x222;
			m_L3Config_Reg4 = 0x150;
			m_L3Alloc_MaxIndex = 7;
			m_L3Alloc_Stride = 2;
			m_L3Alloc_BaseSize = 0x80;
			m_L3Alloc_Granularity = 0x10;
			m_L3BankMask = 0xF00;

			// CHANGE: Dynamic Max Unit Count
			// Standard is 6 (12 SS). If we have 0xA (10 SS), max is 5 (Dual Subslices).
			if (m_numSubSlices == 0xA) {
				maxUnitCount = 5;
			} else if (m_numSubSlices == 4) {
				maxUnitCount = 2;
			} else {
				maxUnitCount = 6; // Default to 6 for 0xC (12 SS)
			}
			
			m_field0x1148 = maxUnitCount; // Store max units

			// Calculate active units (Dual Subslices)
			activeUnitCount = m_numSubSlices >> 1; // 0xA >> 1 = 5
			
			unitMultiplier = 2;
			unitSize = 0x80;
		}
		else if (m_sku == 1) {
			// HP Logic (unchanged)
			// ...
			return true; // Skipping HP details for brevity
		}
		else {
			IOLog("[IGPU] HWCAPS: invalid Gfx SKU: %d\n", m_sku);
			return false;
		}

		// --- Derived Calculations ---

		m_activeUnitCount = activeUnitCount; // 5
		m_hwFlags &= ~0x4;

		// Partition A = Active * 2
		// For 0xA: 5 * 2 = 10
		m_partitionA_Size = unitMultiplier * activeUnitCount;

		// Partition B = (Max - Active) * 2
		// For 0xA: (5 - 5) * 2 = 0 (Optimized: No reserved space for missing unit)
		// Old Logic: (6 - 5) * 2 = 2 (Reserved space for dead unit)
		m_partitionB_Size = (maxUnitCount - activeUnitCount) * unitMultiplier;

		// Total URB = Active * 0x80
		// For 0xA: 5 * 0x80 = 0x280 (640 KB)
		m_totalURBAllocation = unitSize * activeUnitCount;

		m_sliceConfig = m_numSlices << 4;

		success = true;
		return success;
	*/
	

}

void Gen11::checkWOPCMSettings(void *that,unsigned long param_1,void *param_2)
{
	const uint32_t GUC_WOPCM_OFFSET = 1 * 1024 * 1024;
	const uint32_t GUC_WOPCM_SIZE = 1 * 1024 * 1024;
	
	typedef struct {
		uint64_t address;
		uint64_t length;
	} IOVirtualRange;
	
	IOVirtualRange *wopcm_range = (IOVirtualRange *)param_2;
		
	wopcm_range->address = GUC_WOPCM_OFFSET;
	wopcm_range->length = GUC_WOPCM_SIZE;
	
}

void *ccont;
void *ccont2;

IOReturn Gen11::wrapPavpSessionCallback( void *intelAccelerator, int32_t sessionCommand, uint32_t sessionAppId, uint32_t *a4, bool flag) {
	
	//getMember<void *>(intelAccelerator, 0x90) = ccont;

	//void* pPavpContext = *getMember<void**>(intelAccelerator, 0x1278);
	//void* pStampTrackingStruct = *(void**)getMember<char*>(pPavpContext, 0xb8);
	
	if (sessionCommand == 4) {
		return kIOReturnTimeout;
	}

	return FunctionCast(wrapPavpSessionCallback, callback->orgPavpSessionCallback)(intelAccelerator, sessionCommand, sessionAppId, a4, flag);
}

IOReturn Gen11::wrapFBClientDoAttribute(void *fbclient, uint32_t attribute, unsigned long *unk1, unsigned long unk2, unsigned long *unk3, unsigned long *unk4,  void *externalMethodArguments) {
	if (attribute == 0x923) {
		return kIOReturnUnsupported;
	}
	
	return FunctionCast(wrapFBClientDoAttribute, callback->orgFBClientDoAttribute)(fbclient, attribute, unk1, unk2, unk3, unk4,  externalMethodArguments);
}


UInt8 Gen11::wrapLoadGuCBinary(void *that) {

	if (callback->firmwareSizePointer)
		callback->performingFirmwareLoad = true;

	auto r = FunctionCast(wrapLoadGuCBinary, callback->orgLoadGuCBinary)(that);
	DBGLOG("nblue", "loadGuCBinary returned %d", r);

	callback->performingFirmwareLoad = false;

	return r;
}

void  Gen11::displayModeDidChange(void *that)
{
	probeDisplayPipe(that);
  //  FunctionCast(displayModeDidChange, callback->odisplayModeDidChange)(that);
}

bool  Gen11::IsThisCamellia2(void *that)
{
    FunctionCast(IsThisCamellia2, callback->oIsThisCamellia2)(that);
	return true;
}

uint64_t  Gen11::probeDisplayPipe(void *that)
{
   return FunctionCast(probeDisplayPipe, callback->oprobeDisplayPipe)(that);
}

uint64_t Gen11::setupPort(void *that)
{
	getMember<uint32_t>(that, 0x116) = 1;
   return FunctionCast(setupPort, callback->osetupPort)(that);
}

bool Gen11::wrapLoadFirmware(void *that) {

	//(*reinterpret_cast<uintptr_t **>(that))[35] = reinterpret_cast<uintptr_t>(wrapSystemWillSleep);
	//(*reinterpret_cast<uintptr_t **>(that))[36] = reinterpret_cast<uintptr_t>(wrapSystemDidWake);
	return FunctionCast(wrapLoadFirmware, callback->orgLoadFirmware)(that);
}

uint32_t Gen11::DisplaySleepCallback(void *that,uint32_t param_1,uint param_2,uint param_3)
{


   return FunctionCast(DisplaySleepCallback, callback->oDisplaySleepCallback)(that,param_1,param_2,param_3);
}

void Gen11::wrapSystemWillSleep(void *that) {
	DBGLOG("nblue", "systemWillSleep GuC callback");
}

void Gen11::wrapSystemDidWake(void *that) {
	DBGLOG("nblue", "systemDidWake GuC callback");

	// This is IGHardwareGuC class instance.
	auto &GuC = (reinterpret_cast<OSObject **>(that))[76];

	if (GuC)
	if (GuC->metaCast("IGHardwareGuC")) {
		DBGLOG("igfx", "reloading firmware on wake; discovered IGHardwareGuC - releasing");
		GuC->release();
		GuC = nullptr;
	}

	FunctionCast(wrapLoadFirmware, callback->orgLoadFirmware)(that);
}

bool Gen11::wrapInitSchedControl(void *that) {
	DBGLOG("nblue", "attempting to init sched control with load %d", callback->performingFirmwareLoad);
	bool perfLoad = callback->performingFirmwareLoad;
	callback->performingFirmwareLoad = false;
	bool r = FunctionCast(wrapInitSchedControl, callback->orgInitSchedControl)(that);

	callback->performingFirmwareLoad = perfLoad;
	return r;
}

void *Gen11::wrapIgBufferWithOptions(void *accelTask, void* size, unsigned int type, unsigned int flags) {
	void *r = nullptr;

	if (callback->performingFirmwareLoad) {
		callback->dummyFirmwareBuffer = Buffer::create<uint8_t>(*(unsigned long*)size);

		const void *fw = nullptr;
		const void *fwsig = nullptr;
		size_t fwsize = 0;
		size_t fwsigsize = 0;


		/*fw = GuCFirmwareKBL;
		fwsig = GuCFirmwareKBLSignature;
		fwsize = GuCFirmwareKBLSize;
		fwsigsize = GuCFirmwareSignatureSize;*/

		unsigned long newsize = fwsize > *(unsigned long*)size ? ((fwsize + 0xFFFF) & (~0xFFFF)) : *(unsigned long*)size;
		r = FunctionCast(wrapIgBufferWithOptions, callback->orgIgBufferWithOptions)(accelTask, (void*)newsize,type,flags);
		if (r && callback->dummyFirmwareBuffer) {
			auto status = MachInfo::setKernelWriting(true, KernelPatcher::kernelWriteLock);
			if (status == KERN_SUCCESS) {
				callback->realFirmwareBuffer = static_cast<uint8_t **>(r)[7];
				static_cast<uint8_t **>(r)[7] = callback->dummyFirmwareBuffer;
				lilu_os_memcpy(callback->realFirmwareBuffer, fw, fwsize);
				lilu_os_memcpy(callback->signaturePointer, fwsig, fwsigsize);
				callback->realBinarySize = static_cast<uint32_t>(fwsize);
				*callback->firmwareSizePointer = static_cast<uint32_t>(fwsize);
				MachInfo::setKernelWriting(false, KernelPatcher::kernelWriteLock);
			} else {
				//SYSLOG("igfx", "ig buffer protection upgrade failure %d", status);
			}
		} else if (callback->dummyFirmwareBuffer) {
			//SYSLOG("igfx", "ig shared buffer allocation failure");
			Buffer::deleter(callback->dummyFirmwareBuffer);
			callback->dummyFirmwareBuffer = nullptr;
		} else {
			//SYSLOG("igfx", "dummy buffer allocation failure");
		}
	} else {
		r = FunctionCast(wrapIgBufferWithOptions, callback->orgIgBufferWithOptions)(accelTask, size,type,flags);
	}

	return r;
}

UInt64 Gen11::wrapIgBufferGetGpuVirtualAddress(void *that) {
	if (callback->performingFirmwareLoad && callback->realFirmwareBuffer) {
		static_cast<uint8_t **>(that)[7] = callback->realFirmwareBuffer;
		callback->realFirmwareBuffer = nullptr;
		Buffer::deleter(callback->dummyFirmwareBuffer);
		callback->dummyFirmwareBuffer = nullptr;
	}

	return FunctionCast(wrapIgBufferGetGpuVirtualAddress, callback->orgIgBufferGetGpuVirtualAddress)(that);
}


uint32_t Gen11::wrapReadRegister32(void *controller, uint32_t address) {
	uint32_t retVal = FunctionCast(wrapReadRegister32, callback->owrapReadRegister32)(controller,address);

	return retVal;
}

void Gen11::wrapWriteRegister32(void *controller, uint32_t address, uint32_t value) {

	FunctionCast(wrapWriteRegister32, callback->owrapWriteRegister32)(controller,address,value);

}


int hwu=0;

int Gen11::LightUpEDP(void *that,void *param_1, void *param_2,void *param_3)
{
	FunctionCast(LightUpEDP, callback->oLightUpEDP)(that,param_1,param_2,param_3);
	return 0;
};

unsigned long Gen11::hwRegsNeedUpdate
		  (void *that,void *param_1,
		   void *param_2,void *param_3,void *param_4,
		   void *param_5)
{
	
	auto ret=FunctionCast(hwRegsNeedUpdate, callback->ohwRegsNeedUpdate)(that,param_1,param_2,param_3,param_4,param_5 );

	if (hwu){
		hwu=0;
		return 1;
	}
	return ret;
}

uint64_t  Gen11::linkTraining(void *that,void *param_1)
{
	//fastLinkTraining(that);
	//return 0;
	/*auto ret= FunctionCast(linkTraining, callback->olinkTraining)(that,param_1);
	return 0;
	return ret;*/
	



	void *path=getFirstPathByPort(that);
	if (!path) {
	  //  _IntelLog(1, 9, __FILE__, 0x1F6, "linkTraining", "Port is not associated to any path. Returning Error\n");
		return 0xe00002bc;
	}

	// Fast Link Training
	/*if (!(_gController_osinfo->paramflags & 0x02)) {
		if (this->agdclinkset == 0 && this->auxhandsk == 0) {
			if (fastLinkTraining(this) == 0) {
			   // if (_IntelLogEnabled(8, 9)) _IntelLog(8, 9, __FILE__, 0, "linkTraining", "Fast Link Training Successful\n");
				return 0;
			}
			// Fallback logic for TCON/Camellia reset check...
		}
	}*/


	
	
	PortConfig *pPortConfig=getMember<PortConfig*>(that, 0x548);
	uint8_t ASR=getMember<uint8_t>(that, 0x118);
	uint8_t Downspread=getMember<uint8_t>(that, 0x119);
	uint32_t CR=getMember<uint32_t>(that, 0x12c);
	uint32_t EQ=getMember<uint32_t>(that, 0x130);
	uint32_t NumberOfLanes=getMember<uint32_t>(that, 0x124);
	uint8_t linktype=getMember<uint8_t>(that, 0x11b);
	uint8_t enableMST=getMember<uint8_t>(that, 0x117);
	uint32_t BitRate=getMember<uint32_t>(that, 0x128);
	uint8_t EnhancedFraming=getMember<uint8_t>(that, 0x11a);
	uint8_t agdclinkset=getMember<uint8_t>(that, 0x116);
	uint8_t auxhandsk=getMember<uint8_t>(that, 0x14);
	

	
	uint8_t linkConfig[2];
	linkConfig[0]=BitRate;
	linkConfig[1]=(EnhancedFraming << 7) | NumberOfLanes;
	
	writeAUX(that, 0x100, linkConfig, 2);
	
	if (pPortConfig->pipe == 0) {
		writeAUX(that, 0x10A, &ASR, 1);
	}
	
	//if (that->FEConSink) configureFEConSink(that, 1);
	//if (that->DSConSink) configureDSConSink(that, 1);

	uint8_t downspread = Downspread << 4;
	writeAUX(that, 0x107, &downspread, 1);

	uint16_t laneMask = *(uint16_t *)((uint8_t*)&gLaneStatusMasks + ((NumberOfLanes - 1) * 2));
	laneMask = 0x0011;
	
	setVoltageSwingAndPreEmphasis(that,0, 0, 0);
	
	setLinkTrainingPattern(path, 1);
	enablePorth(that);
	
	uint8_t pattern = 1; // TPS1
	writeAUX(that, 0x102, &pattern, 1);
	

	// Call HAL to set initial electrical
	//((void (*)(AppleIntelPort*, uint32_t, uint32_t, void*))this->vt[0x68])(this, 0, 0, 0);
	

	// -----------------------------------------------------
	// Clock Recovery Loop
	// -----------------------------------------------------
	uint8_t voltageSwing = 0, preEmphasis = 0;
	int retries = 0;
	bool crSuccess = false;
	uint8_t request01, request23;
	uint8_t newVS, newPE;

	do {
		// Set Electrical
	   // ((void (*)(AppleIntelPort*, uint32_t, uint32_t, void*))this->vt[0x68])(this, voltageSwing, preEmphasis, 0);
		setVoltageSwingAndPreEmphasis(that,voltageSwing, preEmphasis, 0);
		
		uint8_t trainingSet[4];
		
		uint8_t maxVSReached = (voltageSwing == 3) ? 1 : 0;
			uint8_t maxPEReached = (preEmphasis == 3) ? 1 : 0;

			// Construct the Training Lane Set byte
			// Bits [1:0]: Voltage Swing
			// Bits [3:2]: Pre-Emphasis
			// Bit [5]:   Max Pre-Emphasis Reached
			// Bit [6]:   Max Voltage Swing Reached
			uint8_t laneVal = voltageSwing | (preEmphasis << 2) | (maxPEReached << 5) | (maxVSReached << 6);
		
		//uint8_t laneVal = voltageSwing | (preEmphasis << 2);
		trainingSet[0] = laneVal;
		trainingSet[1] = laneVal;
		trainingSet[2] = laneVal;
		trainingSet[3] = laneVal;
		
		writeAUX(that, 0x103, trainingSet, 4);
		
		if (CR >= 1000) IOSleep(CR / 1000);
		IODelay(CR % 1000);

		uint16_t status[3];
		readAUX(that, 0x202, status, 6);


		if ((status[0] & laneMask) == laneMask) {
			crSuccess = true;
		   // if (_IntelLogEnabled(8, 9)) _IntelLog(8, 9, __FILE__, 0, "linkTraining", "Clock recovery complete\n");
			break;
		}

		// Adjust Request Parsing
		uint8_t adj[2];
		readAUX(that, 0x206, adj, 2);
		request01 = adj[0];
		request23 = adj[1];



		// Decode Lane 0
		newVS = request01 & 0x03;
		newPE = (request01 >> 2) & 0x03;

		// Decode Lane 1
		uint8_t tVS = (request01 >> 4) & 0x03;
		uint8_t tPE = (request01 >> 6) & 0x03;
		if (tVS > newVS) newVS = tVS;
		if (tPE > newPE) newPE = tPE;

		if (NumberOfLanes > 1) {
			// Decode Lane 2
			tVS = request23 & 0x03;
			tPE = (request23 >> 2) & 0x03;
			if (tVS > newVS) newVS = tVS;
			if (tPE > newPE) newPE = tPE;

			if (NumberOfLanes > 2) {
				// Decode Lane 3
				tVS = (request23 >> 4) & 0x03;
				tPE = (request23 >> 6) & 0x03;
				if (tVS > newVS) newVS = tVS;
				if (tPE > newPE) newPE = tPE;
			}
		}

		if (voltageSwing == newVS && preEmphasis == newPE) {
			retries++;
		} else {
			retries = 0;
		}

		if (retries > 5) {
			crSuccess = false;
			break;
		}

		// Apply DP Spec Rules
		if (newVS > voltageSwing) {
			newPE = 0;
		}
		voltageSwing = newVS;
		preEmphasis = newPE;

	} while (true);

	if (!crSuccess) {
	   // _IntelLog(1, 9, __FILE__, 0, "linkTraining", "Phase 1 of link training failed\n");
		return 0xe00002bc;
	}

	// -----------------------------------------------------
	// Channel Equalization Loop
	// -----------------------------------------------------
	/*if (that->tps4) pattern = 4;
	else if (that->tps3) pattern = 3;
	else */pattern = 2;
	
	writeAUX(that, 0x102, &pattern, 1);
	setLinkTrainingPattern(path, pattern);

	retries = 0;
	bool eqSuccess = false;
	uint16_t eqMask = laneMask & 0x0077; // CR(1) + SymLock(2) + EQ(4)

	do {
	   // ((void (*)(AppleIntelPort*, uint32_t, uint32_t, void*))this->vt[0x68])(this, voltageSwing, preEmphasis, 0);
		setVoltageSwingAndPreEmphasis(that,voltageSwing, preEmphasis, 0);
		uint8_t trainingSet[4];
		
		uint8_t maxVSReached = (voltageSwing == 3) ? 1 : 0;
			uint8_t maxPEReached = (preEmphasis == 3) ? 1 : 0;

			// Construct the Training Lane Set byte
			// Bits [1:0]: Voltage Swing
			// Bits [3:2]: Pre-Emphasis
			// Bit [5]:   Max Pre-Emphasis Reached
			// Bit [6]:   Max Voltage Swing Reached
			uint8_t laneVal = voltageSwing | (preEmphasis << 2) | (maxPEReached << 5) | (maxVSReached << 6);
		
	    //uint8_t laneVal = voltageSwing | (preEmphasis << 2);
		trainingSet[0] = laneVal;
		trainingSet[1] = laneVal;
		trainingSet[2] = laneVal;
		trainingSet[3] = laneVal;

		writeAUX(that, 0x103, trainingSet, 4);
		
		if (EQ >= 1000) IOSleep(EQ / 1000);
		IODelay(EQ % 1000);

		uint16_t status[3];
		readAUX(that, 0x202, status, 6);



		if ((status[0] & eqMask) == eqMask) {
			eqSuccess = true;
		  //  if (_IntelLogEnabled(8, 9)) _IntelLog(8, 9, __FILE__, 0, "linkTraining", "Link Training successful\n");
			break;
		}

		// Adjust Request Logic (Same as CR)
		uint8_t adj[2];
		readAUX(that, 0x206, adj, 2);
		request01 = adj[0];
		request23 = adj[1];

		newVS = request01 & 0x03;
		newPE = (request01 >> 2) & 0x03;
		
		uint8_t tVS = (request01 >> 4) & 0x03;
		uint8_t tPE = (request01 >> 6) & 0x03;
		if (tVS > newVS) newVS = tVS;
		if (tPE > newPE) newPE = tPE;

		if (NumberOfLanes > 1) {
			tVS = request23 & 0x03;
			tPE = (request23 >> 2) & 0x03;
			if (tVS > newVS) newVS = tVS;
			if (tPE > newPE) newPE = tPE;

			if (NumberOfLanes > 2) {
				tVS = (request23 >> 4) & 0x03;
				tPE = (request23 >> 6) & 0x03;
				if (tVS > newVS) newVS = tVS;
				if (tPE > newPE) newPE = tPE;
			}
		}

		if (voltageSwing == newVS && preEmphasis == newPE) {
			retries++;
		} else {
			retries = 0;
		}

		if (retries > 6) {
			eqSuccess = false;
			break;
		}

		if (newVS > voltageSwing) newPE = 0;
		voltageSwing = newVS;
		preEmphasis = newPE;

	} while (true);

	if (!eqSuccess) {
	   // _IntelLog(1, 9, __FILE__, 0, "linkTraining", "Phase 2 of link training failed\n");
		return 0xe00002bc;
	}

	// -----------------------------------------------------
	// Success
	// -----------------------------------------------------
	pattern = 0;
	writeAUX(that, 0x102, &pattern, 1);
	setLinkTrainingPattern(path, 0);
	IOSleep(0x11);
	
	if (linktype == 0 || enableMST != 0) {
		setLinkTrainingPattern(path, 5);
	}
	
	//if (that->FEConSink)path->enableFEC(path);

	if (param_1) {
		
		getMember<uint32_t>(param_1, 0x38)= 0;
		getMember<uint16_t>(param_1, 0x8)= 0;
		getMember<uint32_t>(param_1, 0x1c)= 0;
		
		getMember<uint8_t>(param_1, 0xa)= (uint8_t)BitRate;
		getMember<uint8_t>(param_1, 0x13)= (uint8_t)NumberOfLanes;
		getMember<uint8_t>(param_1, 0x1b)= EnhancedFraming;
		getMember<uint8_t>(param_1, 0x17)= ASR;
		getMember<uint8_t>(param_1, 0x16)= Downspread;
		getMember<uint8_t>(param_1, 0x14)= voltageSwing;
		getMember<uint8_t>(param_1, 0x15)= preEmphasis;
		getMember<uint8_t>(param_1, 0x18)= (uint8_t)BitRate;
		getMember<uint8_t>(param_1, 0x19)= (uint8_t)NumberOfLanes;
	}
	
	return 0;

training_failed:
	pattern = 0;
	writeAUX(that, 0x102, &pattern, 1);
	setLinkTrainingPattern(path, 0);
	IOSleep(0x11);
	if (linktype == 0 || enableMST != 0) {
		setLinkTrainingPattern(path, 5);
	}
	
	//if (!agdcConfig && that->field_0x11e == 0) {
	   // AppleIntelBaseController::CallBackAGDC(_gController, 0xB, this->PortIndex, 0);
	//}
	return 0xe00002bc;
	
	
};

void Gen11::SafeForceWake2(void *that,bool param_1,uint param_2)
{
	
	FunctionCast(SafeForceWake2, callback->oSafeForceWake2)(that,param_1,param_2);
};

void Gen11::SafeForceWake(void *that,bool param_1,uint param_2)
{
	
	FunctionCast(SafeForceWake, callback->oSafeForceWake)(that,param_1,param_2);
};

unsigned long  Gen11::isHPDLow(void *that)
{
	
	auto ret= FunctionCast(isHPDLow, callback->oisHPDLow)(that);
	return ret;
};

uint64_t Gen11::getLinkConfig(void *that,IOFBDPLinkConfig *param_1)
{
	
	auto ret= FunctionCast(getLinkConfig, callback->ogetLinkConfig)(that,param_1);
	return ret;
};

int  Gen11::enablePorth(void *that)
{
	auto ret= FunctionCast(enablePorth, callback->oenablePorth)(that);
	return ret;
};
unsigned long Gen11::fastLinkTraining(void *that)
{
	FunctionCast(fastLinkTraining, callback->ofastLinkTraining)(that);
	return 0;
};

uint64_t Gen11::setVoltageSwingAndPreEmphasis(void *that,uint32_t voltageSwing, uint32_t preEmphasis, PortDriveConfig *customDriveConfig)
{
	return FunctionCast(setVoltageSwingAndPreEmphasis, callback->osetVoltageSwingAndPreEmphasis)(that,voltageSwing,preEmphasis,customDriveConfig);
	
	void *port = (void *)that;
	
	// Use the structure type, not a raw uint32_t*
	TGL_ComboPhy_Registers regs;
	
	regs.reg_phy_ctl_0 = 0x605f0;
	regs.reg_phy_ctl_1 = 0x640f4;
	regs.reg_dpll_stat_0 = 0x640f8;
	regs.reg_dpll_stat_1 = 0x640fc;
	regs.reg_phy_misc_0 = 0x64000;
	regs.reg_phy_misc_1 = 0x64010;
	regs.reg_phy_misc_2_0 = 0x64014;
	regs.reg_phy_misc_2_1 = 0x64018;
	regs.reg_phy_misc_3_0 = 0x6401c;
	regs.reg_phy_misc_3_1 = 0x64020;
	regs.reg_misc_4_0 = 0x64024;
	regs.reg_misc_4_1 = 0x64c00;
	regs.reg_port_ctl_0 = 0x162100;
	regs.reg_port_ctl_1 = 0x162104;
	regs.reg_voltage_info = 0x16210c;
	regs.reg_voltage_info_2 = 0x162124;
	regs.reg_comp_ctl_0 = 0x162128;
	regs.reg_comp_ctl_1 = 0x162014;
	regs.reg_ddi_buf_ctl_0 = 0x162028;
	regs.reg_ddi_buf_ctl_1 = 0x162030;
	regs.reg_bias_ctl_0 = 0x16203c;
	regs.reg_bias_ctl_1 = 0x162688;
	regs.reg_pcs_ctl_0 = 0x162888;
	regs.reg_pcs_ctl_1 = 0x162988;
	regs.reg_tx_dwell_0 = 0x162a88;
	regs.reg_tx_dwell_1 = 0x162b88;
	regs.reg_pat_ctl_0 = 0x162388;
	regs.reg_pat_ctl_1 = 0x162690;
	regs.reg_tx_lane_0 = 0x162890;
	regs.reg_tx_lane_1 = 0x162990;
	regs.reg_tx_lane_2 = 0x162a90;
	regs.reg_tx_lane_3 = 0x162b90;
	regs.reg_loadgen_0 = 0x162390;
	regs.reg_loadgen_1 = 0x162694;
	regs.reg_common_ctl_0 = 0x162894;
	regs.reg_common_ctl_1 = 0x162994;
	regs.reg_parity_0 = 0x162a94;
	regs.reg_parity_1 = 0x162b94;
	regs.reg_idle_ctl_0 = 0x162394;
	regs.reg_idle_ctl_1 = 0x162604;
	regs.reg_aux_0 = 0x162804;
	regs.reg_aux_1 = 0x162904;
	regs.reg_ln0_trans_0 = 0x162a04;
	regs.reg_ln0_trans_1 = 0x162b04;
	regs.reg_ln1_trans_0 = 0x162304;
	regs.reg_ln1_trans_1 = 0x162624;
	regs.reg_ln2_trans_0 = 0x162824;
	regs.reg_ln2_trans_1 = 0x162924;
	regs.reg_ln3_trans_0 = 0x162b24;
	regs.reg_ln3_trans_1 = 0x162b24;
	regs.reg_misc_phy_0 = 0x162324;
	
	//if (this->portmode == 2) return 0; // TBT
	

	PortDriveConfig *driveConfig = customDriveConfig;
	int32_t driveTableIndex = 0;

	if (1/*driveConfig == NULL*/) {
		// Read from struct member instead of regs[14]
		//uint32_t voltageInfo = port->readRegister32(regs->reg_voltage_info);
		//uint32_t voltageLevel = (voltageInfo >> 0x18) & 3;

		/*osinfo *platformInfo = (osinfo *)_gController_osinfo;
		osinfov *voltageTable = platformInfo->voltages;

		if (voltageLevel == 2) voltageTable += 6;
		else if (voltageLevel == 1) voltageTable += 3;
		else if (voltageLevel != 0) {
			//_IntelLog(1, 0x11, __FILE__, 0, "setVoltageSwingAndPreEmphasis", "Invalid voltage\n");
		}

		if ((port->pPortConfig->flags & 0x10) == 0) {
			if (port->pPortConfig->pipe == 1) voltageTable += 1; // DP
			else voltageTable += 2; // HDMI
		}*/

		//if (port->pPortConfig->pad == 0) {
			//if ((port->pPortConfig->flags & 0x20000) == 0 && port->pPortConfig->pipe != 2) {
				// if (voltageTable->pad == 0) {
					 driveTableIndex = fLinkTrainingDriveTable[voltageSwing + preEmphasis * 4] & 0xF;
					// goto driveIndexFound;
				// }
			//}
			//driveTableIndex = voltageTable->pad;
		/*} else {
			driveTableIndex = AppleIntelPort::fLinkTrainingDriveTable[voltageSwing + preEmphasis * 4] & 0xF;
		}*/

driveIndexFound:
		if (driveTableIndex > 9) driveTableIndex = 0;
		//if (port->pPortConfig->pad != 0) {
			// driveConfig = (PortDriveConfig *)(&fVoltageSwingTableForDklPhy[driveTableIndex * 3]);
	//	} else {
			//driveConfig = (PortDriveConfig *)(voltageTable->voltages + driveTableIndex * 7);
		driveConfig =(PortDriveConfig *)&gDPVcc0_85V[driveTableIndex];
		//}
	}

	if (1/*port->pPortConfig->pad == 0*/) {
			
		uint32_t valLane0 = driveConfig[0].Voltage;
			uint32_t valLane1 = driveConfig[1].Voltage;
			uint32_t valLane2 = driveConfig[2].Voltage;
			uint32_t valLane3 = driveConfig[3].Voltage;

		
		NBlue::callback->readReg32(regs.reg_tx_lane_0);
		NBlue::callback->readReg32(regs.reg_tx_lane_1);
		//NBlue::callback->readReg32(regs.reg_tx_lane_2);
		//NBlue::callback->readReg32(regs.reg_tx_lane_3);
		
		uint32_t compensationValue = *(uint32_t*)((uint8_t*)&driveConfig[1].value + 1);

		uint32_t laneCount = 2;//(this->setupok) ? this->NumberOfLanes : this->maxlanecount;

			if (laneCount == 1) {
			}
			else if (laneCount == 2) {
				uint32_t currentReg1 = NBlue::callback->readReg32(regs.reg_tx_lane_1);
				valLane1 = (currentReg1 & 0x3FFFF) | compensationValue;
			}
			else if (laneCount == 4) {
				uint32_t currentReg1 = NBlue::callback->readReg32(regs.reg_tx_lane_1);
				uint32_t currentReg2 = NBlue::callback->readReg32(regs.reg_tx_lane_2);
				
				valLane1 = (currentReg1 & 0x3FFFF) | compensationValue;
				valLane2 = (currentReg2 & 0x3FFFF) | compensationValue;
			}

		NBlue::callback->writeReg32(regs.reg_tx_lane_0, valLane0);
		NBlue::callback->writeReg32(regs.reg_tx_lane_1, valLane1);
		//NBlue::callback->writeReg32(regs.reg_tx_lane_2, valLane2);
		//NBlue::callback->writeReg32(regs.reg_tx_lane_3, valLane3);
		
		NBlue::callback->writeReg32(regs.reg_comp_ctl_1, NBlue::callback->readReg32(regs.reg_comp_ctl_1));
		
		NBlue::callback->writeReg32(regs.reg_loadgen_1, NBlue::callback->readReg32(regs.reg_common_ctl_0));
		NBlue::callback->writeReg32(regs.reg_loadgen_1, NBlue::callback->readReg32(regs.reg_common_ctl_0));

		NBlue::callback->writeReg32(regs.reg_idle_ctl_1, NBlue::callback->readReg32(regs.reg_idle_ctl_0));

		NBlue::callback->writeReg32(regs.reg_bias_ctl_1, NBlue::callback->readReg32(regs.reg_pcs_ctl_0));

		uint32_t ddiCtl = NBlue::callback->readReg32(regs.reg_ddi_buf_ctl_0);
		
		ddiCtl = ddiCtl & ~0xF;

		if (laneCount == 1)       ddiCtl |= 0x1;
		else if (laneCount == 2)  ddiCtl |= 0x3;
		else if (laneCount == 4)  ddiCtl |= 0xF;

		NBlue::callback->writeReg32(regs.reg_ddi_buf_ctl_0, ddiCtl);
		

	} else {
		// DEKEL Logic (Full Loop)
		/*uint32_t phyInfo;// = port->readRegister32(*(uint32_t*)(port->vt + 0x1B8));
		uint32_t laneMask = phyInfo >> 2;
		for (int i = 0; i < 2; i++) {
			uint32_t currentLaneMask = (i == 0) ? phyInfo : laneMask;
			currentLaneMask &= 3;

			for (int lane = 0; lane < 2; lane++) {
				if ((currentLaneMask >> lane) & 1) {
					uint32_t regAddr = (lane == 0) ? ((i == 0) ? 0x42 : 0x45) : ((i == 0) ? 0x43 : 0x46);
					uint32_t regVal = ReadDekelPhyReg(this, regAddr);
					if (lane == 0) {
						uint32_t vswing = *(uint32_t*)driveConfig & 0x1F;
						uint32_t preemp = *((uint32_t*)driveConfig + 1) & 0x1F;
						uint32_t term   = *((uint32_t*)driveConfig + 2) & 0x1F;
						regVal = (regVal & 0xFFFC00F8) | (term << 8) | (preemp << 13) | vswing;
					}
					WriteDekelPhyReg(this, regAddr, regVal);
				}
				uint32_t ctrlReg = (lane == 0) ? 0x44 : 0x47;
				uint32_t ctrlVal = ReadDekelPhyReg(this, ctrlReg);
				WriteDekelPhyReg(this, ctrlReg, ctrlVal & 0xFFFFFFFB);
			}
		}*/
	}
	return 0;
	
};

uint32_t Gen11::CallBackAGDC(void *that,uint32_t param_1,unsigned long param_2, uint param_3)
{
	return 1;
	
	if (param_1==0xb) return 0;
	auto ret=FunctionCast(CallBackAGDC, callback->oCallBackAGDC)(that ,param_1,param_2,param_3);
	
	return ret;
}

void* Gen11::getFirstPathByPort(void *that)
{
	auto ret=FunctionCast(getFirstPathByPort, callback->ogetFirstPathByPort)(that);
	return ret;
}

uint32_t Gen11::setLinkTrainingPattern(void *that,uint8_t param_1)
{
	auto ret=FunctionCast(setLinkTrainingPattern, callback->osetLinkTrainingPattern)(that,param_1);
	return ret;
}

int  Gen11::connectionProbe(void *that)
{
	auto ret=FunctionCast(connectionProbe, callback->oconnectionProbe)(that);
	return ret;
}

int Gen11::writeAUX(void *that,uint param_1,void *param_2,uint param_3)
{
	auto ret=FunctionCast(writeAUX, callback->owriteAUX)(that ,param_1,param_2,param_3);
	return ret;
}
int Gen11::readAUX(void *that,uint param_1,void *param_2,uint param_3)
{
	auto ret=FunctionCast(readAUX, callback->oreadAUX)(that ,param_1,param_2,param_3);
	return ret;
}

int au1=1;
uint32_t  Gen11::getDPCDInfo(void *that)
{
	if (au1){
		au1=0;
		struct DPCD_BranchOuiData o;
		readAUX(that,0x400,&o,sizeof(o));
		o.oui_0=0x00;
		o.oui_1=0x10;
		o.oui_2=0xFA;
		writeAUX(that,0x400,&o,sizeof(o));
		readAUX(that,0x400,&o,sizeof(o));
	}
	auto ret=FunctionCast(getDPCDInfo, callback->ogetDPCDInfo)(that);
	return ret;
}
void  Gen11::setDCState(void *that,bool param_1)
{
	FunctionCast(setDCState, callback->osetDCState)(that ,param_1);

}



int64_t Gen11::ddpsmNotify(void *that,uint *param_1)
{

	if ((*param_1 & 1) == 0) {
		//*param_1 = 2;
		hwu=1;
		//getMember<uint32_t>(getMember<void*>(that, 0xd60), 0x4284 )=1;
		//ssetFramebufferPowerState(getMember<void*>(that, 0xd60),2);
		//enableController(getMember<void *>(that, 0xd60 ));
		
		/*IOFramebuffer *r= (IOFramebuffer *)getMember<void *>(that, 0xd60);
		
		getMember<uint8_t>(that, 0xe61 )=1;//lidisclosed
		getMember<uint8_t>(r, 0x1e1 )=0;//newOnlineState
		r->setAttributeForConnection(0,'prob',1);
		*/
		/*getMember<uint8_t>(that, 0xe61 )=0;
		getMember<uint8_t>(r, 0x1e1 )=1;
		r->setAttributeForConnection(0,'prob',1);
		getMember<uint8_t>(r, 0x1e0 )=1;*/
		
		
		
	}
	auto ret=FunctionCast(ddpsmNotify, callback->oddpsmNotify)(that ,param_1);
	if ((*param_1 & 1) == 0) return -1;
	return ret;
}

void  Gen11::enableSWDC6(void *that,bool param_1)
{
	 FunctionCast(enableSWDC6, callback->oenableSWDC6)(that ,param_1);

}

int  Gen11::handleEnableHDCP(void *that)
{
	
	auto ret=FunctionCast(handleEnableHDCP, callback->ohandleEnableHDCP)(that);
	
	IOFramebuffer *r= (IOFramebuffer *)getMember<void *>(that, 0x18);
	
	/*void *ctr=getMember<void *>(that, 0x10);
	
	getMember<uint8_t>(ctr, 0xe61 )=0;//lidisclosed
	getMember<uint8_t>(r, 0x1e1 )=1;//newOnlineState
	r->setAttributeForConnection(0,'prob',1);
	
	getMember<uint8_t>(r, 0x1e0 )=1;*/

	r->setAttribute('wsrv',0x11);
	r->setAttribute('wsrv',0x4);
	
	//void *p=getMember<void*>(r, 0x4a08 );
	//getMember<uint32_t>(p, 0x3644 )=0;
	
	//getMember<uint8_t>(r, 0x1e1 )=0;
	//r->setAttributeForConnection(0,'prob',1);
	
	//DisplaySleepCallback(getMember<void *>(that, 0x10),1,0,1);
	
	
	
	
	
	//r->setAttribute('wsrv',0x11);

	//DisplaySleepCallback(getMember<void *>(that, 0x10),0,0,1);
	
	//r->setAttributeForConnection(0,'prob',1);
	
	//ssetFramebufferPowerState(r,2);
	//enableController(r);
	
	
	
	return ret;
}

uint64_t Gen11::ssetFramebufferPowerState(void *that,uint param_1)
{
	auto ret=FunctionCast(ssetFramebufferPowerState, callback->ossetFramebufferPowerState)(that ,param_1);

	return ret;
}

uint32_t Gen11::SetFbStatusOnNextProbe(void *that,AGDCFBOnline_t *param_1)
{
	auto ret=FunctionCast(SetFbStatusOnNextProbe, callback->oSetFbStatusOnNextProbe)(that ,param_1);

	return ret;
}

void Gen11::SetupParams (void *that,void *param_1,void *param_2,void *param_3,void *param_4)
{
	
	FunctionCast(SetupParams, callback->oSetupParams)(that ,param_1,param_2,param_3,param_4);
	struct crtparams* c=(struct crtparams*)param_3;
	c->TRANS_CLK_SEL=0x10000000;
}


void Gen11::sanitizeCDClockFrequency(void *that) {

	//auto referenceFrequency = callback->wrapReadRegister32(that, SKL_DSSM) & ICL_DSSM_CDCLK_PLL_REFCLK_MASK;
	auto referenceFrequency =NBlue::callback->readReg32(ICL_REG_DSSM)>> 29;
	//auto referenceFrequency = callback->wrapReadRegister32(that, ICL_REG_DSSM) >> 29;
	uint32_t newCdclkFrequency = 0;
	uint32_t newPLLFrequency = 0;
	switch (referenceFrequency) {
		case ICL_REF_CLOCK_FREQ_19_2:
			newCdclkFrequency = ICL_CDCLK_FREQ_652_8;
			newPLLFrequency = ICL_CDCLK_PLL_FREQ_REF_19_2;
			break;
			
		case ICL_REF_CLOCK_FREQ_24_0:
			newCdclkFrequency = ICL_CDCLK_FREQ_648_0;
			newPLLFrequency = ICL_CDCLK_PLL_FREQ_REF_24_0;
			break;
			
		case ICL_REF_CLOCK_FREQ_38_4:
			newCdclkFrequency = ICL_CDCLK_FREQ_652_8;
			newPLLFrequency = ICL_CDCLK_PLL_FREQ_REF_38_4;
			break;
			
		default:
			return;
	}
	
	disableCDClock(that);
	
	callback->orgSetCDClockFrequency(that, newPLLFrequency);
	NBlue::callback->readReg32( ICL_REG_CDCLK_CTL) & 0x7FF;
}

uint32_t Gen11::wrapProbeCDClockFrequency(void *that) {

	//auto cdclk = NBlue::callback->readReg32(ICL_REG_CDCLK_CTL) & BXT_CDCLK_CD2X_DIV_SEL_MASK;
	auto cdclk =NBlue::callback->readReg32(ICL_REG_CDCLK_CTL) & CDCLK_FREQ_DECIMAL_MASK;
	
	if (cdclk < ICL_CDCLK_DEC_FREQ_THRESHOLD) {
		sanitizeCDClockFrequency(that);
	}
	
	auto retVal = callback->orgProbeCDClockFrequency(that);
	return retVal;
}


bool Gen11::start(void *that,void  *param_1)
{
	
	auto ret= FunctionCast(start, callback->ostart)(that,param_1);
	return ret;
}
int Gen11::wrapPmNotifyWrapper(unsigned int a0, unsigned int a1, unsigned long long *a2, unsigned int *freq) {
	
	/*struct intel_rps_freq_caps *caps;
	
	caps->rp0_freq *= GEN9_FREQ_SCALER;
	caps->rp1_freq *= GEN9_FREQ_SCALER;
	caps->min_freq *= GEN9_FREQ_SCALER;
	
	uint32_t mult=GEN9_FREQ_SCALER;
	uint32_t ddcc_status = 0;
	
	*freq =caps->rp1_freq;
	return 0;*/
	
	uint32_t cfreq = 0;

	FunctionCast(wrapPmNotifyWrapper, callback->orgPmNotifyWrapper)(a0, a1, a2, &cfreq);
	
	if (!callback->freq_max) {
		callback->freq_max = wrapReadRegister32(callback->framecont, GEN6_RP_STATE_CAP) & 0xFF;

	}
	
	*freq = (GEN9_FREQ_SCALER << GEN9_FREQUENCY_SHIFT) * callback->freq_max;
	return 0;
}

bool Gen11::patchRCSCheck(mach_vm_address_t& start) {
	constexpr unsigned ninsts_max {256};
	
	hde64s dis;
	
	bool found_cmp = false;
	bool found_jmp = false;

	for (size_t i = 0; i < ninsts_max; i++) {
		auto sz = Disassembler::hdeDisasm(start, &dis);

		if (dis.flags & F_ERROR) {
			break;
		}

		/* cmp byte ptr [rcx], 0 */
		if (!found_cmp && dis.opcode == 0x80 && dis.modrm_reg == 7 && dis.modrm_rm == 1)
			found_cmp = true;
		/* jnz rel32 */
		if (found_cmp && dis.opcode == 0x0f && dis.opcode2 == 0x85) {
			found_jmp = true;
			break;
		}

		start += sz;
	}
	
	if (found_jmp) {
		auto status = MachInfo::setKernelWriting(true, KernelPatcher::kernelWriteLock);
		if (status == KERN_SUCCESS) {
			constexpr uint8_t nop6[] {0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
			lilu_os_memcpy(reinterpret_cast<void*>(start), nop6, arrsize(nop6));
			MachInfo::setKernelWriting(false, KernelPatcher::kernelWriteLock);
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

int forceWakei=1;
void Gen11::forceWake(void *that, bool set, uint32_t dom, uint8_t ctx) {
	// ctx 2: IRQ, ctx 1: normal
	
	uint32_t ack_exp = set << ctx;
	uint32_t mask = 1 << ctx;
	uint32_t wr = ack_exp | (1 << ctx << 16);
	unsigned d2;
	
	
	if (forceWakei){
		/*uint32_t cap=	wrapReadRegister32(callback->framecont,HSW_EDRAM_CAP);
		const unsigned int ways[8] = { 4, 8, 12, 16, 16, 16, 16, 16 };
		const unsigned int sets[4] = { 1, 1, 2, 2 };
		uint64_t size= EDRAM_NUM_BANKS(cap) *
				ways[EDRAM_WAYS_IDX(cap)] *
				sets[EDRAM_SETS_IDX(cap)] *
				1024 * 1024;
		
		SYSLOG("nblue","Found %lluMB of eDRAM\n", size / (1024 * 1024));*/
		

		
		
		/*for (unsigned d = FW_DOMAIN_ID_RENDER; d <= FW_DOMAIN_ID_COUNT;d++){
			wrapWriteRegister32(callback->framecont, regForDom(BIT(d)), fw_clear(FORCEWAKE_KERNEL));
			wrapWriteRegister32(callback->framecont, regForDom(BIT(d)), fw_clear(0xffff));
		}*/
		forceWakei=0;
	}

	/*enum forcewake_domain_id {
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
	};

	enum forcewake_domain_id {
		FW_DOMAIN_ID_RENDER = 0,
		FW_DOMAIN_ID_GT,
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
	};*/
	
	//if (dom & 7) dom=BIT(FW_DOMAIN_ID_MEDIA_VEBOX1);
	//if (dom & 8) dom=BIT(FW_DOMAIN_ID_MEDIA_VEBOX2);
	
	/*for (unsigned d = DOM_FIRST; d <= DOM_LAST; d <<= 1)
	if (dom & d) {
		wrapWriteRegister32(callback->framecont, regForDom(d), wr);
		IOPause(100);
		if (!pollRegister(ackForDom(d), ack_exp, mask, FORCEWAKE_ACK_TIMEOUT_MS) &&
			!forceWakeWaitAckFallback(d, ack_exp, mask) &&
			!pollRegister(ackForDom(d), ack_exp, mask, FORCEWAKE_ACK_TIMEOUT_MS))
			SYSLOG("nblue", "PANIC ForceWake timeout for %d domain %d, expected 0x%x", d,(dom), ack_exp);
	}
	return;*/
	
	if (dom == 7) dom=BIT(FW_DOMAIN_ID_MEDIA_VEBOX0);
	if (dom == 8) dom=BIT(FW_DOMAIN_ID_MEDIA_VEBOX1);
	
	/*if (dom & 1) dom=BIT(FW_DOMAIN_ID_RENDER);
	if (dom & 2) dom=BIT(FW_DOMAIN_ID_MEDIA_VDBOX0)|BIT(FW_DOMAIN_ID_MEDIA_VDBOX2)|BIT(FW_DOMAIN_ID_MEDIA_VEBOX0);
	if (dom & 4) dom=BIT(FW_DOMAIN_ID_MEDIA_VDBOX0)|BIT(FW_DOMAIN_ID_MEDIA_VDBOX2)|BIT(FW_DOMAIN_ID_MEDIA_VEBOX0)|BIT(FW_DOMAIN_ID_RENDER)|BIT(FW_DOMAIN_ID_GT);*/
	
	for (unsigned d = FW_DOMAIN_ID_RENDER; d <= FW_DOMAIN_ID_COUNT;d++){
		d2 = d << 1;
		if (dom & d2) {
			//if (d != FW_DOMAIN_ID_RENDER && d != FW_DOMAIN_ID_GT && d != FW_DOMAIN_ID_MEDIA_VDBOX0 && d != FW_DOMAIN_ID_MEDIA_VDBOX2 && d != FW_DOMAIN_ID_MEDIA_VEBOX0) continue;
			wrapWriteRegister32(callback->framecont, regForDom(d2), wr);
			IOPause(100);
			if (!pollRegister(ackForDom(d2), ack_exp, mask, FORCEWAKE_ACK_TIMEOUT_MS) &&
				!forceWakeWaitAckFallback(d2, ack_exp, mask) &&
				!pollRegister(ackForDom(d2), ack_exp, mask, FORCEWAKE_ACK_TIMEOUT_MS))
				SYSLOG("nblue", "PANIC ForceWake timeout for %d domain %d, expected 0x%x", d,(dom), ack_exp);
		}
	}
}

bool Gen11::pollRegister(uint32_t reg, uint32_t val, uint32_t mask, uint32_t timeout) {
	AbsoluteTime now, deadline;
	
	clock_interval_to_deadline(timeout, kMillisecondScale, &deadline);
	
	for (clock_get_uptime(&now); now < deadline; clock_get_uptime(&now)) {
		auto rd = wrapReadRegister32(callback->framecont, reg);

//		DBGLOG(log, "Rd 0x%x = 0x%x, expected 0x%x", reg, rd, val);

		if ((rd & mask) == val)
			return true;
	}

	return false;
}

bool Gen11::forceWakeWaitAckFallback(uint32_t d, uint32_t val, uint32_t mask) {
	unsigned pass = 1;
	bool ack = false;
	auto controller = callback->framecont;
	
	/*do {
		pollRegister(ackForDom(d), 0, FORCEWAKE_KERNEL_FALLBACK, FORCEWAKE_ACK_TIMEOUT_MS);
		wrapWriteRegister32(controller, regForDom(d), fw_set(FORCEWAKE_KERNEL_FALLBACK));
		
		IODelay(10 * pass);
		pollRegister(ackForDom(d), FORCEWAKE_KERNEL_FALLBACK, FORCEWAKE_KERNEL_FALLBACK, FORCEWAKE_ACK_TIMEOUT_MS);

		ack = (wrapReadRegister32(controller, ackForDom(d)) & mask) == val;

		wrapWriteRegister32(controller, regForDom(d), fw_clear(FORCEWAKE_KERNEL_FALLBACK));
	} while (!ack && pass++ < 10);*/
	
//	DBGLOG(log, "Force wake fallback used to %s %s in %u passes", set ? "set" : "clear", strForDom(d), pass);
	
	return ack;
}

void Gen11::dovoid()
{
	
	
}

bool Gen11::dotrue()
{
	
	return true;
}


int iniin=1;
void  Gen11::readAndClearInterrupts(void *that,void *param_1)
{
	
	if (iniin){
		iniin=0;
		
		wrapWriteRegister32(callback->framecont, GEN11_GFX_MSTR_IRQ, 0);
		wrapWriteRegister32(callback->framecont,GEN11_DISPLAY_INT_CTL, 0);
		uint32_t master_ctl=wrapReadRegister32(callback->framecont,GEN11_GFX_MSTR_IRQ);
		
		//Disable RCS, BCS, VCS and VECS class engines.
		wrapWriteRegister32(callback->framecont, GEN11_RENDER_COPY_INTR_ENABLE, 0);
		wrapWriteRegister32(callback->framecont, GEN11_VCS_VECS_INTR_ENABLE,	  0);
		
		// Restore masks irqs on RCS, BCS, VCS and VECS engines.
		wrapWriteRegister32(callback->framecont, GEN11_RCS0_RSVD_INTR_MASK,	~0);
		wrapWriteRegister32(callback->framecont, GEN11_BCS_RSVD_INTR_MASK,	~0);
		wrapWriteRegister32(callback->framecont, GEN11_VCS0_VCS1_INTR_MASK,	~0);
		wrapWriteRegister32(callback->framecont, GEN11_VCS2_VCS3_INTR_MASK,	~0);
		wrapWriteRegister32(callback->framecont, GEN11_VECS0_VECS1_INTR_MASK,	~0);
		
		wrapWriteRegister32(callback->framecont, GEN11_GPM_WGBOXPERF_INTR_ENABLE, 0);
		wrapWriteRegister32(callback->framecont, GEN11_GPM_WGBOXPERF_INTR_MASK,  ~0);
		wrapWriteRegister32(callback->framecont, GEN11_GUC_SG_INTR_ENABLE, 0);
		wrapWriteRegister32(callback->framecont, GEN11_GUC_SG_INTR_MASK,  ~0);
		
		
		
		uint32_t irqs = GT_RENDER_USER_INTERRUPT;
		uint32_t guc_mask = /*intel_uc_wants_guc(&gt->uc) ? GUC_INTR_GUC2HOST :*/ 0;
		uint32_t gsc_mask = 0;
		uint32_t heci_mask = 0;
		uint32_t dmask;
		uint32_t smask;
		
		irqs |= GT_CS_MASTER_ERROR_INTERRUPT |
		GT_CONTEXT_SWITCH_INTERRUPT |
		GT_WAIT_SEMAPHORE_INTERRUPT;
		
		dmask = irqs << 16 | irqs;
		smask = irqs << 16;
		
		
		/* Enable RCS, BCS, VCS and VECS class interrupts. */
		wrapWriteRegister32(callback->framecont, GEN11_RENDER_COPY_INTR_ENABLE, dmask);
		wrapWriteRegister32(callback->framecont, GEN11_VCS_VECS_INTR_ENABLE, dmask);
		
		/* Unmask irqs on RCS, BCS, VCS and VECS engines. */
		wrapWriteRegister32(callback->framecont, GEN11_RCS0_RSVD_INTR_MASK, ~smask);
		wrapWriteRegister32(callback->framecont, GEN11_BCS_RSVD_INTR_MASK, ~smask);
		wrapWriteRegister32(callback->framecont, GEN11_VCS0_VCS1_INTR_MASK, ~dmask);
		wrapWriteRegister32(callback->framecont, GEN11_VCS2_VCS3_INTR_MASK, ~dmask);
		wrapWriteRegister32(callback->framecont, GEN11_VECS0_VECS1_INTR_MASK, ~dmask);
		
		/*
		 * RPS interrupts will get enabled/disabled on demand when RPS itself
		 * is enabled/disabled.
		 */
		
		
		wrapWriteRegister32(callback->framecont, GEN11_GPM_WGBOXPERF_INTR_ENABLE, 0);
		wrapWriteRegister32(callback->framecont, GEN11_GPM_WGBOXPERF_INTR_MASK,  ~0);
		
		/* Same thing for GuC interrupts */
		wrapWriteRegister32(callback->framecont, GEN11_GUC_SG_INTR_ENABLE, 0);
		wrapWriteRegister32(callback->framecont, GEN11_GUC_SG_INTR_MASK,  ~0);
		
		wrapWriteRegister32(callback->framecont,GEN11_DISPLAY_INT_CTL, GEN11_DISPLAY_IRQ_ENABLE);
		wrapWriteRegister32(callback->framecont, GEN11_GFX_MSTR_IRQ, GEN11_MASTER_IRQ);
	}
	
	
	FunctionCast(readAndClearInterrupts, callback->oreadAndClearInterrupts)(that,param_1);
}


void * Gen11::serviceInterrupts(void *param_1)
{
	return FunctionCast(serviceInterrupts, callback->oserviceInterrupts)(param_1);
	
}

void * Gen11::wprobe(void *that,void *param_1,int *param_2)
{
	//FunctionCast(wprobe, callback->owprobe)(that, param_1,param_2);
	//logStateInRegistry(that,0x56);
	initializeLogging(that);
	return that;
	
}

bool  Gen11::tgstart(void *that,void *param_1)
{
	FunctionCast(tgstart, callback->otgstart)(that, param_1);
	return true;
	
}

void Gen11::FBMemMgr_Init(void *that)
{
	
	FunctionCast(FBMemMgr_Init, callback->oFBMemMgr_Init)(that);
	ccont2=that;
	ccont = getMember<void *>(that, 0xc40);

	/*IODeviceMemory * m= NBlue::callback->iGPU->getDeviceMemoryWithIndex(0);
	IODeviceMemory *dm;
	m->withSubRange(dm,0x4180000,0x12000);//fDSBBufferBytes = 73728, fDSBBufferBaseOffset = 68681728
	IOMemoryMap *dsb=dm->map();
	
	IODeviceMemory *dm2;
	m->withSubRange(dm2,0x4192000,0x3000);//fConnectionStatusBytes = 12288, fConnectionStatusOffset = 68755456
	IOMemoryMap *dsb2=dm2->map();*/
	
}

uint32_t Gen11::probePortMode()
{
	auto ret=FunctionCast(probePortMode, callback->oprobePortMode)();
	return ret;
};


uint32_t Gen11::wdepthFromAttribute(void *that,uint param_1)
{
	return 0x1e;
};

uint32_t Gen11::raReadRegister32(void *that,unsigned long param_1)
{
	//if (reinterpret_cast<volatile uint64_t*>(that)==nullptr) return NBlue::callback->readReg32(param_1);
	//PANIC_COND(reinterpret_cast<volatile uint64_t*>(that)==nullptr, "nblue", "raReadRegister32 Failed 0x%lx",param_1);
	//if (reinterpret_cast<volatile uint64_t*>(that)==nullptr) return 0;
	
	//if (param_1 >=0x2000 && NBlue::callback->readReg32(param_1)) return NBlue::callback->readReg32(param_1);
	auto ret=FunctionCast(raReadRegister32, callback->oraReadRegister32)(that,param_1);
	return ret;
};

unsigned long Gen11::raReadRegister32b(void *that,void *param_1,unsigned long param_2)
{
	//if (reinterpret_cast<volatile uint64_t*>(that)==nullptr) return 0;
	//if (reinterpret_cast<volatile uint64_t*>(param_1)==nullptr) return 0;
	return  raReadRegister32(that,reinterpret_cast<uint64_t>(param_1) + param_2);
};


unsigned long  Gen11::loadGuCBinary(void *that)
{
	
	FunctionCast(loadGuCBinary, callback->oloadGuCBinary)(that);
	return 1;
}

uint64_t Gen11::raReadRegister64(void *that,unsigned long param_1)
{
	//if (reinterpret_cast<volatile uint64_t*>(that)==nullptr) return 0;
	
	return FunctionCast(raReadRegister64, callback->oraReadRegister64)(that,param_1);
};
uint64_t Gen11::raReadRegister64b(void *that,void *param_1,unsigned long param_2)
{
	//if (reinterpret_cast<volatile uint64_t*>(that)==nullptr) return 0;
	return  raReadRegister64(that,reinterpret_cast<uint64_t>(param_1) + param_2);
};

void Gen11::radWriteRegister32(void *that,unsigned long param_1, UInt32 param_2)
{
	radWriteRegister32f( that,param_1,param_2);
};

void Gen11::radWriteRegister32f(void *that,unsigned long param_1, UInt32 param_2)
{
	//FunctionCast(radWriteRegister32f, callback->oradWriteRegister32f)( that,param_1,param_2);
};

void Gen11::raWriteRegister32(void *that,unsigned long param_1, UInt32 param_2)
{
	//if (reinterpret_cast<volatile uint64_t*>(that)==nullptr) return NBlue::callback->writeReg32(param_1,param_2);
	//PANIC_COND(reinterpret_cast<volatile uint64_t*>(that)==nullptr, "nblue", "raWriteRegister32 Failed 0x%lx",param_1);
	//if (reinterpret_cast<volatile uint64_t*>(that)==nullptr) return;
	FunctionCast(raWriteRegister32, callback->oraWriteRegister32)( that,param_1,param_2);
	
};

void Gen11::raWriteRegister32f(void *that,unsigned long param_1, UInt32 param_2)
{
	FunctionCast(raWriteRegister32f, callback->oraWriteRegister32f)( that,param_1,param_2);
};

void Gen11::raWriteRegister32b(void *that,void *param_1,unsigned long param_2, UInt32 param_3)
{
	//if (reinterpret_cast<volatile uint64_t*>(that)==nullptr) return;
	//if (reinterpret_cast<volatile uint64_t*>(param_1)==nullptr) return;
	raWriteRegister32(that, reinterpret_cast<uint64_t>(param_1) + param_2,param_3);
};
void Gen11::raWriteRegister64(void *that,unsigned long param_1,UInt64 param_2)
{
	//if (reinterpret_cast<volatile uint64_t*>(that)==nullptr) return;
	FunctionCast(raWriteRegister64, callback->oraWriteRegister64)( that,param_1,param_2);
};

void Gen11::raWriteRegister64b(void *that,void *param_1,unsigned long param_2,UInt64 param_3)
{
	//if (reinterpret_cast<volatile uint64_t*>(that)==nullptr) return;
	raWriteRegister64( that,reinterpret_cast<uint64_t>(param_1) + param_2,param_3);
};

void Gen11::setupPlanarSurfaceDBUF()
{
	//FunctionCast(setupPlanarSurfaceDBUF, callback->osetupPlanarSurfaceDBUF)();
};

void Gen11::updateDBUF(void *that,uint param_1,uint param_2,bool param_3)
{
	//setupPlanarSurfaceDBUF();
};




int Gen11::handleLinkIntegrityCheck()
{
	return 0;
};

void  Gen11::prepareToEnterWake(void *that)
{
	
	bool Var5 = isPanelPowerOn(getMember<void *>(that, 0x1d0));
	if (Var5) {
		IORegistryEntry *r= (IORegistryEntry *)that;
		r->setProperty("AAPL,LCD-PowerState-ON", true);
	}
	FunctionCast(prepareToEnterWake, callback->oprepareToEnterWake)(that);
	
};

uint32_t Gen11::disableVDDForAux(void *that)
{
	
		uint32_t ppControl = NBlue::callback->readReg32(0xC7204);
	NBlue::callback->writeReg32(0xC7204, ppControl & 0xFFFFFFF7);

		int retries = 0x32;
		while (retries > 0) {
			IOSleep(10);
			
			int32_t ppStatus = NBlue::callback->readReg32(0xC7200);
			
			if (ppStatus > -1) {

				if (getMember<bool>(that, 0xe62) == true) {
					IOFramebuffer *r= (IOFramebuffer *)getMember<void *>(that, 0xd60);
					r->setProperty("AAPL,LCD-PowerState-ON", false);
				}

				return kIOReturnSuccess;
			}
			retries--;
		}

		IOSleep(300);

		return kIOReturnSuccess;

	
};




uint32_t Gen11::enableVDDForAux(void *that,void *param_1)
{

		uint32_t ppControl = NBlue::callback->readReg32(0xC7204);
		int32_t ppStatus = NBlue::callback->readReg32(0xC7200);

		if ((ppControl & 0x8) == 0) {

			if ((int)ppStatus < 0) {
				return 0xe00002eb;
			}


			int timeout = 25;
			while (timeout > 0) {
				ppStatus = NBlue::callback->readReg32(0xC7200);
				if ((ppStatus >> 27 & 1) == 0) {
					break;
				}
				IOSleep(20);
				timeout--;
			}


			NBlue::callback->writeReg32(0xC7204, ppControl | 0x8);

			if (getMember<bool>(that, 0xe62) == true) {
				IOFramebuffer *r= (IOFramebuffer *)getMember<void *>(that, 0xd60);
				r->setProperty("AAPL,LCD-PowerState-ON", true);
			}

			
			if (getMember<uint32_t>(param_1, 0xd48) != 0) {
				int hpdTimer = 0;
				while (hpdTimer != 300) {
					if (isHPDLow(param_1) == 0) {
						return kIOReturnSuccess;
					}
					IOSleep(20);
					hpdTimer += 20;
				}

				return 0xe00002d6;
			}
		}

	return kIOReturnSuccess;

};

uint64_t Gen11::setPanelPowerState(void *that,uint param_1)
{
	return FunctionCast(setPanelPowerState, callback->osetPanelPowerState)(that,param_1);
	
	//IORegistryEntry *r= (IORegistryEntry *)getMember<long *>(that, 0xd60);
	//r->setProperty("AAPL,LCD-PowerState-ON", param_1);
};




void Gen11::logStateInRegistry(void *that,uint param_1)
{
 FunctionCast(logStateInRegistry, callback->ologStateInRegistry)(that,param_1 );
}

void Gen11::initializeLogging(void *that)
{
	FunctionCast(initializeLogging, callback->oinitializeLogging)(that );
}

int Gen11::getPlatformID()
{
 return FunctionCast(getPlatformID, callback->ogetPlatformID)( );
}





bool inpwell=false;
void Gen11::PowerWellinit(void *that,void *param_1)
{
	inpwell=true;
  FunctionCast(PowerWellinit, callback->oPowerWellinit)(that,param_1 );
	inpwell=false;
}

long Gen11::getPortByDDI(uint param_1)
{
 auto ret= FunctionCast(getPortByDDI, callback->ogetPortByDDI)(param_1 );
	if (inpwell) setPortMode((void*)ret,1);
 return ret;
}

uint8_t  Gen11::setPortMode(void *that,uint32_t param_1)
{
 auto ret= FunctionCast(setPortMode, callback->osetPortMode)(that,param_1 );
	
 return ret;
}


uint8_t Gen11::validateModeDepth(void *that,void *param_1,uint param_2)
{
	auto ret= FunctionCast(validateModeDepth, callback->ovalidateModeDepth)(that,param_1,param_2 );
	if (ret==0) return 8;
	return ret;
}

IOReturn Gen11::wrapICLReadAUX(void *that, uint32_t address, void *buffer, uint32_t length) {

	IOReturn retVal =	FunctionCast(wrapICLReadAUX, callback->orgICLReadAUX)(that,address, buffer, length );

	if (address != 0x0000 && address != 0x2200)	return retVal;
	
	auto caps = reinterpret_cast<DPCDCap16*>(buffer);
	
	if (caps->revision < 0x03) {
		caps->maxLinkRate=0;
	}
	
	return retVal;
}



int Gen11::isConflictRegister()
{
	
	return -1;

}



void Gen11::AppleIntelPowerWellinit(void *that,void *param_1)
{
	//ccont = getMember<void *>(param_1, 0xc40);
	
	FunctionCast(AppleIntelPowerWellinit, callback->oAppleIntelPowerWellinit)(that,param_1);
	
	const u32 TGL_PCODE_TCCOLD = 0x12;
			const u32 TGL_PCODE_EXIT_TCCOLD_DATA_L_UNBLOCK_REQ = 0;
			const u32 TGL_PCODE_EXIT_TCCOLD_DATA_L_EXIT_FAILED = 0x80000000;

			u8 tc_tries = 0;
			int tc_ret;
			
			while (1) {
				u32 low_val = TGL_PCODE_EXIT_TCCOLD_DATA_L_UNBLOCK_REQ;
				u32 high_val = 0;

				 hwWriteMailbox(ccont2, TGL_PCODE_TCCOLD, low_val, true);
				tc_ret =hwReadMailbox(ccont2, TGL_PCODE_TCCOLD);
				if (tc_ret == 0) {
					break;
				}

				if (++tc_tries == 3)
					break;

				IOSleep(1);
			}
	

	
}





void Gen11::initCDClock(void *that)
{
	FunctionCast(initCDClock, callback->oinitCDClock)(that );
	
	
	/*if (DISPLAY_VER(display) == 35)
		xe3plpd_display_wa_apply(display);
	else if (display->platform.alderlake_p)
		adlp_display_wa_apply(display);
	else if (DISPLAY_VER(display) == 12)
		xe_d_display_wa_apply(display);
	else if (DISPLAY_VER(display) == 11)
		gen11_display_wa_apply(display);*/
	
	//11
	//NBlue::callback->intel_de_rmw( GEN8_CHICKEN_DCPR_1, 0, ICL_DELAY_PMRSP);
	
	//12
	NBlue::callback->intel_de_rmw( CLKREQ_POLICY, CLKREQ_POLICY_MEM_UP_OVRD, 0);

}

void Gen11::setCDClockFrequencyOnHotplug(void *that)
{
	return FunctionCast(setCDClockFrequencyOnHotplug, callback->osetCDClockFrequencyOnHotplug)(that );
}


void Gen11::disableCDClock(void *that)
{
	FunctionCast(disableCDClock, callback->odisableCDClock)(that );
}


bool Gen11::AppleIntelFramebufferinit(void *frame,void *cont,uint32_t param_2)
{
	getMember<void *>(frame, 0x4a40) = ccont;
	getMember<void *>(frame, 0xc40) = ccont;
	auto ret=FunctionCast(AppleIntelFramebufferinit, callback->oAppleIntelFramebufferinit)(frame,cont,param_2 );
	
	return ret;
}

uint64_t  Gen11::AppleIntelPlaneinit(void *that,uint8_t param_1)
{
	
	auto ret= FunctionCast(AppleIntelPlaneinit, callback->oAppleIntelPlaneinit)(that,param_1 );
	getMember<void *>(that, 0x90) = ccont;
	return ret;
}

uint32_t  Gen11::AppleIntelPortinit(void *that,void *param_1)
{
	
	auto ret=  FunctionCast(AppleIntelPortinit, callback->oAppleIntelPortinit)(that,param_1 );
	getMember<void *>(that, 0xc40) = ccont;
	return ret;
}

unsigned long Gen11::AppleIntelScalerinit(void *that,uint8_t param_1)
{
	
	auto ret=  FunctionCast(AppleIntelScalerinit, callback->oAppleIntelScalerinit)(that,param_1 );
	getMember<void *>(that, 0x28) = ccont;
	getMember<void *>(that, 0x10) = ccont2;
	return ret;
}

void  Gen11::disableScaler(void *that,bool param_1)
{
	getMember<void *>(that, 0x28) = ccont;
	FunctionCast(disableScaler, callback->odisableScaler)(that,param_1 );
}

void Gen11::programPipeScaler(void *that,void *param_1)
{
	getMember<void *>(that, 0x28) = ccont;
	FunctionCast(programPipeScaler, callback->oprogramPipeScaler)(that,param_1 );
}

void  Gen11::enablePlane(void *that,bool param_1)
{
	//getMember<void *>(that, 0x90) = ccont;
	FunctionCast(enablePlane, callback->oenablePlane)(that,param_1 );
}

void Gen11::AppleIntelScalerupdateRegisterCache(void *that)
{
	getMember<void *>(that, 0x28) = ccont;
	FunctionCast(AppleIntelScalerupdateRegisterCache, callback->oAppleIntelScalerupdateRegisterCache)(that );
}

void Gen11::AppleIntelPlaneupdateRegisterCache(void *that)
{
	getMember<void *>(that, 0x90) = ccont;
	FunctionCast(AppleIntelPlaneupdateRegisterCache, callback->oAppleIntelPlaneupdateRegisterCache)(that );
}
uint32_t Gen11::hwReadMailbox(void *that,uint param_1)
{
	return FunctionCast(hwReadMailbox, callback->ohwReadMailbox)(that,param_1 );
}

void Gen11::hwWriteMailbox(void *that,uint param_1,uint param_2,bool param_3)
{
	FunctionCast(hwWriteMailbox, callback->ohwWriteMailbox)(that,param_1,param_2,param_3 );
}
void Gen11::enableDisplayEngine(void *that)
{
	FunctionCast(enableDisplayEngine, callback->oenableDisplayEngine)(that );

}

void Gen11::disableDisplayEngine(void *that)
{
	getMember<void *>(that, 0x78) = ccont;
	FunctionCast(disableDisplayEngine, callback->odisableDisplayEngine)(that );
}

void  Gen11::disablePowerWellPG(void *that,uint param_1)
{
	getMember<void *>(that, 0x78) = ccont;
	FunctionCast(disablePowerWellPG, callback->odisablePowerWellPG)(that,param_1 );
}
void  Gen11::enablePowerWellPG(void *that,uint param_1)
{
	getMember<void *>(that, 0x78) = ccont;
	FunctionCast(enablePowerWellPG, callback->oenablePowerWellPG)(that,param_1 );
}

void Gen11::hwSetPowerWellStatePG(void *that,bool param_1,uint param_2)
{
	//getMember<void *>(that, 0x78) = ccont;
	FunctionCast(hwSetPowerWellStatePG, callback->ohwSetPowerWellStatePG)(that,param_1,param_2);
}

void  Gen11::overridePowerWellsState(void *that,bool param_1)
{
	FunctionCast(overridePowerWellsState, callback->ooverridePowerWellsState)(that,param_1);
}

void Gen11::hwSetPowerWellStateDDI(void *that,bool param_1,uint param_2)
{
	getMember<void *>(that, 0x78) = ccont;
	FunctionCast(hwSetPowerWellStateDDI, callback->ohwSetPowerWellStateDDI)(that,param_1,param_2);
}

void  Gen11::enableComboPhy(void *that)
{

	//FunctionCast(enableComboPhy, callback->oenableComboPhy)(that);
	
	uint32_t val;
	enum phy phy=PHY_A;
	const struct icl_procmon *procmon;
	
	val = NBlue::callback->intel_de_read( ICL_PORT_COMP_DW3(phy));
	switch (val & (PROCESS_INFO_MASK | VOLTAGE_INFO_MASK)) {
	default:
	case VOLTAGE_INFO_0_85V | PROCESS_INFO_DOT_0:
			procmon= &icl_procmon_values[PROCMON_0_85V_DOT_0];
			break;
	case VOLTAGE_INFO_0_95V | PROCESS_INFO_DOT_0:
			procmon= &icl_procmon_values[PROCMON_0_95V_DOT_0];
			break;
	case VOLTAGE_INFO_0_95V | PROCESS_INFO_DOT_1:
			procmon= &icl_procmon_values[PROCMON_0_95V_DOT_1];
			break;
	case VOLTAGE_INFO_1_05V | PROCESS_INFO_DOT_0:
			procmon= &icl_procmon_values[PROCMON_1_05V_DOT_0];
			break;
	case VOLTAGE_INFO_1_05V | PROCESS_INFO_DOT_1:
			procmon= &icl_procmon_values[PROCMON_1_05V_DOT_1];
			break;
	}

	//procmon = icl_get_procmon_ref_values(display, phy);

	NBlue::callback->intel_de_rmw( ICL_PORT_COMP_DW1(phy),
			 (0xff << 16) | 0xff, procmon->dw1);

	NBlue::callback->intel_de_write( ICL_PORT_COMP_DW9(phy), procmon->dw9);
	NBlue::callback->intel_de_write( ICL_PORT_COMP_DW10(phy), procmon->dw10);
	
	bool is_dsi=false;
	int lane_count=2;
	bool lane_reversal=false;
	uint8_t lane_mask;

	if (is_dsi) {

		switch (lane_count) {
		case 1:
			lane_mask = PWR_DOWN_LN_3_1_0;
			break;
		case 2:
			lane_mask = PWR_DOWN_LN_3_1;
			break;
		case 3:
			lane_mask = PWR_DOWN_LN_3;
			break;
		default:
		case 4:
			lane_mask = PWR_UP_ALL_LANES;
			break;
		}
	} else {
		switch (lane_count) {
		case 1:
			lane_mask = lane_reversal ? PWR_DOWN_LN_2_1_0 :
							PWR_DOWN_LN_3_2_1;
			break;
		case 2:
			lane_mask = lane_reversal ? PWR_DOWN_LN_1_0 :
							PWR_DOWN_LN_3_2;
			break;
		default:
		case 4:
			lane_mask = PWR_UP_ALL_LANES;
			break;
		}
	}

	NBlue::callback->intel_de_rmw( ICL_PORT_CL_DW10(phy),
			 PWR_DOWN_LN_MASK, lane_mask);
	
}

void Gen11::hwSetPowerWellStateAux(void *that,bool param_1,uint param_2)
{
	//getMember<void *>(that, 0x78) = ccont;
	
	//if (DISPLAY_VER(display) == 11 && intel_tc_cold_requires_aux_pw(dig_port))
	//hwWriteMailbox(ccont2,0x12,0,true);
	FunctionCast(hwSetPowerWellStateAux, callback->ohwSetPowerWellStateAux)(that,param_1,param_2);
}

void Gen11::hwInitializeCState(void *that)
{
	FunctionCast(hwInitializeCState, callback->ohwInitializeCState)(that);
	return;
	
	
	if (getMember<int>(that, 0xb48) != 1) return;
	

	const uint8_t *fw_data = tgl_dmc_ver2_12_bin;

	const struct intel_css_header *css_header = (const struct intel_css_header *)fw_data;
	uint32_t css_header_size = css_header->header_len * 4; // 0x20 * 4 = 0x80

	uint32_t package_header_offset = css_header_size; // 0x80
	const struct intel_package_header *package_header =
			(const struct intel_package_header *)&fw_data[package_header_offset];

	uint32_t package_header_size = package_header->header_len * 4; // 0x64 * 4 = 0x190

	const struct intel_fw_info *fw_info =
			(const struct intel_fw_info *)((uint8_t *)package_header + sizeof(*package_header));
	uint32_t num_entries = package_header->num_entries;
	uint8_t target_dmc_id = getMember<int>(that, 0xce4)==0 ? 0:1; //  0 for A-step, 1 for others.
	const struct intel_fw_info *selected_entry = NULL;

	for (uint32_t i = 0; i < num_entries; i++) {
			if (fw_info[i].dmc_id == target_dmc_id) {
				selected_entry = &fw_info[i];
				break;
			}
	}

	if (!selected_entry) {
			return;
	}


	uint32_t dmc_headers_base_offset = package_header_offset + package_header_size; // 0x80 + 0x190 = 0x210

	uint32_t dmc_header_offset = dmc_headers_base_offset + (selected_entry->offset * 4);
	const struct intel_dmc_header_v3 *dmc_header =
			(const struct intel_dmc_header_v3 *)&fw_data[dmc_header_offset];



	const uint8_t *payload = (const uint8_t *)dmc_header + sizeof(*dmc_header);
	const uint32_t *payload_dwords = (const uint32_t *)payload;
	uint32_t dmc_size_in_dwords = dmc_header->base.fw_size;
	uint32_t register_addr = dmc_header->start_mmioaddr; // 0x80000 or 0x90000

	for (uint32_t i = 0; i < dmc_size_in_dwords; i++) {
			//FastWriteRegister32(ccont, register_addr, payload_dwords[i]);
			NBlue::callback->writeReg32(register_addr, payload_dwords[i]);
			register_addr += 4;
	}

	for (uint32_t i = 0; i < dmc_header->mmio_count; i++) { //9 or 5 entries
			uint32_t addr = dmc_header->mmioaddr[i];
			uint32_t data = dmc_header->mmiodata[i];
			//FastWriteRegister32(ccont, addr, data);
			NBlue::callback->writeReg32(addr, data);
	}

	
	NBlue::callback->intel_de_rmw( DC_STATE_DEBUG, 0,
			 DC_STATE_DEBUG_MASK_CORES | DC_STATE_DEBUG_MASK_MEMORY_UP);
	
	//if (DISPLAY_VER(display) == 13) ...
	
	//hwConfigureCustomAUX(that,true);
	
}

void Gen11::hwConfigureCustomAUX(void *that,bool param_1)
{
	//if (!PE_parse_boot_argn("-noCustomAux")
	//FunctionCast(hwConfigureCustomAUX, callback->ohwConfigureCustomAUX)(that,param_1 );
	
}

void Gen11::FastWriteRegister32(void *that,unsigned long param_1,uint32_t param_2)
{
	return FunctionCast(FastWriteRegister32, callback->oFastWriteRegister32)(that,param_1,param_2 );
}

int Gen11::hasExternalDispla()
{
	return 0;
}
uint32_t Gen11::getFreeJoinablePathCount(void)
{
	return 1;
}

void * Gen11::getBlit3DContext(void *that,bool param_1)
{
	//return FunctionCast(getBlit3DContext, callback->ogetBlit3DContext)(that,param_1);
	
	void * blti=*getMember<void **>(that, 0x298);
	if (blti!=0) return blti;//FunctionCast(getBlit3DContext, callback->ogetBlit3DContext)(that,param_1);
	
	void * this_00 = IGHardwareBlit3DContextoperatornew(that,param_1);
	
	IGHardwareExtendedContextinitWithOptions(this_00,that,(void*)callback->Blit3DExtendedCtxParams);

	*getMember<void **>(that, 0x298)=this_00;
	return this_00;
	
}

void * Gen11::getBlit2DContext(void *that,bool param_1)
{
	return FunctionCast(getBlit2DContext, callback->ogetBlit2DContext)(that,param_1);
	
}

void *  Gen11::IGHardwareBlit3DContextoperatornew(void *that,unsigned long param_1)
{
	auto ret=FunctionCast(IGHardwareBlit3DContextoperatornew, callback->oIGHardwareBlit3DContextoperatornew)(that,param_1);
	return ret;
}


void Gen11::IGHardwareBlit3DContextinitialize(void *that)
{
	//FunctionCast(IGHardwareBlit3DContextinitialize, callback->oIGHardwareBlit3DContextinitialize)(that);
	
		*getMember<uint64_t *>(that, 0xe8)= 0;
		*getMember<uint64_t *>(that, 0xf0)= 0;
		*getMember<uint64_t *>(that, 0x100)= 0;
		*getMember<uint64_t *>(that, 0xf8)= 0;
		*getMember<uint64_t *>(that, 0x108)= 0;
		*getMember<uint32_t *>(that, 0x110)= 0;
		
		void *pIVar1 = (void *)IGMappedBuffergetMemory(*getMember<void **>(that, 0xd8));
		blit3d_initialize_scratch_space(pIVar1);
		blit3d_init_ctx(that);
	
}

int Gen11::blit3d_supported(void *param_1,void *param_2)
{
	return 0;
}

uint8_t Gen11::IGMappedBuffergetMemory(void *that)
{
	auto ret=FunctionCast(IGMappedBuffergetMemory, callback->oIGMappedBuffergetMemory)(that);
	return ret;
}

uint8_t Gen11::blit3d_init_ctx(void *that)
{
	auto ret=FunctionCast(blit3d_init_ctx, callback->oblit3d_init_ctx)(that);
	return ret;
}

void  Gen11::initBlitUsage(void *that)
{
	/*void *lVar1;
	
	lVar1 = getBlit2DContext(*getMember<void **>(that, 0x68),true);
	if (lVar1!=0){
		if (*getMember<long*>(lVar1, 0xb8)!=0)
		*getMember<uint32_t*>(that, 0x70) = *(uint32_t *)(*getMember<long*>(lVar1, 0xb8) + 0x178);
	}
	
	lVar1 = getBlit3DContext(*getMember<void **>(that, 0x68),true);
	if (lVar1!=0){
		if (*getMember<long*>(lVar1, 0xb8)!=0)
		*getMember<uint32_t*>(that, 0x74) = *(uint32_t *)(*getMember<long*>(lVar1, 0xb8) + 0x178);
	}*/
	
	FunctionCast(initBlitUsage, callback->oinitBlitUsage)(that);
}
void  Gen11::markBlitUsage(void *that)
{
	FunctionCast(markBlitUsage, callback->omarkBlitUsage)(that);
}

uint32_t  Gen11::IGAccelSegmentResourceListprepare(void *that)
{
		
	/*FunctionCast(initBlitUsage, callback->oinitBlitUsage)(that);
	
	FunctionCast(markBlitUsage, callback->omarkBlitUsage)(that);
	*/
	return 0;
	return FunctionCast(IGAccelSegmentResourceListprepare, callback->oIGAccelSegmentResourceListprepare)(that);
}


void Gen11::blit3d_initialize_scratch_space(void *that)
{
	FunctionCast(blit3d_initialize_scratch_space, callback->oblit3d_initialize_scratch_space)(that);
}

uint8_t Gen11::isPanelPowerOn(void *that)
{
	return 1;//FunctionCast(isPanelPowerOn, callback->oisPanelPowerOn)(that);
}

uint8_t  Gen11::IGHardwareExtendedContextinitWithOptions(void *that,void *param_1,void *param_2)
{
	
	return FunctionCast(IGHardwareExtendedContextinitWithOptions, callback->oIGHardwareExtendedContextinitWithOptions)(that,param_1,param_2);
	
	uint8_t ctx=getMember<uint8_t>(that, 0x6c);
	
	//intel_engine_init_workarounds(engine);
		//engine_fake_wa_init
		/*if (engine->class == COMPUTE_CLASS)
		ccs_engine_wa_init(engine, wal);
		 else if (engine->class == RENDER_CLASS)
		rcs_engine_wa_init(engine, wal);
		 else
		xcs_engine_wa_init(engine, wal);*/
	
	//intel_engine_init_whitelist(engine);
	//intel_engine_init_ctx_wa(engine);
	
	if (ctx==0)// RCS
	{
		
		//engine_fake_wa_init
		uint8_t mocs= 3;
		NBlue::callback->wa_masked_field_set(
					RING_CMD_CCTL(RENDER_RING_BASE),
					CMD_CCTL_MOCS_MASK,
					CMD_CCTL_MOCS_OVERRIDE(mocs, mocs));
		
		
		
		
		//	rcs_engine_wa_init(engine, wal);
		//rcs_engine_wa_init
		//Wa_1606700617:tgl,dg1,adl-p
		NBlue::callback->wa_masked_en(
				 GEN9_CS_DEBUG_MODE1,
				 FF_DOP_CLOCK_GATE_DISABLE);
		
		
		// Wa_1606931601:tgl,rkl,dg1,adl-s,adl-p
		NBlue::callback->wa_mcr_masked_en( GEN8_ROW_CHICKEN2, GEN12_DISABLE_EARLY_READ);


		 // Wa_1407928979:tgl A
		NBlue::callback->wa_write_or( GEN7_FF_THREAD_MODE,
				GEN12_FF_TESSELATION_DOP_GATE_DISABLE);

		//general_render_compute_wa_init
		// Wa_1406941453:tgl,rkl,dg1,adl-s,adl-p
		NBlue::callback->wa_mcr_masked_en(
				 GEN10_SAMPLER_MODE,
				 ENABLE_SMALLPL);
		
		
		// Wa_1409804808
		NBlue::callback->wa_mcr_masked_en( GEN8_ROW_CHICKEN2,
				 GEN12_PUSH_CONST_DEREF_HOLD_DIS);

		// Wa_14010229206 /
		NBlue::callback->wa_mcr_masked_en( GEN9_ROW_CHICKEN4, GEN12_DISABLE_TDL_PUSH);
		
		// Wa_1607297627
		NBlue::callback->wa_masked_en(
				RING_PSMI_CTL(RENDER_RING_BASE),
				GEN12_WAIT_FOR_EVENT_POWER_DOWN_DISABLE |
				GEN8_RC_SEMA_IDLE_MSG_DISABLE);
		
		
		//if (GRAPHICS_VER(i915) >= 9)
		NBlue::callback->wa_masked_en(
				 GEN7_FF_SLICE_CS_CHICKEN1,
				 GEN9_FFSC_PERCTX_PREEMPT_CTRL);
		
		
		
		//tgl_whitelist_build
		//WaAllowPMDepthAndInvocationCountAccessFromUMD
		NBlue::callback->whitelist_reg_ext( PS_INVOCATION_COUNT,
				  RING_FORCE_TO_NONPRIV_ACCESS_RD |
				  RING_FORCE_TO_NONPRIV_RANGE_4);

		
		 // Wa_1808121037:tgl

		NBlue::callback->whitelist_reg( GEN7_COMMON_SLICE_CHICKEN1);

		// Wa_1806527549:tgl
		NBlue::callback->whitelist_reg( HIZ_CHICKEN);

		// Required by recommended tuning setting (not a workaround)
		NBlue::callback->whitelist_reg( GEN11_COMMON_SLICE_CHICKEN3);
		
		
		
		//intel_engine_init_ctx_wa
		//gen12_ctx_workarounds_init
		// * Wa_1409142259:tgl,dg1,adl-p
		
		NBlue::callback->wa_masked_en( GEN11_COMMON_SLICE_CHICKEN3,
									  GEN12_DISABLE_CPS_AWARE_COLOR_PIPE);
		
		/* WaDisableGPGPUMidThreadPreemption:gen12 */
		NBlue::callback->wa_masked_field_set( GEN8_CS_CHICKEN1,
											 GEN9_PREEMPT_GPGPU_LEVEL_MASK,
											 GEN9_PREEMPT_GPGPU_THREAD_GROUP_LEVEL);
		
		//* Wa_16011163337 - GS_TIMER
		NBlue::callback->wa_add(
								GEN12_FF_MODE2,
								~0,
								FF_MODE2_TDS_TIMER_128 | FF_MODE2_GS_TIMER_224,
								0, false);
		
						
	}
	
	if (ctx==1)//CCS
	{
		//return 0;
		//engine_fake_wa_init
		uint8_t mocs= 3;
		NBlue::callback->wa_masked_field_set(
					RING_CMD_CCTL(GEN12_COMPUTE0_RING_BASE),
					CMD_CCTL_MOCS_MASK,
					CMD_CCTL_MOCS_OVERRIDE(mocs, mocs));
		
		//panic("x");
	}
	
	if (ctx==2)//BCS
	{

		//gen12_ctx_gt_mocs_init
		/*table->size  = ARRAY_SIZE(tgl_mocs_table);
		table->table = tgl_mocs_table;
		table->n_entries = GEN9_NUM_MOCS_ENTRIES;
		table->uc_index = 3;*/
		uint8_t mocs;
		//if (engine->class == COPY_ENGINE_CLASS) {
			mocs = 3;
			NBlue::callback->wa_write_clr_set(
					 BLIT_CCTL(BLT_RING_BASE),//engine->mmio_base
					 BLIT_CCTL_MASK,
					 BLIT_CCTL_MOCS(mocs, mocs));
		
	}
	
	if (ctx==3)//VCS
	{
	}
	
	if (ctx==4)//VCS2
	{
	}
	
	if (ctx==5)//VECS
	{
	}
	
	
	
	return FunctionCast(IGHardwareExtendedContextinitWithOptions, callback->oIGHardwareExtendedContextinitWithOptions)(that,param_1,param_2);
}

void * Gen11::ExtendedContextWithOptions(void *param_1)
{
	return FunctionCast(ExtendedContextWithOptions, callback->oExtendedContextWithOptions)(param_1);
}

uint64_t Gen11::enableController(void *that)
{
	auto ret= FunctionCast(enableController, callback->oenableController)(that);
	return ret;
}


uint8_t Gen11::setDisplayMode(void *that,int param_1,int param_2)
{
	if (getMember<uint32_t>(that, 0x1dc)==0) getMember<uint8_t>(that, 0x1e0)=1;
	return FunctionCast(setDisplayMode, callback->osetDisplayMode)(that,param_1,param_2 );

}

uint8_t Gen11::connectionChanged(void *that)
{
	
	auto ret= FunctionCast(connectionChanged, callback->oconnectionChanged)(that);
	//getMember<uint8_t>(that, 0x1e0)=1;
	return ret;
}

unsigned long  Gen11::allocateDisplayResources(void *that)
{
	auto ret=FunctionCast(allocateDisplayResources, callback->oallocateDisplayResources)(that);
	

	return ret;
}


	
void Gen11::IGScheduler5resume(void *that) {
		
		
		
		void *accelerator = getMember<void *>(that, 0x10);
		struct IGHwCsDesc *descArray = (struct IGHwCsDesc *)callback->kIGHwCsDesc;
		uint32_t mode = _MASKED_BIT_ENABLE(GEN11_GFX_DISABLE_LEGACY_MODE);

		for (int i = 0; i < 6; i++) {
			struct IGHwCsDesc *desc = &descArray[i];

			// --- FILTER: ONLY RCS AND BCS ---
			if (desc->type == kIGHwCsTypeRCS) {
				//FunctionCast(SafeForceWake, callback->oSafeForceWake)(accelerator, true, 0);
			} else if (desc->type == kIGHwCsTypeBCS) {
				//FunctionCast(SafeForceWake, callback->oSafeForceWake)(accelerator, true, 1);
			} else {
				continue;
			}

					uint32_t ringBase = desc->mmioExecListControl - 0x3c;
					
					// USE THE CORRECT STRUCT MEMBER FOR HWS
					// The dump proves mmioGlobalStatusPage holds the offset 0x2080
					uint32_t hwsRegisterOffset = desc->mmioGlobalStatusPage;
					
					// READ SHADOW REGISTER
					// We use the offset from the struct (0x2080) to index the shadow map
					uint32_t hwsAddr = getMember<uint32_t>(accelerator, 0x1240 + hwsRegisterOffset);
					
					// WRITE TO HARDWARE
					NBlue::callback->writeReg32(hwsRegisterOffset, hwsAddr);
					NBlue::callback->readReg32(hwsRegisterOffset);

					// REST OF INIT (Using explicit offsets from struct where possible)
					NBlue::callback->writeReg32(ringBase + 0x98, ~0u); // HWSTAM
					NBlue::callback->writeReg32(desc->mmioGfxMode, mode); // GFX Mode
					NBlue::callback->writeReg32(ringBase + 0x9c, _MASKED_BIT_DISABLE(STOP_RING)); // MI Mode
					
					NBlue::callback->writeReg32(desc->mmioErrorIdentity, ~0u); // EMR
					NBlue::callback->writeReg32(desc->mmioErrorMask, ~0u);    // EIR
					NBlue::callback->writeReg32(desc->mmioErrorIdentity, ~0x1); // I915_ERROR_INSTRUCTION
		}
	
	FunctionCast(IGScheduler5resume, callback->oIGScheduler5resume)(that);
	
}

unsigned long Gen11::resetGraphicsEngine(void *that,void *param_1)
{
	
	//GT workarounds: the list of these WAs is applied whenever these registers
	//*   revert to their default values: on GPU reset, suspend/resume [1]_, etc.
	//gen12_gt_workarounds_init
	/* Wa_14011060649:tgl,rkl,dg1,adl-s,adl-p */
	//wa_14011060649(gt, wal);
	
	NBlue::callback->wa_write_or( VDBOX_CGCTL3F10(RENDER_RING_BASE), IECPUNIT_CLKGATE_DIS);
	NBlue::callback->wa_write_or( VDBOX_CGCTL3F10(BLT_RING_BASE), IECPUNIT_CLKGATE_DIS);
	NBlue::callback->wa_write_or( VDBOX_CGCTL3F10(GEN11_VEBOX_RING_BASE), IECPUNIT_CLKGATE_DIS);

	/* Wa_14011059788:tgl,rkl,adl-s,dg1,adl-p */
	NBlue::callback->wa_mcr_write_or( GEN10_DFR_RATIO_EN_AND_CHICKEN, DFR_DISABLE);

	/*
	 * Wa_14015795083
	 */
	NBlue::callback->wa_add( GEN7_MISCCPCTL, GEN12_DOP_CLOCK_GATE_RENDER_ENABLE,
		   0, 0, false);
	
	
	auto ret=FunctionCast(resetGraphicsEngine, callback->oresetGraphicsEngine)(that,param_1);
	return ret;
}

typedef enum AGDCVendorClass {
	kAGDCVendorClassReserved,
	kAGDCVendorClassIntegratedGPU,
	kAGDCVendorClassDiscreteGPU,
	kAGDCVendorClassOtherHW,
	kAGDCVendorClassOtherSW,
	kAGDCVendorClassAppleGPUPolicyManager,
	kAGDCVendorClassAppleGPUPowerManager,
	kAGDCVendorClassGPURoot,
	kAGDCVendorClassAppleGPUWrangler,
	kAGDCVendorClassAppleMuxControl,
} AGDCVendorClass_t;

typedef struct AGDCVendorInfo {
	union {
		struct {
			UInt16 Minor;
			UInt16 Major;
		};
		UInt32 Raw;
	} Version;
	char VendorString[32];
	UInt32 VendorID;
	AGDCVendorClass_t VendorClass;
} AGDCVendorInfo_t;

uint32_t
Gen11::IntelFBClientControldoAttribute
		  (void *that,uint param_1,unsigned long *param_2,unsigned long param_3,unsigned long *param_4,
		   unsigned long *param_5,void *param_6)
{

	if (param_1 == 0x928) {
		return kIOReturnUnsupported;
	}
	
	auto ret=FunctionCast(IntelFBClientControldoAttribute, callback->oIntelFBClientControldoAttribute)(that,param_1,param_2,param_3,param_4,param_5,param_6);
	
	/*if (param_1 == 1)//0x2001)
	if (param_5 != (unsigned long *)0x0)
		if (0x2b < *param_5) {
			//memset(param_4,0,0x2c);
			AGDCVendorInfo *v=(AGDCVendorInfo*)param_4;
			v->Version.Raw=0;
			v->Version.Major=0;
			v->Version.Minor=0;
			v->VendorID=0x8086;
			*v->VendorString=*(char*)"INTEL";
			v->VendorClass=kAGDCVendorClassIntegratedGPU;
			//*(mach_vm_address_t*)v=callback->IntelFBClientControl11doAttribut;
		return 0;
	}*/
	return ret;
}

int hw=1;
int Gen11::hwSetMode
		  (void *that,void *param_1,
		   void *param_2,void *param_3)
{
	//if (getMember<UInt32>(param_1, 0x4210 )==3)	ssetFramebufferPowerState(param_1,2);
	
	if (hw)
		{
			hw=0;
			//AGDCFBOnline_t i;
			//i.state=1;
			//i.id=0;
			//SetFbStatusOnNextProbe(that,&i);
			//enablePlane(getMember<void*>(param_1, 0x32c8 ),true);
			//setPanelPowerState(that,2);
			//DisplaySleepCallback(that,1,0,1);
			
			getMember<UInt32>(param_1, 0xd5c )=1;//1 fb
			
			//getMember<UInt8>(param_1, 0xe61 )=1;//lid close
			//getMember<UInt8>(param_1, 0x1e1 )=0;//newonline
			//connectionProbe(param_1);

		}
	auto ret= FunctionCast(hwSetMode, callback->ohwSetMode)(that, param_1, param_2, param_3);

	return ret;
}

void Gen11::enablePipe
		  (void *that,void *param_1,
		   void *param_2,void *param_3)
{
	return FunctionCast(enablePipe, callback->oenablePipe)(that, param_1, param_2, param_3);
}

uint8_t Gen11::beginReset(void *that)
{
	auto ret= FunctionCast(beginReset, callback->obeginReset)(that);
	
	/*static const struct intel_device_info tgl_info = {
		GEN12_FEATURES,
		PLATFORM(INTEL_TIGERLAKE),
		.platform_engine_mask =
			BIT(RCS0) | BIT(BCS0) | BIT(VECS0) | BIT(VCS0) | BIT(VCS2),
	};*/
	
	
	
	/*if (GRAPHICS_VER_FULL(dev_priv) >= IP_VER(12, 10))
		dg1_irq_reset(dev_priv);
	else if (GRAPHICS_VER(dev_priv) >= 11)
		gen11_irq_reset(dev_priv);*/
	
	//dg1_irq_reset(struct drm_i915_private *dev_priv)
	NBlue::callback->writeReg32( DG1_MSTR_TILE_INTR, 0);
	
	//gen11_irq_reset
	
	NBlue::callback->writeReg32( GEN11_GFX_MSTR_IRQ, 0);
	
	//gen11_gt_irq_reset(gt);
	// Disable RCS, BCS, VCS and VECS class engines.
	NBlue::callback->writeReg32( GEN11_RENDER_COPY_INTR_ENABLE, 0);
	NBlue::callback->writeReg32( GEN11_VCS_VECS_INTR_ENABLE,	  0);

	// Restore masks irqs on RCS, BCS, VCS and VECS engines.
	NBlue::callback->writeReg32( GEN11_RCS0_RSVD_INTR_MASK,	~0);
	NBlue::callback->writeReg32( GEN11_BCS_RSVD_INTR_MASK,	~0);
	
	NBlue::callback->writeReg32( GEN11_VCS0_VCS1_INTR_MASK,	~0);
	NBlue::callback->writeReg32( GEN11_VCS2_VCS3_INTR_MASK,	~0);
	
	//if (HAS_ENGINE(gt, VECS2) || HAS_ENGINE(gt, VECS3))
	NBlue::callback->writeReg32( GEN12_VECS2_VECS3_INTR_MASK, ~0);
	
	NBlue::callback->writeReg32( GEN11_GPM_WGBOXPERF_INTR_ENABLE, 0);
	NBlue::callback->writeReg32( GEN11_GPM_WGBOXPERF_INTR_MASK,  ~0);
	NBlue::callback->writeReg32( GEN11_GUC_SG_INTR_ENABLE, 0);
	NBlue::callback->writeReg32( GEN11_GUC_SG_INTR_MASK,  ~0);

	NBlue::callback->writeReg32( GEN11_CRYPTO_RSVD_INTR_ENABLE, 0);
	NBlue::callback->writeReg32( GEN11_CRYPTO_RSVD_INTR_MASK,  ~0);
	
	//gen11_display_irq_reset(dev_priv);
	
	NBlue::callback->writeReg32( GEN11_GFX_MSTR_IRQ, ~0);

	return ret;
}
											
void Gen11::endReset(void *that)
{
	FunctionCast(endReset, callback->oendReset)(that);
	
	//void gen11_gt_irq_postinstall(struct intel_gt *gt)
	uint32_t irqs = GT_RENDER_USER_INTERRUPT;
	uint32_t dmask,smask;
	
	dmask = irqs << 16 | irqs;
	smask = irqs << 16;
	
	
	
	// Enable RCS, BCS, VCS and VECS class interrupts.
	NBlue::callback->writeReg32( GEN11_RENDER_COPY_INTR_ENABLE, dmask);
	NBlue::callback->writeReg32( GEN11_VCS_VECS_INTR_ENABLE, dmask);

		// Unmask irqs on RCS, BCS, VCS and VECS engines.
	NBlue::callback->writeReg32( GEN11_RCS0_RSVD_INTR_MASK, ~smask);
	NBlue::callback->writeReg32( GEN11_BCS_RSVD_INTR_MASK, ~smask);

	NBlue::callback->writeReg32( GEN11_VCS0_VCS1_INTR_MASK, ~dmask);
	NBlue::callback->writeReg32( GEN11_VCS2_VCS3_INTR_MASK, ~dmask);
	
	NBlue::callback->writeReg32( GEN11_VECS0_VECS1_INTR_MASK, ~dmask);
	
		//if (HAS_ENGINE(gt, VECS2) || HAS_ENGINE(gt, VECS3))
	NBlue::callback->writeReg32( GEN12_VECS2_VECS3_INTR_MASK, ~dmask);
	
	
	//gen11_de_irq_postinstall(dev_priv);
	NBlue::callback->writeReg32( GEN11_GFX_MSTR_IRQ, GEN11_MASTER_IRQ);
	
	NBlue::callback->writeReg32( DG1_MSTR_TILE_INTR, DG1_MSTR_IRQ);
}
