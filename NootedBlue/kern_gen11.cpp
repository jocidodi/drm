
#include "kern_gen11.hpp"
#include <Headers/kern_api.hpp>
#include "Firmware.hpp"

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


//globals
void *ccont;
void *ccont2;
int bk=2;
bool kexticl=false;
bool kexttgld=false;
bool kexttglp=false;
IOFramebuffer *frame0;
int hwu=4;
int setpc=0;
void *linkp;

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
		kexticl=true;
		NBlue::callback->setRMMIOIfNecessary();
		
		SolveRequestPlus solveRequests[] = {
			{"_gPlatformInformationList", this->gPlatformInformationList},
			{"__ZN31AppleIntelFramebufferController14disableCDClockEv", this->orgDisableCDClock},
			{"__ZN31AppleIntelFramebufferController19setCDClockFrequencyEy", this->orgSetCDClockFrequency},
		};
		PANIC_COND(!SolveRequestPlus::solveAll(patcher, index, solveRequests, address, size), "nblue",	"Failed to resolve symbols");
		
		RouteRequestPlus requests[] = {
			
			{"__ZN31AppleIntelFramebufferController23initPlatformWorkaroundsEv",initPlatformWorkarounds, this->oinitPlatformWorkarounds},
			{"__ZN31AppleIntelFramebufferController16getOSInformationEv",getOSInformation2, this->ogetOSInformation2},
			{"__ZN31AppleIntelFramebufferController19getTranscoderOffsetEP14AppleIntelPortj",getTranscoderOffset, this->ogetTranscoderOffset},
			{"__ZN31AppleIntelFramebufferController14ReadRegister32Em",raReadRegister32, this->oraReadRegister32},
			{"__ZN31AppleIntelFramebufferController15WriteRegister32Emj",raWriteRegister32, this->oraWriteRegister32},
			{"__ZN21AppleIntelFramebuffer25setAttributeForConnectionEijm",wrapSetAttributeForConnection, this->owrapSetAttributeForConnection},
			{"__ZN21AppleIntelFramebuffer25getAttributeForConnectionEijPm",getAttributeForConnection, this->ogetAttributeForConnection},
			{"__ZN31AppleIntelFramebufferController13FBMemMgr_InitEv", FBMemMgr_Init,this->oFBMemMgr_Init},
			{"__ZN31AppleIntelFramebufferController9hwGetCRTCEP21AppleIntelFramebufferP21AppleIntelDisplayPath",hwGetCRTC, this->ohwGetCRTC},
			{"__ZN31AppleIntelFramebufferController21hwSetPanelPowerConfigEj", hwSetPanelPowerConfig,this->ohwSetPanelPowerConfig},
			{"__ZN31AppleIntelFramebufferController17updateSliceConfigEj",updateSliceConfig, this->oupdateSliceConfig},
			{"__ZN31AppleIntelFramebufferController18setAsyncSliceCountE13IGSliceConfig",setAsyncSliceCount, this->osetAsyncSliceCount},
			{"__ZN21AppleIntelFramebuffer18setPanelPowerStateEb",setPanelPowerState, this->osetPanelPowerState},
			{"__ZN21AppleIntelFramebuffer4initEP31AppleIntelFramebufferControllerj",AppleIntelFramebufferinit, this->oAppleIntelFramebufferinit},
			{"__ZN31AppleIntelFramebufferController21probeCDClockFrequencyEv",wrapProbeCDClockFrequency,	this->orgProbeCDClockFrequency},
			{"__ZN31AppleIntelFramebufferController18hwInitializeCStateEv",hwInitializeCState, this->ohwInitializeCState},
			{"__ZN31AppleIntelFramebufferController20hwConfigureCustomAUXEb",hwConfigureCustomAUX, this->ohwConfigureCustomAUX},
			{"__ZN17AppleIntelPortHAL4initEP10PortConfig",AppleIntelPortHALinit, this->oAppleIntelPortHALinit},
			{"__ZN31AppleIntelFramebufferController11SetupParamsEP21AppleIntelFramebufferP21AppleIntelDisplayPathP10CRTCParamsPK29IODetailedTimingInformationV2",SetupParams,	this->oSetupParams},
			{"__ZN31AppleIntelFramebufferController19setupPipeWatermarksEP21AppleIntelFramebufferP21AppleIntelDisplayPathP10CRTCParams",setupPipeWatermarks, this->osetupPipeWatermarks},
			{"__ZN15AppleIntelPlane10setupPlaneEP21AppleIntelDisplayPathi",setupPlane, this->osetupPlane},
			
			{"__ZN14AppleIntelPort12linkTrainingEP18AGDCDPPortConfig_t",linkTraining, this->olinkTraining},
			{"__ZN14AppleIntelPort8writeAUXEjPvj",writeAUX, this->owriteAUX},
			{"__ZN14AppleIntelPort7readAUXEjPvj",readAUX, this->oreadAUX},
			
			//{"__ZN31AppleIntelFramebufferController16hwRegsNeedUpdateEP21AppleIntelFramebufferP21AppleIntelDisplayPathP10CRTCParamsPK29IODetailedTimingInformationV2PN16AppleIntelScaler12SCALERPARAMSE",hwRegsNeedUpdate, this->ohwRegsNeedUpdate},
			/*{"__ZN21AppleIntelFramebuffer31frameBufferNotificationcallbackEP8OSObjectPvP13IOFramebufferiS2_",aframeBufferNotificationcallback, this->oaframeBufferNotificationcallback},
			{"__ZN31AppleIntelFramebufferController9hwSetModeEP21AppleIntelFramebufferP21AppleIntelDisplayPathiPK29IODetailedTimingInformationV2",hwSetMode, this->ohwSetMode},*/
			
			//{"__ZN21AppleIntelFramebuffer12setAttributeEjm",fsetAttribute, this->ofsetAttribute},
			
			
		};
		PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route symbols");
		
		
		//pipe 0 fix
		static const uint8_t f7[]= {0x83, 0x78, 0x08, 0x00, 0x0f, 0x84, 0x32, 0x01, 0x00, 0x00};
		static const uint8_t r7[]= {0x83, 0x78, 0x08, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
		
		static const uint8_t f7a[]= {0x80, 0x7d, 0xd7, 0x00, 0x75, 0x23};
		static const uint8_t r7a[]= {0x80, 0x7d, 0xd7, 0x00, 0xeb, 0x23};
		
		//builtin
		static const uint8_t f9[]= {0x48, 0x8b, 0xb8, 0x40, 0x04, 0x00, 0x00, 0xf6, 0x47, 0x14, 0x08, 0x75, 0x0a};
		static const uint8_t r9[]= {0x48, 0x8b, 0xb8, 0x40, 0x04, 0x00, 0x00, 0xf6, 0x47, 0x14, 0x08, 0xeb, 0x0a};
		
		//getHPDState register
		static const uint8_t f19[]= {0xbe, 0xa0, 0x38, 0x16, 0x00};
		static const uint8_t r19[]= {0xbe, 0x70, 0x44, 0x04, 0x00};
		
		//register adresses
		static const uint8_t f24b[]= {0xbe, 0x40, 0xf8, 0x06, 0x00};
		static const uint8_t r24b[]= {0xbe, 0x40, 0x08, 0x06, 0x00};
				
		static const uint8_t f24c[]= {0x48, 0x81, 0xfb, 0x00, 0xf8, 0x06, 0x00};
		static const uint8_t r24c[]= {0x48, 0x81, 0xfb, 0x00, 0x08, 0x06, 0x00};
				
		static const uint8_t f24d[]= {0xbe, 0x60, 0xf8, 0x06, 0x00};
		static const uint8_t r24d[]= {0xbe, 0x60, 0x08, 0x06, 0x00};
				
		static const uint8_t f24e[]= {0xbe, 0x00, 0xf8, 0x06, 0x00};
		static const uint8_t r24e[]= {0xbe, 0x00, 0x08, 0x06, 0x00};
		
		//linktrainig 2 lines
		static const uint8_t f25[]= {0x77, 0x77, 0x00, 0x00};
		static const uint8_t r25[]= {0x77, 0x00, 0x00, 0x00};
		
		
		
		LookupPatchPlus const patches[] = {
			{&kextG11FB, f7, r7, arrsize(f7),    1},
			{&kextG11FB, f7a, r7a, arrsize(f7a),    1},
			{&kextG11FB, f9, r9, arrsize(f9),    1},
			{&kextG11FB, f19, r19, arrsize(f19),    5},
			{&kextG11FB, f24b, r24b, arrsize(f24b),    12},
			{&kextG11FB, f24c, r24c, arrsize(f24c),    1},
			{&kextG11FB, f24d, r24d, arrsize(f24d),    10},
			{&kextG11FB, f24e, r24e, arrsize(f24e),    28},
			{&kextG11FB, f25, r25, arrsize(f25),    7},
			
			
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
		kexttgld=!isprod;
		kexttglp=isprod;
		
		if (isprod) {
			SolveRequestPlus solveRequests[] = {
				{"_gPlatformInformationList", this->gPlatformInformationList},
				{"__ZN31AppleIntelFramebufferController14disableCDClockEv", this->orgDisableCDClock},
				{"__ZN31AppleIntelFramebufferController19setCDClockFrequencyEy", this->orgSetCDClockFrequency},
			};
			PANIC_COND(!SolveRequestPlus::solveAll(patcher, index, solveRequests, address, size), "nblue",	"Failed to resolve symbols");
		}
		else
		{
			SolveRequestPlus solveRequests[] = {
				{"_gPlatformInformationList", this->gPlatformInformationList},
				{"__ZN24AppleIntelBaseController14disableCDClockEv", this->orgDisableCDClock},
				{"__ZN24AppleIntelBaseController19setCDClockFrequencyEy", this->orgSetCDClockFrequency},
			};
			PANIC_COND(!SolveRequestPlus::solveAll(patcher, index, solveRequests, address, size), "nblue",	"Failed to resolve symbols");
		}

		
		RouteRequestPlus requests[] = {
			{"__ZN16AppleIntelScaler4initE10IGScalerID", AppleIntelScalerinit,this->oAppleIntelScalerinit},
			{"__ZN15AppleIntelPlane4initE9IGPlaneID", AppleIntelPlaneinit,this->oAppleIntelPlaneinit},
			{"__ZN31AppleIntelRegisterAccessManager14ReadRegister32Em",raReadRegister32, this->oraReadRegister32},
			{"__ZN31AppleIntelRegisterAccessManager15WriteRegister32Emj",raWriteRegister32, this->oraWriteRegister32},
			{"__ZN21AppleIntelFramebuffer25setAttributeForConnectionEijm",wrapSetAttributeForConnection, this->owrapSetAttributeForConnection},
			{"__ZN21AppleIntelFramebuffer25getAttributeForConnectionEijPm",getAttributeForConnection, this->ogetAttributeForConnection},
			//{"__ZN21AppleIntelFramebuffer12setAttributeEjm",fsetAttribute, this->ofsetAttribute},
			{"__ZN26AppleIntelDSBAccessManager13isDSBRegisterEj", dozero},
			{"__ZN15AppleIntelPlane10setupPlaneEP21AppleIntelDisplayPath",setupPlane2, this->osetupPlane2},
			{"__ZN14AppleIntelPort12linkTrainingEP18AGDCDPPortConfig_t",linkTraining, this->olinkTraining},
			{"__ZN14AppleIntelPort8writeAUXEjPvj",writeAUX, this->owriteAUX},
			{"__ZN14AppleIntelPort7readAUXEjPvj",readAUX, this->oreadAUX},
			
		};
		PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route dp symbols");
		
		if (isprod) {
			RouteRequestPlus requests[] = {
				//{"__ZN31AppleIntelFramebufferController16hwRegsNeedUpdateEP21AppleIntelFramebufferP21AppleIntelDisplayPathP10CRTCParamsPK29IODetailedTimingInformationV2PN16AppleIntelScaler12SCALERPARAMSE",hwRegsNeedUpdate, this->ohwRegsNeedUpdate},
				{"__ZN31AppleIntelFramebufferController15hwSetPanelPowerEj",hwSetPanelPower, this->ohwSetPanelPower},
				{"__ZN31AppleIntelFramebufferController11SetupParamsEP21AppleIntelFramebufferP21AppleIntelDisplayPathP10CRTCParamsPK29IODetailedTimingInformationV2",SetupParams,	this->oSetupParams},
				{"__ZN31AppleIntelFramebufferController19setupPipeWatermarksEP21AppleIntelFramebufferP21AppleIntelDisplayPathP10CRTCParams",setupPipeWatermarks, this->osetupPipeWatermarks},
				{"__ZN31AppleIntelFramebufferController21probeCDClockFrequencyEv",wrapProbeCDClockFrequency,	this->orgProbeCDClockFrequency},
				{"__ZN31AppleIntelFramebufferController17updateSliceConfigEj",updateSliceConfig, this->oupdateSliceConfig},
				{"__ZN31AppleIntelFramebufferController18setAsyncSliceCountE13IGSliceConfig",setAsyncSliceCount, this->osetAsyncSliceCount},
				{"__ZN31AppleIntelFramebufferController9hwGetCRTCEP21AppleIntelFramebufferP21AppleIntelDisplayPath",hwGetCRTC, this->ohwGetCRTC},
				{"__ZN31AppleIntelFramebufferController21hwSetPanelPowerConfigEj", hwSetPanelPowerConfig,this->ohwSetPanelPowerConfig},
				{"__ZN31AppleIntelFramebufferController15enableVDDForAuxEP14AppleIntelPort",enableVDDForAux, this->oenableVDDForAux},
				{"__ZN31AppleIntelFramebufferController16disableVDDForAuxEv",disableVDDForAux, this->odisableVDDForAux},
				{"__ZN21AppleIntelFramebuffer4initEP31AppleIntelFramebufferControllerj",AppleIntelFramebufferinit, this->oAppleIntelFramebufferinit},
				{"__ZN31AppleIntelFramebufferController13FBMemMgr_InitEv", FBMemMgr_Init,this->oFBMemMgr_Init},
				{"__ZN31AppleIntelFramebufferController23initPlatformWorkaroundsEv",initPlatformWorkarounds, this->oinitPlatformWorkarounds},
				{"__ZN31AppleIntelFramebufferController16getOSInformationEv",getOSInformation, this->ogetOSInformation},
				{"__ZN31AppleIntelFramebufferController18hwInitializeCStateEv",hwInitializeCState, this->ohwInitializeCState},
				{"__ZN31AppleIntelFramebufferController20hwConfigureCustomAUXEb",hwConfigureCustomAUX, this->ohwConfigureCustomAUX},
				
			};
			PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route p symbols");
			
		} else //debug version
		{
			RouteRequestPlus requests[] = {
				//{"__ZN24AppleIntelBaseController16hwRegsNeedUpdateEP21AppleIntelFramebufferP21AppleIntelDisplayPathP10CRTCParamsPK29IODetailedTimingInformationV2PN16AppleIntelScaler12SCALERPARAMSE",hwRegsNeedUpdate, this->ohwRegsNeedUpdate},
				{"__ZN24AppleIntelBaseController15hwSetPanelPowerEj",hwSetPanelPower, this->ohwSetPanelPower},
				{"__ZN24AppleIntelBaseController11SetupParamsEP21AppleIntelFramebufferP21AppleIntelDisplayPathP10CRTCParamsPK29IODetailedTimingInformationV2",SetupParams,	this->oSetupParams},
				{"__ZN24AppleIntelBaseController19setupPipeWatermarksEP21AppleIntelFramebufferP21AppleIntelDisplayPathP10CRTCParams",setupPipeWatermarks, this->osetupPipeWatermarks},
				{"__ZN24AppleIntelBaseController21probeCDClockFrequencyEv",wrapProbeCDClockFrequency,	this->orgProbeCDClockFrequency},
				{"__ZN24AppleIntelBaseController17updateSliceConfigEj",updateSliceConfig, this->oupdateSliceConfig},
				{"__ZN24AppleIntelBaseController18setAsyncSliceCountE13IGSliceConfig",setAsyncSliceCount, this->osetAsyncSliceCount},
				{"__ZN24AppleIntelBaseController9hwGetCRTCEP21AppleIntelFramebufferP21AppleIntelDisplayPath",hwGetCRTC, this->ohwGetCRTC},
				{"__ZN24AppleIntelBaseController21hwSetPanelPowerConfigEj", hwSetPanelPowerConfig,this->ohwSetPanelPowerConfig},
				{"__ZN24AppleIntelBaseController15enableVDDForAuxEP14AppleIntelPort",enableVDDForAux, this->oenableVDDForAux},
				{"__ZN24AppleIntelBaseController16disableVDDForAuxEv",disableVDDForAux, this->odisableVDDForAux},
				{"__ZN21AppleIntelFramebuffer4initEP24AppleIntelBaseControllerj",AppleIntelFramebufferinit, this->oAppleIntelFramebufferinit},
				{"__ZN24AppleIntelBaseController13FBMemMgr_InitEv", FBMemMgr_Init,this->oFBMemMgr_Init},
				{"__ZN24AppleIntelBaseController23initPlatformWorkaroundsEv",initPlatformWorkarounds, this->oinitPlatformWorkarounds},
				{"__ZN24AppleIntelBaseController16getOSInformationEv",getOSInformation, this->ogetOSInformation},
				{"__ZN24AppleIntelBaseController18hwInitializeCStateEv",hwInitializeCState, this->ohwInitializeCState},
				{"__ZN24AppleIntelBaseController20hwConfigureCustomAUXEb",hwConfigureCustomAUX, this->ohwConfigureCustomAUX},
				
			};
			PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route d symbols");
			
		}
		
		
		//dbuff
		static const uint8_t f5[]= {0x74, 0x5a, 0xc7, 0x83, 0x48, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		static const uint8_t r5[]= {0x90, 0x90, 0xc7, 0x83, 0x48, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		
		
		//ReadRegister64
		static const uint8_t f7[]= {0x83, 0xc0, 0xfc, 0x48, 0x39, 0xf0, 0x76, 0x11, 0x48, 0x8b, 0x47, 0x50, 0x48, 0xff, 0x05, 0xca, 0xf5, 0x0c, 0x00};
		static const uint8_t r7[]= {0x83, 0xc0, 0xf8, 0x48, 0x39, 0xf0, 0x76, 0x11, 0x48, 0x8b, 0x47, 0x50, 0x48, 0xff, 0x05, 0xca, 0xf5, 0x0c, 0x00};
		
		static const uint8_t f7p[]= {0x83, 0xc0, 0xfc, 0x48, 0x39, 0xf0, 0x76, 0x11, 0x48, 0x8b, 0x47, 0x50, 0x48, 0xff, 0x05, 0x84, 0x40, 0x08, 0x00};
		static const uint8_t r7p[]= {0x83, 0xc0, 0xf8, 0x48, 0x39, 0xf0, 0x76, 0x11, 0x48, 0x8b, 0x47, 0x50, 0x48, 0xff, 0x05, 0x84, 0x40, 0x08, 0x00};
		
		//builtin
		static const uint8_t f9[]= {0xf6, 0x40, 0x14, 0x08, 0x75, 0x0d};
		static const uint8_t r9[]= {0xf6, 0x40, 0x14, 0x08, 0xeb, 0x0d};
		
		static const uint8_t f9p[]= {0xf6, 0x40, 0x14, 0x08, 0x75, 0x0a};
		static const uint8_t r9p[]= {0xf6, 0x40, 0x14, 0x08, 0xeb, 0x0a};
		
		//probeportmode hookcase fix
		static const uint8_t f13[]= {0xff, 0x91, 0x90, 0x01, 0x00, 0x00, 0x83, 0xf8, 0x02, 0x0f, 0x84, 0xec, 0x00, 0x00, 0x00};
		static const uint8_t r13[]= {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
		
		static const uint8_t f13b[]= {0xff, 0x90, 0x90, 0x01, 0x00, 0x00, 0x49, 0x8b, 0x0e, 0x4c, 0x89, 0xf7, 0x89, 0xc6, 0xff, 0x91, 0x38, 0x01, 0x00, 0x00};
		static const uint8_t r13b[]= {0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x49, 0x8b, 0x0e, 0x4c, 0x89, 0xf7, 0x89, 0xc6, 0xff, 0x91, 0x38, 0x01, 0x00, 0x00};

		//probeportmode hookcase fix
		static const uint8_t f13p[]= {0xff, 0x91, 0x78, 0x01, 0x00, 0x00, 0x83, 0xf8, 0x02, 0x74, 0x64};
		static const uint8_t r13p[]= {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
		
		static const uint8_t f13pb[]= {0xff, 0x90, 0x78, 0x01, 0x00, 0x00, 0x49, 0x8b, 0x0e, 0x4c, 0x89, 0xf7, 0x89, 0xc6, 0xff, 0x91, 0x38, 0x01, 0x00, 0x00};
		static const uint8_t r13pb[]= {0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x49, 0x8b, 0x0e, 0x4c, 0x89, 0xf7, 0x89, 0xc6, 0xff, 0x91, 0x38, 0x01, 0x00, 0x00};

		//getPathByPipe disable logs
		static const uint8_t f15[]= {0x74, 0x36, 0x48, 0xff, 0x05, 0x7e, 0x51, 0x08, 0x00, 0x44, 0x89, 0x3c, 0x24, 0x48, 0x8d, 0x15, 0x4d, 0x88, 0x03, 0x00, 0x4c, 0x8d, 0x05, 0x28, 0x8a, 0x03, 0x00};
		static const uint8_t r15[]= {0xeb, 0x36, 0x48, 0xff, 0x05, 0x7e, 0x51, 0x08, 0x00, 0x44, 0x89, 0x3c, 0x24, 0x48, 0x8d, 0x15, 0x4d, 0x88, 0x03, 0x00, 0x4c, 0x8d, 0x05, 0x28, 0x8a, 0x03, 0x00};
		
		//savenvram
		static const uint8_t f20[]= {0xff, 0x90, 0xf8, 0x09, 0x00, 0x00, 0x41, 0x89, 0xc6, 0x48, 0x85, 0xdb, 0x74, 0x17};
		static const uint8_t r20[]= {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x48, 0x85, 0xdb, 0x74, 0x17};
		
		static const uint8_t f20p[]= {0xff, 0x90, 0xf8, 0x09, 0x00, 0x00, 0x41, 0x89, 0xc6, 0x48, 0x85, 0xdb, 0x74, 0x17};
		static const uint8_t r20p[]= {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x48, 0x85, 0xdb, 0x74, 0x17};

		//production ver fix register adresses if pipe=0
		static const uint8_t f24bp[]= {0x83, 0x78, 0x08, 0x00, 0x75, 0x0c};
		static const uint8_t r24bp[]= {0x83, 0x78, 0x08, 0x00, 0xeb, 0x0c};
				
		static const uint8_t f24cp[]= {0x00, 0x4c, 0x89, 0xea, 0x75, 0x12};
		static const uint8_t r24cp[]= {0x00, 0x4c, 0x89, 0xea, 0xeb, 0x12};
				
		static const uint8_t f24dp[]= {0x83, 0x78, 0x08, 0x00, 0x75, 0x0d};
		static const uint8_t r24dp[]= {0x83, 0x78, 0x08, 0x00, 0xeb, 0x0d};
				
		//debug ver fix register adresses if pipe=0
		static const uint8_t f24b[]= {0x83, 0x78, 0x08, 0x00, 0x75, 0x0c};
		static const uint8_t r24b[]= {0x83, 0x78, 0x08, 0x00, 0xeb, 0x0c};
				
		static const uint8_t f24c[]= {0x48, 0x8b, 0x55, 0xd0, 0x75, 0x13};
		static const uint8_t r24c[]= {0x48, 0x8b, 0x55, 0xd0, 0xeb, 0x13};
				
		static const uint8_t f24d[]= {0x83, 0x78, 0x08, 0x00, 0x75, 0x0d};
		static const uint8_t r24d[]= {0x83, 0x78, 0x08, 0x00, 0xeb, 0x0d};
		
		//linktrainig 2 lines
		static const uint8_t f25[]= {0x77, 0x77, 0x00, 0x00};
		static const uint8_t r25[]= {0x33, 0x00, 0x00, 0x00};



		
		if (isprod){
			LookupPatchPlus const patchesp[] = {// tgl production kext
				{&kextG11FBT, f5, r5, arrsize(f5),	1},
				{&kextG11FBT, f7p, r7p, arrsize(f7p),	1},
				{&kextG11FBT, f9p, r9p, arrsize(f9p),	1},
				{&kextG11FBT, f13p, r13p, arrsize(f13p),	1},
				{&kextG11FBT, f13pb, r13pb, arrsize(f13pb),	1},
				{&kextG11FBT, f20p, r20p, arrsize(f20p),	1},
				{&kextG11FBT, f24bp, r24bp, arrsize(f24bp),	14},
				{&kextG11FBT, f24cp, r24cp, arrsize(f24cp),	1},
				{&kextG11FBT, f24dp, r24dp, arrsize(f24dp),	4},
				{&kextG11FBT, f25, r25, arrsize(f25),	6},

			};
			PANIC_COND(!LookupPatchPlus::applyAll(patcher, patchesp , address, size), "nblue", "kextG11FBT Failed to apply production patches!");
		}
		else {
			LookupPatchPlus const patches[] = {// tgl debug kext
				{&kextG11FBT, f5, r5, arrsize(f5),	1},
				{&kextG11FBT, f7, r7, arrsize(f7),	1},
				{&kextG11FBT, f9, r9, arrsize(f9),	1},
				{&kextG11FBT, f13, r13, arrsize(f13),	1},
				{&kextG11FBT, f13b, r13b, arrsize(f13b),	1},
				{&kextG11FBT, f15, r15, arrsize(f15),	1},
				{&kextG11FBT, f20, r20, arrsize(f20),	1},
				{&kextG11FBT, f24b, r24b, arrsize(f24b),	11},
				{&kextG11FBT, f24c, r24c, arrsize(f24c),	1},
				{&kextG11FBT, f24d, r24d, arrsize(f24d),	6},
				{&kextG11FBT, f25, r25, arrsize(f25),	6},

			};
			PANIC_COND(!LookupPatchPlus::applyAll(patcher, patches , address, size), "nblue", "kextG11FBT Failed to apply dbg patches!");
		}
		return true;
		
	}     else if (kextG11HW.loadIndex == index) {
		NBlue::callback->setRMMIOIfNecessary();
		 
		 RouteRequestPlus requests[] = {
			 
			 {"__ZN16IntelAccelerator19PAVPCommandCallbackE22PAVPSessionCommandID_tjPjb", wrapPavpSessionCallback, this->orgPavpSessionCallback},
			 {"__ZL27ContextStatusBufferValidateRK15IGHwCsExecList5PK28SGfxContextStatusBufferEntry.cold.2", dovoid},
			 {"__ZN16IntelAccelerator18setAsyncSliceCountE13IGSliceConfig",setAsyncSliceCount2, this->osetAsyncSliceCount2},
			 {"__ZN16IntelAccelerator14setSliceConfigE13IGSliceConfig",setSliceConfig, this->osetSliceConfig},
			 
		 };
		PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route symbols");
		
		//sku = 8
		static const uint8_t f2[] = {0x41, 0xc1, 0xef, 0x1c, 0x44, 0x89, 0xbb, 0x50, 0x11, 0x00, 0x00};
		static const uint8_t r2[] = {0xc7, 0x83, 0x50, 0x11, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x90};
		//8 subslices
		static const uint8_t f2a[] = {0x44, 0x89, 0xf8, 0xf7, 0xd0, 0xf3, 0x0f, 0xb8, 0xf0, 0x89, 0xb3, 0x88, 0x11, 0x00, 0x00};
		static const uint8_t r2a[] = {0xc7, 0x83, 0x88, 0x11, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90};
		//ringmask
		static const uint8_t f2b[] = {0x80, 0x8b, 0x40, 0x13, 0x00, 0x00, 0x1f};
		static const uint8_t r2b[] = {0x80, 0x8b, 0x40, 0x13, 0x00, 0x00, 0x3f};
		
		LookupPatchPlus const patches[] = {
			{&kextG11HW, f2, r2, arrsize(f2),	1},
			{&kextG11HW, f2a, r2a, arrsize(f2a),	1},
			{&kextG11HW, f2b, r2b, arrsize(f2b),	1},
		};
		PANIC_COND(!LookupPatchPlus::applyAll(patcher, patches , address, size), "nblue", "kextG11HW Failed to apply patches!");

		return true;
		
		
    } else if (kextG11HWT.loadIndex == index || kextG11HWTe.loadIndex == index) {
		NBlue::callback->setRMMIOIfNecessary();
		auto kext=kextG11HWT.loadIndex == index ? kextG11HWT: kextG11HWTe;
		
		SolveRequestPlus solveRequests[] = {
			//apple rings control variable -> 11 globals
			{"__ZL11kIGHwCsDesc", this->kIGHwCsDesc},
			
		};
		//PANIC_COND(!SolveRequestPlus::solveAll(patcher, index, solveRequests, address, size), "nblue",	"Failed to resolve symbols");
		 
		
		 RouteRequestPlus requests[] = {
			 
			 {"__ZN16IntelAccelerator19PAVPCommandCallbackE22PAVPSessionCommandID_tjPjb", wrapPavpSessionCallback, this->orgPavpSessionCallback},
			 {"__ZN16IntelAccelerator18setAsyncSliceCountE13IGSliceConfig",setAsyncSliceCount2, this->osetAsyncSliceCount2},
			 {"__ZN16IntelAccelerator14setSliceConfigE13IGSliceConfig",setSliceConfig, this->osetSliceConfig},
			 
			 
		 };
		PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route symbols");
		
		// 8 subslices
		static const uint8_t f2[] = {0x8b, 0x5d, 0xd0, 0xf3, 0x0f, 0xb8, 0xf3, 0x01, 0xf6, 0x41, 0x89, 0xb7, 0x58, 0x11, 0x00, 0x00};
		static const uint8_t r2[] = {0x8b, 0x5d, 0xd0, 0x41, 0xc7, 0x87, 0x58, 0x11, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x90, 0x90};
		
		// 8 MaxEUPerSubSlice
		static const uint8_t f2a[] = {0xbe, 0x08, 0x00, 0x00, 0x00, 0x29, 0xde, 0x41, 0x89, 0xb7, 0x6c, 0x11, 0x00, 0x00};
		static const uint8_t r2a[] = {0xbe, 0x08, 0x00, 0x00, 0x00, 0x90, 0x90, 0x41, 0x89, 0xb7, 0x6c, 0x11, 0x00, 0x00};
		
		//sku =2  getGPUInfo
		static const uint8_t f3[] = {0x8b, 0x3e, 0x81, 0xff, 0xee, 0xbe, 0xaf, 0xde, 0x7f, 0x15, 0x81, 0xff, 0x86, 0x80, 0x40, 0x9a, 0x74, 0x2d};
		static const uint8_t r3[] = {0x8b, 0x3e, 0x81, 0xff, 0xee, 0xbe, 0xaf, 0xde, 0x90, 0x90, 0x81, 0xff, 0x86, 0x80, 0x40, 0x9a, 0xeb, 0x2d};
		
		// 12 to 8 subslices + L3BankCount = 8
		static const uint8_t f3a[] = {0x83, 0xfe, 0x01, 0x75, 0x59, 0x83, 0xfa, 0x0c};
		static const uint8_t r3a[] = {0x83, 0xfe, 0x01, 0x75, 0x59, 0x83, 0xfa, 0x08};
		
		
		//blit3d mem align patch
		static const uint8_t f5[] = {0x40, 0xd2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		static const uint8_t r5[] = {0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		
		
			LookupPatchPlus const patches[] = {
				{&kext, f2, r2, arrsize(f2),	1},
				{&kext, f2a, r2a, arrsize(f2a),	1},
				{&kext, f3, r3, arrsize(f3),	1},
				{&kext, f3a, r3a, arrsize(f3a),	1},
				{&kext, f5, r5, arrsize(f5),	1},
				
			};
			PANIC_COND(!LookupPatchPlus::applyAll(patcher, patches , address, size), "nblue", "kextG11HWT Failed to apply patches!");

		
		DBGLOG("nblue", "Loaded AppleIntelTGLGraphics!");

		return true;
	}

    return false;
}




void  Gen11::initPlatformWorkarounds(void *that)
{
	if (kexticl)
	{
		//PlatformWorkarounds
		getMember<volatile uint32_t>(that, 0xc1c)=
	 /*FB_FLAG_ENABLE_SLICE_FEATURES|FB_FLAG_ENABLE_BACKLIGHT_REG_CONTROL|FB_FLAG_LIMIT_4K_SOURCE_SIZE|*/
		/*FB_FLAG_DISABLE_FEATURE_IPS|FB_FLAG_ALTERNATE_PWM_INCREMENT2|
		FB_FLAG_ALTERNATE_PWM_INCREMENT1|FB_FLAG_DISABLE_HIGH_BITRATE_MODE2|*/
		/*FB_FLAG_FORCE_POWER_ALWAYS_CONNECTED|*/FB_FLAG_AVOID_FAST_LINK_TRAINING;
		
		//ig boot flags
		getMember<volatile uint32_t>(that, 0xc10)=/*FB_FLAG_DISABLE_HIGH_BITRATE_MODE2|*/FB_FLAG_BOOST_PIXEL_FREQUENCY_LIMIT;
		
	}
	else
	{
		//PlatformWorkarounds
		getMember<volatile uint32_t>(that, 0xc5c)=
		/*FB_FLAG_ALTERNATE_PWM_INCREMENT1|FB_FLAG_ENABLE_SLICE_FEATURES|*/
		/*FB_FLAG_FORCE_POWER_ALWAYS_CONNECTED|*/FB_FLAG_AVOID_FAST_LINK_TRAINING/*| FB_FLAG_ENABLE_BACKLIGHT_REG_CONTROL|FB_FLAG_LIMIT_4K_SOURCE_SIZE*/;
		
		//ig boot flags
		getMember<volatile uint32_t>(that, 0xc58)=/*FB_FLAG_ENABLE_BACKLIGHT_REG_CONTROL|*/FB_FLAG_BOOST_PIXEL_FREQUENCY_LIMIT;
	}
}

uint64_t  Gen11::getOSInformation2(void *that)
{
	if (NBlue::callback->intel_opregion_setup()!=0) panic("BAD BIOS");
	
	struct FramebufferICLLP *pinfo =static_cast<FramebufferICLLP *>(callback->gPlatformInformationList);
	int p=0x6;
	
	pinfo[p].flags=
	FB_FLAG_DISABLE_PIPE_SCRAMBLE|/*FB_FLAG_ALLOW_CONNECTOR_RECOVER|FB_FLAG_ENABLE_DITHERING|
	FB_FLAG_LIMIT_4K_SOURCE_SIZE|FB_FLAG_BOOST_PIXEL_FREQUENCY_LIMIT|*/
	/*FB_FLAG_FRAMEBUFFER_COMPRESSION|FB_FLAG_ENABLE_BACKLIGHT_REG_CONTROL|*/FB_FLAG_AVOID_FAST_LINK_TRAINING;
	
	
		pinfo[p].camelliaVersion=0;
	
		pinfo[p].fMobile=1;
		pinfo[p].fPipeCount=3;
		pinfo[p].fPortCount=3;
		pinfo[p].fInfoFramebufferCount=2;

		pinfo[p].fSliceCount=1;
		pinfo[p].fEuCount=8;
		pinfo[p].fSubSliceCount=8;
	
	//pinfo[p].fVideoTurboFreq=270000000;
	
	
	for (int i = 0; i < 6; i++) {
		pinfo[p].connectors[i].index=NBlue::callback->display_base.bconnectors[i].index;
		pinfo[p].connectors[i].busId=NBlue::callback->display_base.bconnectors[i].busId;
		pinfo[p].connectors[i].pipe=NBlue::callback->display_base.bconnectors[i].pipe;
		pinfo[p].connectors[i].pad=NBlue::callback->display_base.bconnectors[i].pad;
		pinfo[p].connectors[i].type=NBlue::callback->display_base.bconnectors[i].type;
		pinfo[p].connectors[i].flags=NBlue::callback->display_base.bconnectors[i].flags;
	}
	
	pinfo[p].connectors[0].pipe=1; // bad hack
	
	OSArray *connectorArray = OSArray::withCapacity(6);
	for (int i = 0; i < 6; i++) {
		OSDictionary *connectorDict = OSDictionary::withCapacity(10);
		connectorDict->setObject("Index", OSNumber::withNumber(pinfo[p].connectors[i].index, 32));
		connectorDict->setObject("busId", OSNumber::withNumber(pinfo[p].connectors[i].busId, 32));
		connectorDict->setObject("pipe", OSNumber::withNumber(pinfo[p].connectors[i].pipe, 32));
		connectorDict->setObject("pad", OSNumber::withNumber(pinfo[p].connectors[i].pad, 32));
		connectorDict->setObject("type", OSNumber::withNumber(pinfo[p].connectors[i].type, 32));
		connectorDict->setObject("flags", OSNumber::withNumber(pinfo[p].connectors[i].flags, 32));
		connectorArray->setObject(connectorDict);
		connectorDict->release();
	}
	NBlue::callback->iGPU->setProperty("Driver_Connectors", connectorArray);
	connectorArray->release();
	
	auto ret=FunctionCast(getOSInformation2, callback->ogetOSInformation2)(that );
	return ret;
}

uint64_t  Gen11::getOSInformation(void *that)
{
	
	if (NBlue::callback->intel_opregion_setup()!=0) panic("BAD BIOS");
	
	struct PlatformInfo *pinfo =static_cast<PlatformInfo *>(callback->gPlatformInformationList);
	
	int p=1;
	pinfo[p].fInfoFlags=
	FB_FLAG_DISABLE_PIPE_SCRAMBLE/*|FB_FLAG_FRAMEBUFFER_COMPRESSION|FB_FLAG_ENABLE_BACKLIGHT_REG_CONTROL*/
	/*|FB_FLAG_FORCE_POWER_ALWAYS_CONNECTED*/|FB_FLAG_AVOID_FAST_LINK_TRAINING/*|FB_FLAG_ALLOW_CONNECTOR_RECOVER
	|FB_FLAG_USE_VIDEO_TURBO|FB_FLAG_ALTERNATE_PWM_INCREMENT2*/;
	
	
		pinfo[p].cameliav=0;
		//CamelliaTcon2=2 BanksiaTcon=3
	
		pinfo[p].fMobile=1;
		pinfo[p].fPipeCount=3;
		pinfo[p].fInfoPortCount=3;
		pinfo[p].fInfoFramebufferCount=2;

		pinfo[p].fSliceCount=1;
		pinfo[p].fmaxEuCount=8;
		pinfo[p].fsubslices=8;
	
	//pinfo[p].fInfoFBCompressionMemorySize=	0xB6D000;
	//pinfo[p].fVideoTurboFreq=270000000;
	
	
	for (int i = 0; i < 6; i++) {
		pinfo[p].connectors[i].index=NBlue::callback->display_base.bconnectors[i].index;
		pinfo[p].connectors[i].busId=NBlue::callback->display_base.bconnectors[i].busId;
		pinfo[p].connectors[i].pipe=NBlue::callback->display_base.bconnectors[i].pipe;
		pinfo[p].connectors[i].pad=NBlue::callback->display_base.bconnectors[i].pad;
		pinfo[p].connectors[i].type=NBlue::callback->display_base.bconnectors[i].type;
		pinfo[p].connectors[i].flags=NBlue::callback->display_base.bconnectors[i].flags;
	}
	
	pinfo[p].connectors[0].flags=0x1+0x8+0x10; // bad hack
	
	OSArray *connectorArray = OSArray::withCapacity(6);
	for (int i = 0; i < 6; i++) {
		OSDictionary *connectorDict = OSDictionary::withCapacity(10);
		connectorDict->setObject("Index", OSNumber::withNumber(pinfo[p].connectors[i].index, 32));
		connectorDict->setObject("busId", OSNumber::withNumber(pinfo[p].connectors[i].busId, 32));
		connectorDict->setObject("pipe", OSNumber::withNumber(pinfo[p].connectors[i].pipe, 32));
		connectorDict->setObject("pad", OSNumber::withNumber(pinfo[p].connectors[i].pad, 32));
		connectorDict->setObject("type", OSNumber::withNumber(pinfo[p].connectors[i].type, 32));
		connectorDict->setObject("flags", OSNumber::withNumber(pinfo[p].connectors[i].flags, 32));
		connectorArray->setObject(connectorDict);
		connectorDict->release();
	}
	NBlue::callback->iGPU->setProperty("Driver_Connectors", connectorArray);
	connectorArray->release();
	
	auto ret=FunctionCast(getOSInformation, callback->ogetOSInformation)(that );
	return ret;
}



IOReturn Gen11::wrapPavpSessionCallback( void *intelAccelerator, int32_t sessionCommand, uint32_t sessionAppId, uint32_t *a4, bool flag) {
	
	if (sessionCommand == 4) {
		return kIOReturnTimeout;
	}

	return FunctionCast(wrapPavpSessionCallback, callback->orgPavpSessionCallback)(intelAccelerator, sessionCommand, sessionAppId, a4, flag);
}

int Gen11::dozero()
{
	return 0;
}

void Gen11::dovoid()
{
}

bool Gen11::dotrue()
{
	return true;
}

bool Gen11::dofalse()
{
	return false;
}

void Gen11::FBMemMgr_Init(void *that)
{
	
	FunctionCast(FBMemMgr_Init, callback->oFBMemMgr_Init)(that);
	ccont2=that;
	
	if (kexticl) ccont=that;
	else
	ccont = getMember<void *>(that, 0xc40);
	
}

uint32_t Gen11::AppleIntelFramebufferinit(void *frame,void *cont,uint param_2)
{
	if (!kexticl) {
		getMember<void *>(frame, 0x4a40) = ccont;
		getMember<void *>(frame, 0xc40) = ccont;
	}
	auto ret=FunctionCast(AppleIntelFramebufferinit, callback->oAppleIntelFramebufferinit)(frame,cont,param_2 );
	if (param_2==0) frame0=(IOFramebuffer *)frame;
	return ret;
}

uint64_t  Gen11::AppleIntelPlaneinit(void *that,uint param_1)
{
	auto ret= FunctionCast(AppleIntelPlaneinit, callback->oAppleIntelPlaneinit)(that,param_1 );
	getMember<void *>(that, 0x90) = ccont;
	return ret;
}

unsigned long Gen11::AppleIntelScalerinit(void *that,uint param_1)
{
	auto ret=  FunctionCast(AppleIntelScalerinit, callback->oAppleIntelScalerinit)(that,param_1 );
	getMember<void *>(that, 0x28) = ccont;
	return ret;
}






void Gen11::hwGetCRTC(void *that,void *param_1,void *param_2)
{
	FunctionCast(hwGetCRTC, callback->ohwGetCRTC)(that,param_1,param_2 );
	
	if (bk==2){
		NBlue::callback->parse_backlight();
		bk=1;
	}
	
	if (bk==1){
		
		//bklfrequency
		getMember<uint32_t>(that, kexticl ? 0xe54 : kexttgld ? 0xe6c : 0x60)=NBlue::callback->display_base.panel.backlight.pwm_level_max;
		
		//bkllvl
		if (getMember<uint32_t>(that, kexticl ? 0xe4c : kexttgld ? 0xe64 : 0xe58)<NBlue::callback->display_base.panel.backlight.level)
		getMember<uint32_t>(that, kexticl ? 0xe4c : kexttgld ? 0xe64 : 0xe58)=NBlue::callback->display_base.panel.backlight.level;
		
		if (getMember<uint32_t>(that, kexticl ? 0xe50 : kexttgld ? 0xe68 : 0xe5c)>NBlue::callback->display_base.panel.backlight.level)
			getMember<uint32_t>(that, kexticl ? 0xe4c : kexttgld ? 0xe64 : 0xe58)=getMember<uint32_t>(that, kexticl ? 0xe50 : kexttgld ? 0xe68 : 0xe5c);
		
		//bkllvl_saved
		if (getMember<uint32_t>(that, kexticl ? 0xe50 : kexttgld ? 0xe68 : 0xe5c)<NBlue::callback->display_base.panel.backlight.level)
		getMember<uint32_t>(that, kexticl ? 0xe50 : kexttgld ? 0xe68 : 0xe5c)=NBlue::callback->display_base.panel.backlight.level;
		
		
		u32 pwm_ctl;

		pwm_ctl = NBlue::callback->readReg32( BXT_BLC_PWM_CTL(NBlue::callback->display_base.panel.backlight.controller));
		if (pwm_ctl & BXT_BLC_PWM_ENABLE) {
			pwm_ctl &= ~BXT_BLC_PWM_ENABLE;
			NBlue::callback->writeReg32( BXT_BLC_PWM_CTL(NBlue::callback->display_base.panel.backlight.controller),
					   pwm_ctl);
		}

		NBlue::callback->writeReg32( BXT_BLC_PWM_FREQ(NBlue::callback->display_base.panel.backlight.controller),
									NBlue::callback->display_base.panel.backlight.pwm_level_max);

		NBlue::callback->writeReg32( BXT_BLC_PWM_DUTY(NBlue::callback->display_base.panel.backlight.controller), NBlue::callback->display_base.panel.backlight.level);

		pwm_ctl = 0;
		if (NBlue::callback->display_base.panel.backlight.active_low_pwm)
			pwm_ctl |= BXT_BLC_PWM_POLARITY;

		NBlue::callback->writeReg32( BXT_BLC_PWM_CTL(NBlue::callback->display_base.panel.backlight.controller), pwm_ctl);
		NBlue::callback->readReg32( BXT_BLC_PWM_CTL(NBlue::callback->display_base.panel.backlight.controller));
		NBlue::callback->writeReg32( BXT_BLC_PWM_CTL(NBlue::callback->display_base.panel.backlight.controller),
				   pwm_ctl | BXT_BLC_PWM_ENABLE);

	}

}


void Gen11::hwSetPanelPowerConfig(void *that, uint param_1)
{
	
	if (bk==2){
		NBlue::callback->parse_backlight();
		bk=1;
	}
	
	if (kexticl) getMember<uint32_t>(that, 0xd00)= param_1;
	else getMember<uint32_t>(that, 0xd48)= param_1;
	
	struct intel_pps_delays p = NBlue::callback->display_base.panel.pps.pps_delays;


	if (kexttgld)
	{
		getMember<uint32_t>(that, 0x1550)= p.backlight_on;
		getMember<uint32_t>(that, 0x1554)= p.backlight_off;
		getMember<uint32_t>(that, 0x1558) = p.power_up;
		getMember<uint32_t>(that, 0x155c)= p.power_down;
		getMember<uint32_t>(that, 0x1560)= p.power_cycle;
	}
	else
	{
		getMember<uint32_t>(that, 0x1548)= p.backlight_on;
		getMember<uint32_t>(that, 0x1550)= p.backlight_off;
		getMember<uint32_t>(that, 0x154c) = p.power_up;
		getMember<uint32_t>(that, 0x1554)= p.power_down;
		getMember<uint32_t>(that, 0x1558)= p.power_cycle;
	}
	
	struct pps_registers regs=NBlue::callback->display_base.panel.regs;
	
	
	uint32_t PCH_PP_ON_DELAYS = REG_FIELD_PREP(PANEL_POWER_UP_DELAY_MASK, p.power_up) |
		REG_FIELD_PREP(PANEL_LIGHT_ON_DELAY_MASK, p.backlight_on);
	uint32_t PCH_PP_OFF_DELAYS = REG_FIELD_PREP(PANEL_LIGHT_OFF_DELAY_MASK, p.backlight_off) |
		REG_FIELD_PREP(PANEL_POWER_DOWN_DELAY_MASK, p.power_down);

	NBlue::callback->writeReg32(regs.pp_on, PCH_PP_ON_DELAYS);
	NBlue::callback->writeReg32(regs.pp_off, PCH_PP_OFF_DELAYS);

	int div = NBlue::callback->display_base.panel.rawclk_freq / 1000;
	
	if (NBlue::callback->readReg32(regs.pp_div)!=-1)
		NBlue::callback->writeReg32( regs.pp_div,
				   REG_FIELD_PREP(PP_REFERENCE_DIVIDER_MASK,
						  (100 * div) / 2 - 1) |
				   REG_FIELD_PREP(PANEL_POWER_CYCLE_DELAY_MASK,
						  DIV_ROUND_UP(p.power_cycle, 1000) + 1));
	else
		NBlue::callback->intel_de_rmw( regs.pp_ctrl, BXT_POWER_CYCLE_DELAY_MASK,
				 REG_FIELD_PREP(BXT_POWER_CYCLE_DELAY_MASK,
						DIV_ROUND_UP(p.power_cycle, 1000) + 1));
	

	if (kexticl)
	{
		getMember<uint32_t>(that, 0xd30)=PCH_PP_ON_DELAYS;
		getMember<uint32_t>(that, 0xd34)=PCH_PP_OFF_DELAYS;
		getMember<uint32_t>(that, 0xd38) = NBlue::callback->readReg32(regs.pp_ctrl);
	}
	else
	{
		getMember<uint32_t>(that, 0xd78)=PCH_PP_ON_DELAYS;
		getMember<uint32_t>(that, 0xd7c)=PCH_PP_OFF_DELAYS;
		getMember<uint32_t>(that, 0xd80) = NBlue::callback->readReg32(regs.pp_ctrl);
	}

}


uint32_t Gen11::raReadRegister32(void *that,unsigned long param_1)
{
	if (that==nullptr) that=ccont;
	auto ret=FunctionCast(raReadRegister32, callback->oraReadRegister32)(that,param_1);
	return ret;
};
void Gen11::raWriteRegister32(void *that,unsigned long param_1, UInt32 param_2)
{
	if (that==nullptr) that=ccont;
	FunctionCast(raWriteRegister32, callback->oraWriteRegister32)( that,param_1,param_2);
	
};

uint32_t Gen11::enableVDDForAux(void *that,void *param_1)
{
	auto ret= FunctionCast(enableVDDForAux, callback->oenableVDDForAux)( that,param_1);

	if (ret==0xe00002eb) {
		if (getMember<bool>(that, kexttgld ? 0xe62 : 0xe55) == true) {
			IOFramebuffer *r= (IOFramebuffer *)getMember<void *>(that, 0xd60);
			r->setProperty("AAPL,LCD-PowerState-ON", true);
		}
	}
	return ret;

};

uint64_t Gen11::disableVDDForAux(void *that)
{
	auto ret= FunctionCast(disableVDDForAux, callback->odisableVDDForAux)( that);

	if (ret==0xe00002eb) {
		if (getMember<bool>(that, kexttgld ? 0xe62 : 0xe55) == true) {
			IOFramebuffer *r= (IOFramebuffer *)getMember<void *>(that, 0xd60);
			r->setProperty("AAPL,LCD-PowerState-ON", false);
		}
	}
	return ret;

};

IOReturn Gen11::getAttributeForConnection(void* framebuffer, int32_t connectIndex, uint32_t attribute,
										  unsigned long *value)
{
	const auto ret = FunctionCast(getAttributeForConnection, callback->ogetAttributeForConnection)(
																										   framebuffer, connectIndex, attribute, value);
	
	
	if (attribute != 'bklt') { return ret; }
	
	unsigned long v=NBlue::callback->display_base.panel.backlight.level;
	if (getMember<uint32_t>(ccont2, kexticl ? 0xe4c: kexttgld ? 0xe64 : 0xe58)<v)  getMember<uint32_t>(ccont2, kexticl ? 0xe4c: kexttgld ? 0xe64 : 0xe58)=v;
	
	*value=getMember<uint32_t>(ccont2, kexticl ? 0xe4c: kexttgld ? 0xe64 : 0xe58);
	value[1] = 0;
	value[2] = 0xffff;
	
	return kIOReturnSuccess;
};

 
uint32_t Gen11::fsetAttribute(void *that,uint param_1,unsigned long param_2)
{
	if (param_1 == 'wsrv' && param_2!=0 && hwu==4)
	{
		fsetAttribute(that, 'powr',1);
		IOSleep(2);
		fsetAttribute(that, 'powr',2);
		IOSleep(2);
	}
	auto ret=FunctionCast(fsetAttribute,callback->ofsetAttribute)(that, param_1, param_2);
	
	if (param_1 == 'wsrv' && param_2!=0 && hwu==4)
	{
		hwu=3;
	}
	return ret;
};



IOReturn Gen11::wrapSetAttributeForConnection(void* framebuffer, int32_t connectIndex, uint32_t attribute,
											  unsigned long value)
{
	const auto ret = FunctionCast(wrapSetAttributeForConnection, callback->owrapSetAttributeForConnection)(
																										   framebuffer, connectIndex, attribute, value);
	
	
	if (attribute != 'bklt') { return ret; }
	
	
	if (value<NBlue::callback->display_base.panel.backlight.level)  value=NBlue::callback->display_base.panel.backlight.level;
	
	NBlue::callback->writeReg32( BXT_BLC_PWM_DUTY(NBlue::callback->display_base.panel.backlight.controller), value);
	getMember<uint32_t>(ccont2, kexticl ? 0xe4c: kexttgld ? 0xe64 : 0xe58)=value;
	return kIOReturnSuccess;
};



void Gen11::updateSliceConfig(void *that, uint32_t val)
{
	IGSliceConfig requestedConfig;
	requestedConfig.raw = val;
	if (requestedConfig.getSubSliceCount()>4) requestedConfig.setSubSliceCount(int(requestedConfig.getSubSliceCount()/2));
	if (requestedConfig.getSubSliceCount()>4) requestedConfig.setSubSliceCount(4);
	
	if (requestedConfig.getEUCount()>8) requestedConfig.setEUCount(8);
	
	FunctionCast(updateSliceConfig, callback->oupdateSliceConfig)( that,requestedConfig.raw);
}

void Gen11::setAsyncSliceCount(void *that, uint32_t val)
{
	IGSliceConfig requestedConfig;
	requestedConfig.raw = val;
	if (requestedConfig.getSubSliceCount()>4) requestedConfig.setSubSliceCount(int(requestedConfig.getSubSliceCount()/2));
	if (requestedConfig.getSubSliceCount()>4) requestedConfig.setSubSliceCount(4);
	
	if (requestedConfig.getEUCount()>8) requestedConfig.setEUCount(8);
	
	FunctionCast(setAsyncSliceCount, callback->osetAsyncSliceCount)( that,requestedConfig.raw);
}

unsigned long  Gen11::setSliceConfig(void *that, uint32_t val)
{
	IGSliceConfig requestedConfig;
	requestedConfig.raw = val;
	if (requestedConfig.getSubSliceCount()>4) requestedConfig.setSubSliceCount(int(requestedConfig.getSubSliceCount()/2));
	if (requestedConfig.getSubSliceCount()>4) requestedConfig.setSubSliceCount(4);
	
	if (requestedConfig.getEUCount()>8) requestedConfig.setEUCount(8);
	
	return FunctionCast(setSliceConfig, callback->osetSliceConfig)( that,requestedConfig.raw);
}

void Gen11::setAsyncSliceCount2(void *that, uint32_t val)
{
	IGSliceConfig requestedConfig;
	requestedConfig.raw = val;
	if (requestedConfig.getSubSliceCount()>4) requestedConfig.setSubSliceCount(int(requestedConfig.getSubSliceCount()/2));
	if (requestedConfig.getSubSliceCount()>4) requestedConfig.setSubSliceCount(4);
	
	if (requestedConfig.getEUCount()>8) requestedConfig.setEUCount(8);
	
	FunctionCast(setAsyncSliceCount2, callback->osetAsyncSliceCount2)( that,requestedConfig.raw);
}



void  Gen11::setPanelPowerState(void *that ,bool param_1)
{
	frame0->setProperty("AAPL,LCD-PowerState-ON", param_1);
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
	
	callback->orgDisableCDClock(that);
	
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




void Gen11::hwConfigureCustomAUX(void *that,bool param_1)
{
	//FunctionCast(hwConfigureCustomAUX, callback->ohwConfigureCustomAUX)(that,param_1 );
}

unsigned long Gen11::hwRegsNeedUpdate
		  (void *that,void *param_1,
		   void *param_2,void *param_3,void *param_4,
		   void *param_5)
{
	
	auto ret=FunctionCast(hwRegsNeedUpdate, callback->ohwRegsNeedUpdate)(that,param_1,param_2,param_3,param_4,param_5 );

	return 0;
	
	if (hwu==1){
		hwu=0;
		return 1;
	}
	return ret;
}

int Gen11::getTranscoderOffset(void *that,void *param_1,uint param_2)
{
	auto ret=FunctionCast(getTranscoderOffset, callback->ogetTranscoderOffset)(that,param_1,param_2 );
	
	if (ret==0xf000) return 0;
	return ret;
}

unsigned long  Gen11::AppleIntelPortHALinit(void *that,void *param_1)
{
	auto ret=FunctionCast(AppleIntelPortHALinit, callback->oAppleIntelPortHALinit)(that,param_1 );
	getMember<uint32_t>(that, 0x584)=0x60540;
	getMember<uint32_t>(that, 0x588)=0x60544;
	return ret;
}

unsigned long Gen11::hwSetMode(void *that,void *param_1,void *param_2,int param_3,void *param_4)
{
	auto ret=FunctionCast(hwSetMode, callback->ohwSetMode)(that,param_1,param_2,param_3,param_4 );
	
	if (hwu==2){
		hwu=1;
	}
	
	return ret;
}


uint64_t Gen11::aframeBufferNotificationcallback(void *param_1,void *param_2,void *param_3,int param_4,void *param_5)
{
	auto ret=FunctionCast(aframeBufferNotificationcallback, callback->oaframeBufferNotificationcallback)(param_1,param_2,param_3,param_4,param_5 );

	if (param_4 == 2)
	{
		if (hwu==3) hwu=2;
	}
	return ret;
}


static bool is_valid_dmc_id(enum intel_dmc_id dmc_id)
{
	return dmc_id >= DMC_FW_MAIN && dmc_id < DMC_FW_MAX;
}
static bool fw_info_matches_stepping(const struct intel_fw_info *fw_info,
					 const struct stepping_info *si)
{
	if ((fw_info->substepping == '*' && si->stepping == fw_info->stepping) ||
		(si->stepping == fw_info->stepping && si->substepping == fw_info->substepping) ||

		(si->stepping == '*' && si->substepping == fw_info->substepping) ||
		(fw_info->stepping == '*' && fw_info->substepping == '*'))
		return true;

	return false;
}
static void dmc_set_fw_offset(struct intel_dmc *dmc,
				  const struct intel_fw_info *fw_info,
				  unsigned int num_entries,
				  const struct stepping_info *si,
				  u8 package_ver)
{
	enum intel_dmc_id dmc_id;
	unsigned int i;

	for (i = 0; i < num_entries; i++) {
		dmc_id = static_cast<intel_dmc_id>(package_ver <= 1 ? DMC_FW_MAIN : fw_info[i].dmc_id);

		if (!is_valid_dmc_id(dmc_id)) {
			continue;
		}


		if (dmc->dmc_info[dmc_id].present)
			continue;

		
		if (fw_info_matches_stepping(&fw_info[i], si)) {
			dmc->dmc_info[dmc_id].present = true;
			dmc->dmc_info[dmc_id].dmc_offset = fw_info[i].offset;
		}
	}
}

static u32
parse_dmc_fw_package(struct intel_dmc *dmc,
			 const struct intel_package_header *package_header,
			 const struct stepping_info *si,
			 size_t rem_size)
{
	u32 package_size = sizeof(struct intel_package_header);
	u32 num_entries, max_entries;
	const struct intel_fw_info *fw_info;

	if (rem_size < package_size)
		goto error_truncated;

	if (package_header->header_ver == 1) {
		max_entries = PACKAGE_MAX_FW_INFO_ENTRIES;
	} else if (package_header->header_ver == 2) {
		max_entries = PACKAGE_V2_MAX_FW_INFO_ENTRIES;
	} else {
		return 0;
	}


	package_size += max_entries * sizeof(struct intel_fw_info);
	if (rem_size < package_size)
		goto error_truncated;

	if (package_header->header_len * 4 != package_size) {
		return 0;
	}

	num_entries = package_header->num_entries;
	if ((num_entries > max_entries))
		num_entries = max_entries;
	

	fw_info = (const struct intel_fw_info *)
		((u8 *)package_header + sizeof(*package_header));
	dmc_set_fw_offset(dmc, fw_info, num_entries, si,
			  package_header->header_ver);

	return package_size;

error_truncated:
	return 0;
}

static bool dmc_mmio_addr_sanity_check(struct intel_dmc *dmc,
					   const u32 *mmioaddr, u32 mmio_count,
					   int header_ver, enum intel_dmc_id dmc_id)
{
	struct intel_display *display = dmc->display;
	u32 start_range, end_range;
	u32 i;

	if (header_ver == 1) {
		start_range = DMC_MMIO_START_RANGE;
		end_range = DMC_MMIO_END_RANGE;
	} else if (dmc_id == DMC_FW_MAIN) {
		start_range = TGL_MAIN_MMIO_START;
		end_range = TGL_MAIN_MMIO_END;
	} else if (DISPLAY_VER(display) >= 13) {
		start_range = ADLP_PIPE_MMIO_START;
		end_range = ADLP_PIPE_MMIO_END;
	} else if (DISPLAY_VER(display) >= 12) {
		start_range = TGL_PIPE_MMIO_START(dmc_id);
		end_range = TGL_PIPE_MMIO_END(dmc_id);
	} else {
		return false;
	}

	for (i = 0; i < mmio_count; i++) {
		if (mmioaddr[i] < start_range || mmioaddr[i] > end_range)
			return false;
	}

	return true;
}


static bool is_dmc_evt_ctl_reg(struct intel_display *display,
				   enum intel_dmc_id dmc_id, u32 reg)
{
	u32 offset = (reg);
	u32 start = (DMC_EVT_CTL(display, dmc_id, 0));
	u32 end = (DMC_EVT_CTL(display, dmc_id, DMC_EVENT_HANDLER_COUNT_GEN12));

	return offset >= start && offset < end;
}

static bool is_dmc_evt_htp_reg(struct intel_display *display,
				   enum intel_dmc_id dmc_id, u32 reg)
{
	u32 offset = (reg);
	u32 start = (DMC_EVT_HTP(display, dmc_id, 0));
	u32 end = (DMC_EVT_HTP(display, dmc_id, DMC_EVENT_HANDLER_COUNT_GEN12));

	return offset >= start && offset < end;
}

static bool is_event_handler(struct intel_display *display,
				 enum intel_dmc_id dmc_id,
				 unsigned int event_id,
							 u32 reg, u32 data)
{
	return is_dmc_evt_ctl_reg(display, dmc_id, reg) &&
		REG_FIELD_GET(DMC_EVT_CTL_EVENT_ID_MASK, data) == event_id;
}

static bool fixup_dmc_evt(struct intel_display *display,
			  enum intel_dmc_id dmc_id,
						  u32 reg_ctl, u32 *data_ctl,
						  u32 reg_htp, u32 *data_htp)
{
	if (!is_dmc_evt_ctl_reg(display, dmc_id, reg_ctl))
		return false;

	if (!is_dmc_evt_htp_reg(display, dmc_id, reg_htp))
		return false;

	if ((reg_ctl) - (DMC_EVT_CTL(display, dmc_id, 0)) !=
		(reg_htp) - (DMC_EVT_HTP(display, dmc_id, 0)))
		return false;


	if (display->platform.alderlake_s && dmc_id == DMC_FW_MAIN &&
		is_event_handler(display, dmc_id, 0x32, reg_ctl, *data_ctl)) {
		*data_ctl = 0;
		*data_htp = 0;
		return true;
	}


	if ((display->platform.tigerlake || display->platform.alderlake_s) &&
		is_event_handler(display, dmc_id, 0x32, reg_ctl, *data_ctl)) {
		*data_ctl &= ~DMC_EVT_CTL_EVENT_ID_MASK;
		*data_ctl |=  REG_FIELD_PREP(DMC_EVT_CTL_EVENT_ID_MASK,
									 0x2e);
		return true;
	}

	return false;
}


static u32 parse_dmc_fw_header(struct intel_dmc *dmc,
				   const struct intel_dmc_header_base *dmc_header,
				   size_t rem_size, enum intel_dmc_id dmc_id)
{
	struct intel_display *display = dmc->display;
	struct dmc_fw_info *dmc_info = &dmc->dmc_info[dmc_id];
	unsigned int header_len_bytes, dmc_header_size, payload_size, i;
	const u32 *mmioaddr, *mmiodata;
	u32 mmio_count, mmio_count_max, start_mmioaddr;
	u8 *payload;


	if (rem_size < sizeof(struct intel_dmc_header_base))
		goto error_truncated;

	if (dmc_header->header_ver == 3) {
		const struct intel_dmc_header_v3 *v3 =
			(const struct intel_dmc_header_v3 *)dmc_header;

		if (rem_size < sizeof(struct intel_dmc_header_v3))
			goto error_truncated;

		mmioaddr = v3->mmioaddr;
		mmiodata = v3->mmiodata;
		mmio_count = v3->mmio_count;
		mmio_count_max = DMC_V3_MAX_MMIO_COUNT;

		header_len_bytes = dmc_header->header_len * 4;
		start_mmioaddr = v3->start_mmioaddr;
		dmc_header_size = sizeof(*v3);
	} else if (dmc_header->header_ver == 1) {
		const struct intel_dmc_header_v1 *v1 =
			(const struct intel_dmc_header_v1 *)dmc_header;

		if (rem_size < sizeof(struct intel_dmc_header_v1))
			goto error_truncated;

		mmioaddr = v1->mmioaddr;
		mmiodata = v1->mmiodata;
		mmio_count = v1->mmio_count;
		mmio_count_max = DMC_V1_MAX_MMIO_COUNT;
		header_len_bytes = dmc_header->header_len;
		start_mmioaddr = DMC_V1_MMIO_START_RANGE;
		dmc_header_size = sizeof(*v1);
	} else {
		return 0;
	}

	if (header_len_bytes != dmc_header_size) {
		return 0;
	}

	if (mmio_count > mmio_count_max) {
		return 0;
	}

	if (!dmc_mmio_addr_sanity_check(dmc, mmioaddr, mmio_count,
					dmc_header->header_ver, dmc_id)) {
		return 0;
	}

	for (i = 0; i < mmio_count; i++) {
		dmc_info->mmioaddr[i] = _MMIO(mmioaddr[i]);
		dmc_info->mmiodata[i] = mmiodata[i];
	}

	for (i = 0; i < mmio_count - 1; i++) {

		if (!fixup_dmc_evt(display, dmc_id,
				   dmc_info->mmioaddr[i], &dmc_info->mmiodata[i],
				   dmc_info->mmioaddr[i+1], &dmc_info->mmiodata[i+1]))
			continue;

	}

	dmc_info->mmio_count = mmio_count;
	dmc_info->start_mmioaddr = start_mmioaddr;

	rem_size -= header_len_bytes;

	payload_size = dmc_header->fw_size * 4;
	if (rem_size < payload_size)
		goto error_truncated;

	if (payload_size > dmc->max_fw_size) {
		return 0;
	}
	dmc_info->dmc_fw_size = dmc_header->fw_size;

	dmc_info->payload = static_cast<UInt32 *>(IOMallocZero(payload_size));
	if (!dmc_info->payload)
		return 0;
	

	payload = (u8 *)(dmc_header) + header_len_bytes;
	memcpy(dmc_info->payload, payload, payload_size);

	return header_len_bytes + payload_size;

error_truncated:
	return 0;
}



static void initialize_stepping_info(struct intel_display *display, struct stepping_info *si)
{
	const char *step_name = DISPLAY_RUNTIME_INFO(display)->step_name;

	si->stepping = step_name[0] ?: '*';
	si->substepping = step_name[1] ?: '*';
}

static u32 parse_dmc_fw_css(struct intel_dmc *dmc,
				struct intel_css_header *css_header,
				size_t rem_size)
{

	if (rem_size < sizeof(struct intel_css_header)) {
		return 0;
	}

	if (sizeof(struct intel_css_header) !=
		(css_header->header_len * 4)) {
		return 0;
	}

	dmc->version = css_header->version;

	return sizeof(struct intel_css_header);
}
static struct intel_dmc *display_to_dmc(struct intel_display *display)
{
	return display->dmc.dmc;
}
static void disable_event_handler(struct intel_display *display,
				  u32 ctl_reg, u32 htp_reg)
{
	NBlue::callback->writeReg32(  ctl_reg,
			   REG_FIELD_PREP(DMC_EVT_CTL_TYPE_MASK,
					  DMC_EVT_CTL_TYPE_EDGE_0_1) |
			   REG_FIELD_PREP(DMC_EVT_CTL_EVENT_ID_MASK,
					  DMC_EVENT_FALSE));
	NBlue::callback->writeReg32( htp_reg, 0);
}
static bool has_dmc_id_fw(struct intel_display *display, enum intel_dmc_id dmc_id)
{
	struct intel_dmc *dmc = display_to_dmc(display);

	return dmc && dmc->dmc_info[dmc_id].payload;
}

bool intel_dmc_has_payload(struct intel_display *display)
{
	return has_dmc_id_fw(display, DMC_FW_MAIN);
}
static void disable_all_event_handlers(struct intel_display *display,
					   enum intel_dmc_id dmc_id)
{
	int handler;

	if (DISPLAY_VER(display) < 12)
		return;

	if (!has_dmc_id_fw(display, dmc_id))
		return;

	for (handler = 0; handler < DMC_EVENT_HANDLER_COUNT_GEN12; handler++)
		disable_event_handler(display,
					  DMC_EVT_CTL(display, dmc_id, handler),
					  DMC_EVT_HTP(display, dmc_id, handler));
}
static u32 dmc_evt_ctl_disable(u32 dmc_evt_ctl)
{

	return (dmc_evt_ctl & DMC_EVT_CTL_ENABLE) |
		REG_FIELD_PREP(DMC_EVT_CTL_TYPE_MASK,
				   DMC_EVT_CTL_TYPE_EDGE_0_1) |
		REG_FIELD_PREP(DMC_EVT_CTL_EVENT_ID_MASK,
				   DMC_EVENT_FALSE);
}
static bool disable_dmc_evt(struct intel_display *display,
				enum intel_dmc_id dmc_id,
							u32 reg, u32 data)
{
	if (!is_dmc_evt_ctl_reg(display, dmc_id, reg))
		return false;

	if (dmc_id != DMC_FW_MAIN)
		return true;

	if (display->platform.tigerlake &&
		is_event_handler(display, dmc_id, 0xbf, reg, data))
		return true;

	if ((display->platform.tigerlake || display->platform.alderlake_s) &&
		is_event_handler(display, dmc_id, 0x2e, reg, data))
		return true;

	return false;
}
static u32 dmc_mmiodata(struct intel_display *display,
			struct intel_dmc *dmc,
			enum intel_dmc_id dmc_id, int i)
{
	if (disable_dmc_evt(display, dmc_id,
				dmc->dmc_info[dmc_id].mmioaddr[i],
				dmc->dmc_info[dmc_id].mmiodata[i]))
		return dmc_evt_ctl_disable(dmc->dmc_info[dmc_id].mmiodata[i]);
	else
		return dmc->dmc_info[dmc_id].mmiodata[i];
}

static void dmc_load_mmio(struct intel_display *display, enum intel_dmc_id dmc_id)
{
	struct intel_dmc *dmc = display_to_dmc(display);
	u32 i;

	for (i = 0; i < dmc->dmc_info[dmc_id].mmio_count; i++) {
		NBlue::callback->writeReg32( dmc->dmc_info[dmc_id].mmioaddr[i],
				   dmc_mmiodata(display, dmc, dmc_id, i));
	}
}
static void dmc_load_program(struct intel_display *display, enum intel_dmc_id dmc_id)
{
	struct intel_dmc *dmc = display_to_dmc(display);
	u32 i;

	disable_all_event_handlers(display, dmc_id);

	IOSimpleLock *myLock;
	myLock = IOSimpleLockAlloc();
	IOSimpleLockLock(myLock);

	//preempt_disable();

	for (i = 0; i < dmc->dmc_info[dmc_id].dmc_fw_size; i++) {
		NBlue::callback->writeReg32(
				  DMC_PROGRAM(dmc->dmc_info[dmc_id].start_mmioaddr, i),
				  dmc->dmc_info[dmc_id].payload[i]);
	}

	//preempt_enable();
	
	IOSimpleLockUnlock(myLock);
	IOSimpleLockFree(myLock);
	
	dmc_load_mmio(display, dmc_id);
}


static u32 gen9_dc_mask(struct intel_display *display)
{
	u32 mask;

	mask = DC_STATE_EN_UPTO_DC5;

	if (DISPLAY_VER(display) >= 12)
		mask |= DC_STATE_EN_DC3CO | DC_STATE_EN_UPTO_DC6
					  | DC_STATE_EN_DC9;
	else if (DISPLAY_VER(display) == 11)
		mask |= DC_STATE_EN_UPTO_DC6 | DC_STATE_EN_DC9;
	else if (display->platform.geminilake || display->platform.broxton)
		mask |= DC_STATE_EN_DC9;
	else
		mask |= DC_STATE_EN_UPTO_DC6;

	return mask;
}
void intel_dmc_update_dc6_allowed_count(struct intel_display *display,
					bool start_tracking)
{
	struct intel_dmc *dmc = display_to_dmc(display);
	u32 dc5_cur_count;

	if (DISPLAY_VER(dmc->display) < 14)
		return;

	dc5_cur_count = NBlue::callback->readReg32( DG1_DMC_DEBUG_DC5_COUNT);

	if (!start_tracking)
		dmc->dc6_allowed.count += dc5_cur_count - dmc->dc6_allowed.dc5_start;

	dmc->dc6_allowed.dc5_start = dc5_cur_count;
}

static void gen9_write_dc_state(struct intel_display *display,
				u32 state)
{
	int rewrites = 0;
	int rereads = 0;
	u32 v;

	NBlue::callback->writeReg32( DC_STATE_EN, state);

	do  {
		v = NBlue::callback->readReg32( DC_STATE_EN);

		if (v != state) {
			NBlue::callback->writeReg32( DC_STATE_EN, state);
			rewrites++;
			rereads = 0;
		} else if (rereads++ > 5) {
			break;
		}

	} while (rewrites < 100);

	/*if (v != state)
		drm_err(display->drm,
			"Writing dc state to 0x%x failed, now 0x%x\n",
			state, v);*/

	/* Most of the times we need one retry, avoid spam */
	/*if (rewrites > 1)
		drm_dbg_kms(display->drm,
				"Rewrote dc state to 0x%x %d times\n",
				state, rewrites);*/
}

void gen9_set_dc_state(struct intel_display *display, u32 state)
{
	struct i915_power_domains *power_domains = &display->power.domains;
	bool dc6_was_enabled, enable_dc6;
	u32 mask;
	u32 val;


	if ((state & ~power_domains->allowed_dc_mask))
		state &= power_domains->allowed_dc_mask;

	//if (!power_domains->initializing)
	//	intel_psr_notify_dc5_dc6(display);

	val = NBlue::callback->readReg32( DC_STATE_EN);
	mask = gen9_dc_mask(display);
	//drm_dbg_kms(display->drm, "Setting DC state from %02x to %02x\n",
		//	val & mask, state);

	/* Check if DMC is ignoring our DC state requests */
	//if ((val & mask) != power_domains->dc_state)
	//	drm_err(display->drm, "DC state mismatch (0x%x -> 0x%x)\n",
		//	power_domains->dc_state, val & mask);

	enable_dc6 = state & DC_STATE_EN_UPTO_DC6;
	dc6_was_enabled = power_domains->dc_state & DC_STATE_EN_UPTO_DC6;
	if (!dc6_was_enabled && enable_dc6)
		intel_dmc_update_dc6_allowed_count(display, true);

	val &= ~mask;
	val |= state;

	gen9_write_dc_state(display, val);

	if (!enable_dc6 && dc6_was_enabled)
		intel_dmc_update_dc6_allowed_count(display, false);

	power_domains->dc_state = val & mask;
}

static int gen7_check_mailbox_status(u32 mbox)
{
	switch (mbox & GEN6_PCODE_ERROR_MASK) {
	case GEN6_PCODE_SUCCESS:
		return 0;
	case GEN6_PCODE_ILLEGAL_CMD:
		return -ENXIO;
	case GEN7_PCODE_TIMEOUT:
		return -ETIMEDOUT;
	case GEN7_PCODE_ILLEGAL_DATA:
		return -EINVAL;
	case GEN11_PCODE_ILLEGAL_SUBCOMMAND:
		return -ENXIO;
	case GEN11_PCODE_LOCKED:
		return -EBUSY;
	case GEN11_PCODE_REJECTED:
		return -EACCES;
	case GEN7_PCODE_MIN_FREQ_TABLE_GT_RATIO_OUT_OF_RANGE:
		return -EOVERFLOW;
	default:
		return 0;
	}
}


static int snb_pcode_rw( u32 mbox,
			  u32 *val, u32 *val1,
			  int fast_timeout_us, int slow_timeout_ms,
			  bool is_read)
{

	if (NBlue::callback->readReg32( GEN6_PCODE_MAILBOX) & GEN6_PCODE_READY)
		return -EAGAIN;

	NBlue::callback->writeReg32( GEN6_PCODE_DATA, *val);
	NBlue::callback->writeReg32( GEN6_PCODE_DATA1, val1 ? *val1 : 0);
	NBlue::callback->writeReg32( GEN6_PCODE_MAILBOX, GEN6_PCODE_READY | mbox);

	u32 iVar2 = -500;
	u32 iVar4=0;
	do {
		if (iVar2 == 0) return -ETIMEDOUT;
	  IOSleep(1);
		iVar4 = NBlue::callback->readReg32(GEN6_PCODE_MAILBOX);
	  iVar2 = iVar2 + 1;
	} while (iVar4 < 0);

	if (is_read)
		*val = NBlue::callback->readReg32( GEN6_PCODE_DATA);
	if (is_read && val1)
		*val1 = NBlue::callback->readReg32( GEN6_PCODE_DATA1);

		return gen7_check_mailbox_status(mbox);

}

static void
tgl_tc_cold_request(struct intel_display *display, bool block)
{
	u8 tries = 0;
	int ret;
	
	IOSimpleLock *myLock;
	myLock = IOSimpleLockAlloc();

	while (1) {
		u32 low_val;
		u32 high_val = 0;

		if (block)
			low_val = TGL_PCODE_EXIT_TCCOLD_DATA_L_BLOCK_REQ;
		else
			low_val = TGL_PCODE_EXIT_TCCOLD_DATA_L_UNBLOCK_REQ;

		IOSimpleLockLock(myLock);
		ret = snb_pcode_rw( TGL_PCODE_TCCOLD, &low_val, &high_val, 500, 20, true);
		IOSimpleLockUnlock(myLock);
		if (ret == 0) {
			if (block &&
				(low_val & TGL_PCODE_EXIT_TCCOLD_DATA_L_EXIT_FAILED))
				ret = -EIO;
			else
				break;
		}

		if (++tries == 3)
			break;

		IOSleep(1);
	}
	IOSimpleLockFree(myLock);
}





void Gen11::hwInitializeCState(void *that)
{
	struct intel_display *display=&NBlue::callback->display_base;
	
	
	if (getMember<int>(that, kexticl ? 0xb38 : 0xb48) != 1) return;
	
	gen9_set_dc_state(display,DC_STATE_DISABLE);
	
	if (DISPLAY_VER(display)== 12)
		tgl_tc_cold_request(&NBlue::callback->display_base,false);

	if (intel_display_wa(display, INTEL_DISPLAY_WA_14011294188))
		NBlue::callback->intel_de_rmw( SOUTH_DSPCLK_GATE_D, 0,
				 PCH_DPMGUNIT_CLOCK_GATE_DISABLE);
	
	if (DISPLAY_VER(display)== 12){
		NBlue::callback->intel_de_rmw( CLKREQ_POLICY, CLKREQ_POLICY_MEM_UP_OVRD, 0);
	}
	
	struct intel_dmc *dmc=NBlue::callback->display_base.dmc.dmc;
	
	struct intel_css_header *css_header;
	struct intel_package_header *package_header;
	struct intel_dmc_header_base *dmc_header;
	struct stepping_info si = {};
	u32 readcount = 0;
	u32 r, offset;
	
	auto fw = getFWByName("tgl_dmc_ver2_12.bin");
	if (!fw.data || fw.size == 0) {
		return;
	}
	
	initialize_stepping_info(&NBlue::callback->display_base, &si);
	
	css_header = (struct intel_css_header *)fw.data;
	r = parse_dmc_fw_css(dmc, css_header, fw.size);
	if (!r)
		return ;

	readcount += r;

	package_header = (struct intel_package_header *)&fw.data[readcount];
	r = parse_dmc_fw_package(dmc, package_header, &si, fw.size - readcount);
	if (!r)
		return ;

	readcount += r;

	for (uint8_t i = 0; i < DMC_FW_MAX; i++) {
		if (!dmc->dmc_info[i].present)
			continue;
		offset = readcount + dmc->dmc_info[i].dmc_offset * 4;
		if (offset > fw.size) {
			continue;
		}

		dmc_header = (struct intel_dmc_header_base *)&fw.data[offset];
		parse_dmc_fw_header(dmc, dmc_header, fw.size - offset, static_cast<intel_dmc_id>(i));
	}

	if (!intel_dmc_has_payload(display)) {
		return ;
	}
	

	
	//skip DMC_FW_MAIN if i=1
	for (uint8_t i = 0; i < DMC_FW_MAX; i++) {
		if (!dmc->dmc_info[i].present)
			continue;
		dmc_load_program(display, static_cast<intel_dmc_id>(i));
	}
	

		
	
	if (intel_display_wa(display, INTEL_DISPLAY_WA_14011508470))
			NBlue::callback->intel_de_rmw( GEN11_CHICKEN_DCPR_2, 0,
					 DCPR_CLEAR_MEMSTAT_DIS | DCPR_SEND_RESP_IMM |
					 DCPR_MASK_LPMODE | DCPR_MASK_MAXLATENCY_MEMUP_CLR);
	
	NBlue::callback->intel_de_rmw( DC_STATE_DEBUG, 0,
			 DC_STATE_DEBUG_MASK_CORES | DC_STATE_DEBUG_MASK_MEMORY_UP);
	NBlue::callback->readReg32(DC_STATE_DEBUG);
	
	NBlue::callback->intel_de_rmw( PIPEDMC_CONTROL(PIPE_A), 0, PIPEDMC_ENABLE);

	hwConfigureCustomAUX(that, true);
	
}

void Gen11::setupPlane(void *that,void *param_1,int param_2)
{
	FunctionCast(setupPlane, callback->osetupPlane)(that ,param_1,param_2);
	
	//PLANE_CTL_1_A (0x00070180): 0x84000400
	//PLANE_STRIDE_1_A (0x00070188): 0x0000000d
	
	getMember<uint32_t>(that, 0x100)=0x84000400;
	getMember<uint32_t>(that, 0x118)=0xd;
}

void Gen11::setupPlane2(void *that,void *param_1)
{
	FunctionCast(setupPlane2, callback->osetupPlane2)(that ,param_1);
	
	//PLANE_CTL_1_A (0x00070180): 0x84000400
	//PLANE_STRIDE_1_A (0x00070188): 0x0000000d
	
	getMember<uint32_t>(that, 0x100)=0x84000400;
	getMember<uint32_t>(that, 0x118)=0xd;
}

void Gen11::SetupParams (void *that,void *param_1,void *param_2,CRTCParams *param_3,void *param_4)
{
	if (getMember<uint32_t>(param_1, 0x1dc) == 0) setpc=1;
	if (!setpc) return FunctionCast(SetupParams, callback->oSetupParams)(that ,param_1,param_2,param_3,param_4);
	SetupParams2(param_2, param_3);
	setupPipeWatermarks(that ,param_1,param_2,param_3);
}

void Gen11::setupPipeWatermarks (void *that,void *param_1,void *param_2,CRTCParams *param_3)
{
	FunctionCast(setupPipeWatermarks, callback->osetupPipeWatermarks)(that ,param_1,param_2,param_3);
}

void Gen11::SetupParams2 (void *param_2, CRTCParams *param_3)
{
	struct intel_display *display = &NBlue::callback->display_base;
	if (setpc){
		setpc=0;
		
		enum port port=display->port0;
		u32 val=0;
		switch (display->panel.vbt.edp.bpp) {
			case 18:
				val |= PIPE_MISC_BPC_6;
				break;
			default:
			case 24:
				val |= PIPE_MISC_BPC_8;
				break;
			case 30:
				val |= PIPE_MISC_BPC_10;
				break;
			case 36:
				if (DISPLAY_VER(display) >= 13)
					val |= PIPE_MISC_BPC_12_ADLP;
				break;
		}
		

		if (display->panel.vbt.lvds_dither)
			val |= PIPE_MISC_DITHER_ENABLE | PIPE_MISC_DITHER_TYPE_SP;
		
		if (display->output_format == INTEL_OUTPUT_FORMAT_YCBCR420 ||
			display->output_format == INTEL_OUTPUT_FORMAT_YCBCR444)
			val |= PIPE_MISC_OUTPUT_COLORSPACE_YUV;

		if (display->output_format == INTEL_OUTPUT_FORMAT_YCBCR420)
			val |= DISPLAY_VER(display) >= 30 ? PIPE_MISC_YUV420_ENABLE :
				PIPE_MISC_YUV420_ENABLE | PIPE_MISC_YUV420_MODE_FULL_BLEND;
		
		//if (DISPLAY_VER(display) >= 11 && is_hdr_mode(crtc_state))
		val |= PIPE_MISC_HDR_MODE_PRECISION;
		
		if (DISPLAY_VER(display) >= 12)
			val |= PIPE_MISC_PIXEL_ROUNDING_TRUNC;
		
		
		u32 temp;
		temp = TRANS_DDI_FUNC_ENABLE;
		temp |= TGL_TRANS_DDI_SELECT_PORT(port);
		
		switch (display->panel.vbt.edp.bpp) {
			case 18:
				temp |= TRANS_DDI_BPC_6;
				break;
			default:
			case 24:
				temp |= TRANS_DDI_BPC_8;
				break;
			case 30:
				temp |= TRANS_DDI_BPC_10;
				break;
			case 36:
				temp |= TRANS_DDI_BPC_12;
				break;
		}
		
		temp |= TRANS_DDI_PVSYNC;
		temp |= TRANS_DDI_PHSYNC;
		
		//if (intel_crtc_has_type(crtc_state, INTEL_OUTPUT_DP_MST) ||intel_dp_is_uhbr(crtc_state))
		//else
		temp |= TRANS_DDI_MODE_SELECT_DP_SST;
		temp |= DDI_PORT_WIDTH(display->panel.vbt.edp.lanes);
		
		
		//param_3->TRANS_CLK_SEL=0x10000000;
		param_3->TRANS_CLK_SEL=TGL_TRANS_CLK_SEL_PORT(port);
		param_3->TRANS_MSA_MISC = 0x21;
		//param_3->TRANS_DDI_FUNC_CTL= 0x8a000102;
		param_3->TRANS_DDI_FUNC_CTL= temp;
		//param_3->PIPE_MISC= 0x1800010;
		param_3->PIPE_MISC= val;
		//i9xx_set_pipeconf
		param_3->TRANS_CONF= 0xc0000024;
		
		int fScanoutHeight=getMember<int>(param_2, kexticl ? 0x2fc : 0xfc);
		int fLinkScanoutWidth=getMember<int>(param_2, kexticl ? 0x2f8 : 0xf8);
		param_3->PIPESRC =
			 (fScanoutHeight - 1) & 0x1fff |
			 param_3->PIPESRC & 0xe000e000 |
			 (fLinkScanoutWidth * 0x10000 + 0x1fff0000) & 0x1fff0000;
		
	}
	
}





static const struct intel_ddi_buf_trans *
intel_get_buf_trans(const struct intel_ddi_buf_trans *trans, int *num_entries)
{
	*num_entries = trans->num_entries;
	return trans;
}

static const struct intel_ddi_buf_trans *
tgl_get_combo_buf_trans_dp(struct intel_display *display,
			   int *n_entries)
{
	
//[drm:intel_crtc_state_dump [i915]] port clock: 270000
	
	if (display->port_clock > 270000) {
		if (display->platform.tigerlake_uy) {
			return intel_get_buf_trans(&tgl_uy_combo_phy_trans_dp_hbr2,
						   n_entries);
		} else {
			return intel_get_buf_trans(&tgl_combo_phy_trans_dp_hbr2,
						   n_entries);
		}
	} else {
		return intel_get_buf_trans(&tgl_combo_phy_trans_dp_hbr,
					   n_entries);
	}
}

static u32 icl_combo_phy_loadgen_select(int lane)
{
	struct intel_display *display = &NBlue::callback->display_base;
	if (display->port_clock > 600000)
		return 0;

	if (NBlue::callback->display_base.panel.vbt.edp.lanes == 4)
		return lane >= 1 ? LOADGEN_SELECT : 0;
	else
		return lane == 1 || lane == 2 ? LOADGEN_SELECT : 0;
}


static int translate_signal_level(u8 signal_levels)
{
	const u8 *signal_array;
	size_t array_size;
	int i;

	signal_array = index_to_dp_signal_levels;
	array_size = ARRAY_SIZE(index_to_dp_signal_levels);

	for (i = 0; i < array_size; i++) {
		if (signal_array[i] == signal_levels)
			return i;
	}


	return 0;
}

static int intel_ddi_dp_level(struct intel_display *display,
				  int lane)
{
	u8 train_set = display->intel_dp0.train_set[lane];
	
	/*if (intel_dp_is_uhbr(crtc_state)) {
		return train_set & DP_TX_FFE_PRESET_VALUE_MASK;
	} else {*/
		u8 signal_levels = train_set & (DP_TRAIN_VOLTAGE_SWING_MASK |
						DP_TRAIN_PRE_EMPHASIS_MASK);

		return translate_signal_level( signal_levels);
	//}
}

int intel_ddi_level(struct intel_display *display,
			int lane)
{
	const struct intel_ddi_buf_trans *trans;
	int level, n_entries;

	trans=tgl_get_combo_buf_trans_dp(display,&n_entries);

	/*if (intel_crtc_has_type(crtc_state, INTEL_OUTPUT_HDMI))
		level = intel_ddi_hdmi_level(encoder, trans);
	else*/
	level = intel_ddi_dp_level(display, lane);

	if (( level >= n_entries))
		level = n_entries - 1;

	return level;
}

static void icl_ddi_combo_vswing_program(struct intel_display *display)
{
	const struct intel_ddi_buf_trans *trans;
	enum phy phy = PHY_A;
	int n_entries, ln;
	u32 val;

	
	trans=tgl_get_combo_buf_trans_dp(display,&n_entries);
	
	/*if (intel_crtc_has_type(crtc_state, INTEL_OUTPUT_EDP)) {
		struct intel_dp *intel_dp = enc_to_intel_dp(encoder);

		val = EDP4K2K_MODE_OVRD_EN | EDP4K2K_MODE_OVRD_OPTIMIZED;
		intel_dp->hobl_active = is_hobl_buf_trans(trans);
		NBlue::callback->intel_de_rmw( ICL_PORT_CL_DW10(phy), val,
				 intel_dp->hobl_active ? val : 0);
	}*/

	
	val = NBlue::callback->readReg32( ICL_PORT_TX_DW5_LN(0, phy));
	val &= ~(SCALING_MODE_SEL_MASK | RTERM_SELECT_MASK |
		 COEFF_POLARITY | CURSOR_PROGRAM |
		 TAP2_DISABLE | TAP3_DISABLE);
	val |= SCALING_MODE_SEL(0x2);
	val |= RTERM_SELECT(0x6);
	val |= TAP3_DISABLE;
	NBlue::callback->writeReg32( ICL_PORT_TX_DW5_GRP(phy), val);


	for (ln = 0; ln < 4; ln++) {
		int level = intel_ddi_level(display, ln);

		NBlue::callback->intel_de_rmw( ICL_PORT_TX_DW2_LN(ln, phy),
				 SWING_SEL_UPPER_MASK | SWING_SEL_LOWER_MASK | RCOMP_SCALAR_MASK,
				 SWING_SEL_UPPER(trans->entries[level].icl.dw2_swing_sel) |
				 SWING_SEL_LOWER(trans->entries[level].icl.dw2_swing_sel) |
				 RCOMP_SCALAR(0x98));
	}


	for (ln = 0; ln < 4; ln++) {
		int level = intel_ddi_level(display, ln);

		NBlue::callback->intel_de_rmw( ICL_PORT_TX_DW4_LN(ln, phy),
				 POST_CURSOR_1_MASK | POST_CURSOR_2_MASK | CURSOR_COEFF_MASK,
				 POST_CURSOR_1(trans->entries[level].icl.dw4_post_cursor_1) |
				 POST_CURSOR_2(trans->entries[level].icl.dw4_post_cursor_2) |
				 CURSOR_COEFF(trans->entries[level].icl.dw4_cursor_coeff));
	}

	for (ln = 0; ln < 4; ln++) {
		int level = intel_ddi_level(display, ln);

		NBlue::callback->intel_de_rmw( ICL_PORT_TX_DW7_LN(ln, phy),
				 N_SCALAR_MASK,
				 N_SCALAR(trans->entries[level].icl.dw7_n_scalar));
	}
}


static void icl_combo_phy_set_signal_levels(struct intel_display *display)
{
	enum phy phy = PHY_A;
	u32 val;
	int ln;


	val = NBlue::callback->readReg32( ICL_PORT_PCS_DW1_LN(0, phy));
	//if (intel_crtc_has_type(crtc_state, INTEL_OUTPUT_HDMI))
	//	val &= ~COMMON_KEEPER_EN;
	//else
		val |= COMMON_KEEPER_EN;
	NBlue::callback->writeReg32( ICL_PORT_PCS_DW1_GRP(phy), val);


	for (ln = 0; ln < 4; ln++) {
		NBlue::callback->intel_de_rmw( ICL_PORT_TX_DW4_LN(ln, phy),
				 LOADGEN_SELECT,
				 icl_combo_phy_loadgen_select(ln));
	}

	NBlue::callback->intel_de_rmw( ICL_PORT_CL_DW5(phy),
			 0, SUS_CLOCK_CONFIG);

	val = NBlue::callback->readReg32( ICL_PORT_TX_DW5_LN(0, phy));
	val &= ~TX_TRAINING_EN;
	NBlue::callback->writeReg32( ICL_PORT_TX_DW5_GRP(phy), val);

	icl_ddi_combo_vswing_program(display);

	val = NBlue::callback->readReg32( ICL_PORT_TX_DW5_LN(0, phy));
	val |= TX_TRAINING_EN;
	NBlue::callback->writeReg32( ICL_PORT_TX_DW5_GRP(phy), val);
}


static int intel_dp_training_pattern_set_reg(struct intel_dp *intel_dp,enum drm_dp_phy dp_phy)
{
	return dp_phy == DP_PHY_DPRX ?
		DP_TRAINING_PATTERN_SET :
		DP_TRAINING_PATTERN_SET_PHY_REPEATER(dp_phy);
}

static inline u8 intel_dp_training_pattern_symbol(u8 pattern)
{
	return pattern & ~DP_LINK_SCRAMBLING_DISABLE;
}

u32 dp_tp_ctl_reg()
{
	struct intel_display *display = &NBlue::callback->display_base;
	return TGL_DP_TP_CTL(display, display->cpu_transcoder);
}
static void intel_ddi_set_link_train(struct intel_dp *intel_dp,u8 dp_train_pat)
{
	struct intel_display *display = &NBlue::callback->display_base;
	u32 temp;

	temp = NBlue::callback->readReg32( dp_tp_ctl_reg());

	temp &= ~DP_TP_CTL_LINK_TRAIN_MASK;
	switch (intel_dp_training_pattern_symbol(dp_train_pat)) {
	case DP_TRAINING_PATTERN_DISABLE:
		temp |= DP_TP_CTL_LINK_TRAIN_NORMAL;
		break;
	case DP_TRAINING_PATTERN_1:
		temp |= DP_TP_CTL_LINK_TRAIN_PAT1;
		break;
	case DP_TRAINING_PATTERN_2:
		temp |= DP_TP_CTL_LINK_TRAIN_PAT2;
		break;
	case DP_TRAINING_PATTERN_3:
		temp |= DP_TP_CTL_LINK_TRAIN_PAT3;
		break;
	case DP_TRAINING_PATTERN_4:
		temp |= DP_TP_CTL_LINK_TRAIN_PAT4;
		break;
	}
	NBlue::callback->writeReg32( dp_tp_ctl_reg(), temp);
}

void
intel_dp_program_link_training_pattern(struct intel_dp *intel_dp,enum drm_dp_phy dp_phy,
					   u8 dp_train_pat)
{
	u8 train_pat = intel_dp_training_pattern_symbol(dp_train_pat);
	intel_ddi_set_link_train( intel_dp,dp_train_pat);
}

static bool
intel_dp_set_link_train(struct intel_dp *intel_dp,enum drm_dp_phy dp_phy,
			u8 dp_train_pat)
{
	struct intel_display *display = &NBlue::callback->display_base;
	int reg = intel_dp_training_pattern_set_reg(intel_dp, dp_phy);
	u8 buf[sizeof(intel_dp->train_set) + 1];
	int len;

	intel_dp_program_link_training_pattern(intel_dp,
						   dp_phy, dp_train_pat);

	buf[0] = dp_train_pat;

	memcpy(buf + 1, intel_dp->train_set, display->panel.vbt.edp.lanes);
	len = display->panel.vbt.edp.lanes+ 1;

	return Gen11::callback->writeAUX(linkp,reg,buf, len)==0;
}

static bool
intel_dp_reset_link_train(struct intel_dp *intel_dp,enum drm_dp_phy dp_phy,
			  u8 dp_train_pat)
{
	memset(intel_dp->train_set, 0, sizeof(intel_dp->train_set));
	//intel_dp_set_signal_levels(intel_dp, crtc_state, dp_phy);
	icl_combo_phy_set_signal_levels(&NBlue::callback->display_base);
	return intel_dp_set_link_train( intel_dp,dp_phy, dp_train_pat);
}

static u8 dp_link_status(const u8 link_status[DP_LINK_STATUS_SIZE], int r)
{
	return link_status[r - DP_LANE0_1_STATUS];
}

static u8 dp_get_lane_status(const u8 link_status[DP_LINK_STATUS_SIZE],
				 int lane)
{
	int i = DP_LANE0_1_STATUS + (lane >> 1);
	int s = (lane & 1) * 4;
	u8 l = dp_link_status(link_status, i);

	return (l >> s) & 0xf;
}

bool drm_dp_clock_recovery_ok(const u8 link_status[DP_LINK_STATUS_SIZE],
				  int lane_count)
{
	int lane;
	u8 lane_status;

	for (lane = 0; lane < lane_count; lane++) {
		lane_status = dp_get_lane_status(link_status, lane);
		if ((lane_status & DP_LANE_CR_DONE) == 0)
			return false;
	}
	return true;
}


static u8 drm_dp_get_adjust_request_voltage(const u8 link_status[DP_LINK_STATUS_SIZE],
					 int lane)
{
	int i = DP_ADJUST_REQUEST_LANE0_1 + (lane >> 1);
	int s = ((lane & 1) ?
		 DP_ADJUST_VOLTAGE_SWING_LANE1_SHIFT :
		 DP_ADJUST_VOLTAGE_SWING_LANE0_SHIFT);
	u8 l = dp_link_status(link_status, i);

	return ((l >> s) & 0x3) << DP_TRAIN_VOLTAGE_SWING_SHIFT;
}

static u8 drm_dp_get_adjust_request_pre_emphasis(const u8 link_status[DP_LINK_STATUS_SIZE],
					  int lane)
{
	int i = DP_ADJUST_REQUEST_LANE0_1 + (lane >> 1);
	int s = ((lane & 1) ?
		 DP_ADJUST_PRE_EMPHASIS_LANE1_SHIFT :
		 DP_ADJUST_PRE_EMPHASIS_LANE0_SHIFT);
	u8 l = dp_link_status(link_status, i);

	return ((l >> s) & 0x3) << DP_TRAIN_PRE_EMPHASIS_SHIFT;
}



static u8 intel_dp_phy_preemph_max(struct intel_dp *intel_dp,
				   enum drm_dp_phy dp_phy)
{
	u8 preemph_max;

	preemph_max = DP_TRAIN_PRE_EMPH_LEVEL_3;

	return preemph_max;
}

static u8 dp_voltage_max(u8 preemph)
{
	switch (preemph & DP_TRAIN_PRE_EMPHASIS_MASK) {
	case DP_TRAIN_PRE_EMPH_LEVEL_0:
		return DP_TRAIN_VOLTAGE_SWING_LEVEL_3;
	case DP_TRAIN_PRE_EMPH_LEVEL_1:
		return DP_TRAIN_VOLTAGE_SWING_LEVEL_2;
	case DP_TRAIN_PRE_EMPH_LEVEL_2:
		return DP_TRAIN_VOLTAGE_SWING_LEVEL_1;
	case DP_TRAIN_PRE_EMPH_LEVEL_3:
	default:
		return DP_TRAIN_VOLTAGE_SWING_LEVEL_0;
	}
}

static u8 intel_ddi_dp_voltage_max(struct intel_display *display)
{
	
	int n_entries;
	const struct intel_ddi_buf_trans *trans;
	trans=tgl_get_combo_buf_trans_dp(display,&n_entries);

	return index_to_dp_signal_levels[n_entries - 1] &
		DP_TRAIN_VOLTAGE_SWING_MASK;
}

static u8 intel_dp_phy_voltage_max(struct intel_dp *intel_dp,
				   enum drm_dp_phy dp_phy)
{
	struct intel_display *display = &NBlue::callback->display_base;
	u8 voltage_max;


	//if (intel_dp_phy_is_downstream_of_source(intel_dp, dp_phy))
		voltage_max = intel_ddi_dp_voltage_max(display);
	//else
	//	voltage_max = intel_dp_lttpr_voltage_max(intel_dp, dp_phy + 1);


	return voltage_max;
}

static u8 intel_dp_get_lane_adjust_vswing_preemph(struct intel_dp *intel_dp, enum drm_dp_phy dp_phy,
						  const u8 link_status[DP_LINK_STATUS_SIZE],
						  int lane)
{
	u8 v = 0;
	u8 p = 0;
	u8 voltage_max;
	u8 preemph_max;

	if (1/*has_per_lane_signal_levels(intel_dp, dp_phy)*/) {
		lane = min(lane, NBlue::callback->display_base.panel.vbt.edp.lanes - 1);

		v = drm_dp_get_adjust_request_voltage(link_status, lane);
		p = drm_dp_get_adjust_request_pre_emphasis(link_status, lane);
	} /*else {
		for (lane = 0; lane < crtc_state->lane_count; lane++) {
			v = max(v, drm_dp_get_adjust_request_voltage(link_status, lane));
			p = max(p, drm_dp_get_adjust_request_pre_emphasis(link_status, lane));
		}
	}*/

	preemph_max = intel_dp_phy_preemph_max(intel_dp, dp_phy);
	if (p >= preemph_max)
		p = preemph_max | DP_TRAIN_MAX_PRE_EMPHASIS_REACHED;

	v = min(v, dp_voltage_max(p));

	voltage_max = intel_dp_phy_voltage_max(intel_dp, dp_phy);
	if (v >= voltage_max)
		v = voltage_max | DP_TRAIN_MAX_SWING_REACHED;

	return v | p;
}

static u8 intel_dp_get_lane_adjust_train(struct intel_dp *intel_dp, enum drm_dp_phy dp_phy,
					 const u8 link_status[DP_LINK_STATUS_SIZE],
					 int lane)
{
	/*if (intel_dp_is_uhbr(crtc_state))
		return intel_dp_get_lane_adjust_tx_ffe_preset(intel_dp, crtc_state,
								  dp_phy, link_status, lane);
	else*/
		return intel_dp_get_lane_adjust_vswing_preemph( intel_dp, dp_phy, link_status, lane);
}

bool
intel_dp_get_adjust_train(struct intel_dp *intel_dp,enum drm_dp_phy dp_phy,
			  const u8 link_status[DP_LINK_STATUS_SIZE])
{
	bool changed = false;
	int lane;


	for (lane = 0; lane < 4; lane++) {
		u8 new2 = intel_dp_get_lane_adjust_train(intel_dp, dp_phy, link_status, lane);
		if (intel_dp->train_set[lane] == new2)
			continue;

		intel_dp->train_set[lane] = new2;
		changed = true;
	}

	return changed;
}

static bool intel_dp_adjust_request_changed(const u8 old_link_status[DP_LINK_STATUS_SIZE],
						const u8 new_link_status[DP_LINK_STATUS_SIZE])
{
	int lane;

	for (lane = 0; lane < NBlue::callback->display_base.panel.vbt.edp.lanes; lane++) {
		u8 old, new2;

		/*if (intel_dp_is_uhbr(crtc_state)) {
			old = drm_dp_get_adjust_tx_ffe_preset(old_link_status, lane);
			new = drm_dp_get_adjust_tx_ffe_preset(new_link_status, lane);
		} else {*/
			old = drm_dp_get_adjust_request_voltage(old_link_status, lane) |
				drm_dp_get_adjust_request_pre_emphasis(old_link_status, lane);
			new2 = drm_dp_get_adjust_request_voltage(new_link_status, lane) |
				drm_dp_get_adjust_request_pre_emphasis(new_link_status, lane);
		//}

		if (old != new2)
			return true;
	}

	return false;
}

static bool intel_dp_lane_max_vswing_reached(u8 train_set_lane)
{
	u8 v = (train_set_lane & DP_TRAIN_VOLTAGE_SWING_MASK) >>
		DP_TRAIN_VOLTAGE_SWING_SHIFT;
	u8 p = (train_set_lane & DP_TRAIN_PRE_EMPHASIS_MASK) >>
		DP_TRAIN_PRE_EMPHASIS_SHIFT;

	if ((train_set_lane & DP_TRAIN_MAX_SWING_REACHED) == 0)
		return false;

	if (v + p != 3)
		return false;

	return true;
}

static bool intel_dp_link_max_vswing_reached(struct intel_dp *intel_dp)
{
	int lane;

	for (lane = 0; lane < NBlue::callback->display_base.panel.vbt.edp.lanes; lane++) {
		u8 train_set_lane = intel_dp->train_set[lane];

		/*if (intel_dp_is_uhbr(crtc_state)) {
			if (!intel_dp_lane_max_tx_ffe_reached(train_set_lane))
				return false;
		} else {*/
			if (!intel_dp_lane_max_vswing_reached(train_set_lane))
				return false;
		//}
	}

	return true;
}



static bool
intel_dp_update_link_train(struct intel_dp *intel_dp,
			   enum drm_dp_phy dp_phy)
{
	int reg = dp_phy == DP_PHY_DPRX ?
				DP_TRAINING_LANE0_SET :
				DP_TRAINING_LANE0_SET_PHY_REPEATER(dp_phy);
	int ret;

	//intel_dp_set_signal_levels(intel_dp, dp_phy);
	icl_combo_phy_set_signal_levels(&NBlue::callback->display_base);
	
	ret=Gen11::callback->writeAUX(linkp,reg,intel_dp->train_set, NBlue::callback->display_base.panel.vbt.edp.lanes);
	

	return ret == 0;//NBlue::callback->display_base.panel.vbt.edp.lanes;
}

static bool
intel_dp_link_training_clock_recovery(struct intel_dp *intel_dp,enum drm_dp_phy dp_phy)
{
	u8 old_link_status[DP_LINK_STATUS_SIZE] = {};
	int voltage_tries, cr_tries, max_cr_tries;
	u8 link_status[DP_LINK_STATUS_SIZE];
	bool max_vswing_reached = false;
	int delay_us;
	struct intel_display *display = &NBlue::callback->display_base;
	delay_us = 100;

	
	
	if (!intel_dp_reset_link_train( intel_dp,dp_phy,
					   DP_TRAINING_PATTERN_1 |
					   DP_LINK_SCRAMBLING_DISABLE)) {
		return false;
	}

	
		max_cr_tries = 80;

	voltage_tries = 1;
	for (cr_tries = 0; cr_tries < max_cr_tries; ++cr_tries) {
		IODelay(delay_us);
		
		
		Gen11::callback->readAUX(linkp,DP_LANE0_1_STATUS,link_status,DP_LINK_STATUS_SIZE);
		/*if (drm_dp_dpcd_read_phy_link_status(&intel_dp->aux, dp_phy,
							 link_status) < 0) {
			return false;
		}*/

		if (drm_dp_clock_recovery_ok(link_status, display->panel.vbt.edp.lanes)) {
			return true;
		}

		if (voltage_tries == 5) {
			//intel_dp_dump_link_status(intel_dp, dp_phy, link_status);
			return false;
		}

		if (max_vswing_reached) {
			//intel_dp_dump_link_status(intel_dp, dp_phy, link_status);
			return false;
		}

		intel_dp_get_adjust_train(intel_dp, dp_phy,link_status);
		if (!intel_dp_update_link_train(intel_dp, dp_phy)) {
			return false;
		}

		if (!intel_dp_adjust_request_changed(  old_link_status, link_status))
			++voltage_tries;
		else
			voltage_tries = 1;

		memcpy(old_link_status, link_status, sizeof(link_status));

		if (intel_dp_link_max_vswing_reached(intel_dp))
			max_vswing_reached = true;
	}

	//intel_dp_dump_link_status(intel_dp, dp_phy, link_status);


	return false;
}


static bool drm_dp_channel_eq_ok(const u8 link_status[DP_LINK_STATUS_SIZE],
			  int lane_count)
{
	u8 lane_align;
	u8 lane_status;
	int lane;

	lane_align = dp_link_status(link_status,
					DP_LANE_ALIGN_STATUS_UPDATED);
	if ((lane_align & DP_INTERLANE_ALIGN_DONE) == 0)
		return false;
	for (lane = 0; lane < lane_count; lane++) {
		lane_status = dp_get_lane_status(link_status, lane);
		if ((lane_status & DP_CHANNEL_EQ_BITS) != DP_CHANNEL_EQ_BITS)
			return false;
	}
	return true;
}



static bool
intel_dp_link_training_channel_equalization(struct intel_dp *intel_dp,enum drm_dp_phy dp_phy)
{
	int tries;
	u32 training_pattern;
	u8 link_status[DP_LINK_STATUS_SIZE];
	bool channel_eq = false;
	int delay_us;
	struct intel_display *display = &NBlue::callback->display_base;
	
	
	delay_us = 400;

	training_pattern = DP_TRAINING_PATTERN_2;

	if (training_pattern != DP_TRAINING_PATTERN_4)
		training_pattern |= DP_LINK_SCRAMBLING_DISABLE;


	if (!intel_dp_set_link_train(intel_dp,  dp_phy,
					 training_pattern)) {
		return false;
	}

	for (tries = 0; tries < 5; tries++) {
		IODelay(delay_us);

		Gen11::callback->readAUX(linkp,DP_LANE0_1_STATUS,link_status,DP_LINK_STATUS_SIZE);
		
		/*if (drm_dp_dpcd_read_phy_link_status(&intel_dp->aux, dp_phy,
							 link_status) < 0) {
			break;
		}*/


		if (!drm_dp_clock_recovery_ok(link_status,
									  display->panel.vbt.edp.lanes)) {
			//intel_dp_dump_link_status(intel_dp, dp_phy, link_status);
			break;
		}

		if (drm_dp_channel_eq_ok(link_status,
								 display->panel.vbt.edp.lanes)) {
			channel_eq = true;
			break;
		}


		intel_dp_get_adjust_train(intel_dp,  dp_phy,link_status);
		if (!intel_dp_update_link_train(intel_dp,  dp_phy)) {
			break;
		}
	}


	if (tries == 5) {
		//intel_dp_dump_link_status(intel_dp, dp_phy, link_status);
	}

	return channel_eq;
}

static bool
intel_dp_link_train_phy(struct intel_dp *intel_dp,enum drm_dp_phy dp_phy)
{
	bool ret = false;

	if (!intel_dp_link_training_clock_recovery( intel_dp,dp_phy))
		goto out;

	if (!intel_dp_link_training_channel_equalization( intel_dp,dp_phy))
		goto out;

	ret = true;

out:

	return ret;
}

static bool intel_dp_disable_dpcd_training_pattern(struct intel_dp *intel_dp,
						   enum drm_dp_phy dp_phy)
{
	int reg = intel_dp_training_pattern_set_reg(intel_dp, dp_phy);
	u8 val = DP_TRAINING_PATTERN_DISABLE;

	auto ret=Gen11::callback->writeAUX(linkp,reg,&val, 1);
	
	return ret == 0;
}

u8 drm_dp_link_rate_to_bw_code(int link_rate)
{
	switch (link_rate) {
	case 1000000:
		return DP_LINK_BW_10;
	case 1350000:
		return DP_LINK_BW_13_5;
	case 2000000:
		return DP_LINK_BW_20;
	default:
		/* Spec says link_bw = link_rate / 0.27Gbps */
		return link_rate / 27000;
	}
}

static void intel_ddi_init_dp_buf_reg(struct intel_dp *intel_dp)
{
	struct intel_display *display = &NBlue::callback->display_base;
	

	intel_dp->DP = DDI_PORT_WIDTH(display->panel.vbt.edp.lanes) |
		DDI_BUF_TRANS_SELECT(0);

	if (display->child0->lane_reversal)
		intel_dp->DP |= DDI_BUF_PORT_REVERSAL;
	
/*	if (dig_port->ddi_a_4_lanes)
		intel_dp->DP |= DDI_A_4_LANES;
*/
	/*if (DISPLAY_VER(display) >= 14) {
		if (intel_dp_is_uhbr(crtc_state))
			intel_dp->DP |= DDI_BUF_PORT_DATA_40BIT;
		else
			intel_dp->DP |= DDI_BUF_PORT_DATA_10BIT;
	}*/


	/*if (IS_DISPLAY_VER(display, 11, 13) && intel_encoder_is_tc(encoder)) {
		int delay = dp_phy_lane_stagger_delay(crtc_state->port_clock);

		intel_dp->DP |= DDI_BUF_LANE_STAGGER_DELAY(delay);
	}*/
}


static void intel_ddi_buf_enable( u32 buf_ctl)
{
	struct intel_display *display = &NBlue::callback->display_base;
	enum port port = display->port0;

	NBlue::callback->writeReg32( DDI_BUF_CTL(port), buf_ctl | DDI_BUF_CTL_ENABLE);
	NBlue::callback->readReg32( DDI_BUF_CTL(port));

	IOSleep(100);
	//intel_wait_ddi_buf_active(encoder);
}

static void intel_ddi_prepare_link_retrain(struct intel_dp *intel_dp)
{
	struct intel_display *display = &NBlue::callback->display_base;
	u32 dp_tp_ctl;

	dp_tp_ctl = NBlue::callback->readReg32( dp_tp_ctl_reg());


	dp_tp_ctl = DP_TP_CTL_ENABLE | DP_TP_CTL_LINK_TRAIN_PAT1;
	/*if (intel_crtc_has_type(crtc_state, INTEL_OUTPUT_DP_MST) ||
		intel_dp_is_uhbr(crtc_state)) {
		dp_tp_ctl |= DP_TP_CTL_MODE_MST;
	} else {*/
		dp_tp_ctl |= DP_TP_CTL_MODE_SST;
		//if (crtc_state->enhanced_framing)
			dp_tp_ctl |= DP_TP_CTL_ENHANCED_FRAME_ENABLE;
	//}
	NBlue::callback->writeReg32( dp_tp_ctl_reg(), dp_tp_ctl);
	NBlue::callback->readReg32( dp_tp_ctl_reg());

	/*if (display->platform.alderlake_p &&
		(intel_tc_port_in_dp_alt_mode(dig_port) || intel_tc_port_in_legacy_mode(dig_port)))
		adlp_tbt_to_dp_alt_switch_wa(encoder);*/

	intel_ddi_buf_enable( intel_dp->DP);
	intel_dp->DP |= DDI_BUF_CTL_ENABLE;
}

static void _icl_ddi_enable_clock(struct intel_display *display, u32 reg,
				  u32 clk_sel_mask, u32 clk_sel, u32 clk_off)
{
	
	IOSimpleLock *myLock;
	myLock = IOSimpleLockAlloc();
	IOSimpleLockLock(myLock);

	NBlue::callback->intel_de_rmw( reg, clk_sel_mask, clk_sel);
	NBlue::callback->intel_de_rmw( reg, clk_off, 0);

	IOSimpleLockUnlock(myLock);
	IOSimpleLockFree(myLock);
}

void intel_combo_phy_power_up_lanes(struct intel_display *display,
					enum phy phy, bool is_dsi,
					int lane_count, bool lane_reversal)
{
	u8 lane_mask;

	if (is_dsi) {

		switch (lane_count) {
		/*case 1:
			lane_mask = PWR_DOWN_LN_3_1_0;
			break;
		case 2:
			lane_mask = PWR_DOWN_LN_3_1;
			break;
		case 3:
			lane_mask = PWR_DOWN_LN_3;
			break;*/
		default:
		case 4:
			lane_mask = PWR_UP_ALL_LANES;
			break;
		}
	} else {
		switch (lane_count) {
		/*case 1:
			lane_mask = lane_reversal ? PWR_DOWN_LN_2_1_0 :
							PWR_DOWN_LN_3_2_1;
			break;*/
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

int drm_dp_pcon_convert_rgb_to_ycbcr(u8 color_spc)
{
	int ret;
	u8 buf;

	ret = Gen11::callback->readAUX(linkp,DP_PROTOCOL_CONVERTER_CONTROL_2,&buf,1);
	if (ret != 0)
		return ret;

	if (color_spc & DP_CONVERSION_RGB_YCBCR_MASK)
		buf |= (color_spc & DP_CONVERSION_RGB_YCBCR_MASK);
	else
		buf &= ~DP_CONVERSION_RGB_YCBCR_MASK;

	return Gen11::callback->writeAUX(linkp,DP_PROTOCOL_CONVERTER_CONTROL_2, &buf,1);
}

void intel_dp_configure_protocol_converter(struct intel_dp *intel_dp)
{
	struct intel_display *display = &NBlue::callback->display_base;
	bool ycbcr444_to_420 = false;
	bool rgb_to_ycbcr = false;
	u8 tmp;

	//tmp = intel_dp_has_hdmi_sink(intel_dp) ? DP_HDMI_DVI_OUTPUT_CONFIG : 0;
	tmp = 0;
	
	Gen11::callback->writeAUX(linkp,DP_PROTOCOL_CONVERTER_CONTROL_0, &tmp,1);


	if (display->sink_format == INTEL_OUTPUT_FORMAT_YCBCR420) {
		switch (display->output_format) {
		case INTEL_OUTPUT_FORMAT_YCBCR420:
			break;
		case INTEL_OUTPUT_FORMAT_YCBCR444:
			ycbcr444_to_420 = true;
			break;
		case INTEL_OUTPUT_FORMAT_RGB:
			rgb_to_ycbcr = true;
			ycbcr444_to_420 = true;
			break;
		default:
			break;
		}
	} else if (display->sink_format == INTEL_OUTPUT_FORMAT_YCBCR444) {
		switch (display->output_format) {
		case INTEL_OUTPUT_FORMAT_YCBCR444:
			break;
		case INTEL_OUTPUT_FORMAT_RGB:
			rgb_to_ycbcr = true;
			break;
		default:
			break;
		}
	}

	tmp = ycbcr444_to_420 ? DP_CONVERSION_TO_YCBCR420_ENABLE : 0;
	Gen11::callback->writeAUX(linkp,DP_PROTOCOL_CONVERTER_CONTROL_1, &tmp,1);


	tmp = rgb_to_ycbcr ? DP_CONVERSION_BT709_RGB_YCBCR_ENABLE : 0;
	
	drm_dp_pcon_convert_rgb_to_ycbcr(tmp);


}




static int psr_get_status_and_error_status(struct intel_dp *intel_dp,
					   u8 *status, u8 *error_status)
{
	int ret;
	unsigned int offset;

	offset = /*intel_dp->psr.panel_replay_enabled ?
		 DP_SINK_DEVICE_PR_AND_FRAME_LOCK_STATUS :*/ DP_PSR_STATUS;

	ret=Gen11::callback->readAUX(linkp,offset, status,1);
	
	if (ret != 0)
		return ret;

	offset = /*intel_dp->psr.panel_replay_enabled ?
		 DP_PANEL_REPLAY_ERROR_STATUS :*/ DP_PSR_ERROR_STATUS;

	ret=Gen11::callback->readAUX(linkp,offset, error_status,1);
	if (ret != 0)
		return ret;

	*status = *status & DP_PSR_SINK_STATE_MASK;

	return 0;
}




static void intel_psr_exit(struct intel_dp *intel_dp)
{
	struct intel_display *display = &NBlue::callback->display_base;


	/*gen9_set_dc_state(display, DC_STATE_EN_UPTO_DC6);

	NBlue::callback->intel_de_rmw( EDP_PSR2_CTL(display, display->cpu_transcoder),
			 EDP_PSR2_IDLE_FRAMES_MASK,
			 EDP_PSR2_IDLE_FRAMES(0));*/
	
	NBlue::callback->intel_de_rmw(
					EDP_PSR_CTL(display, display->cpu_transcoder),
				   EDP_PSR_ENABLE, 0);

}

static void intel_psr_wait_exit_locked(struct intel_dp *intel_dp)
{
	struct intel_display *display = &NBlue::callback->display_base;
	enum transcoder cpu_transcoder = display->cpu_transcoder;
	u32 psr_status;
	u32 psr_status_mask;


		psr_status = EDP_PSR_STATUS(display, cpu_transcoder);
		psr_status_mask = EDP_PSR_STATUS_STATE_MASK;
	
	IOSleep(2000);

}

static void intel_psr_disable_locked(struct intel_dp *intel_dp)
{
	struct intel_display *display = &NBlue::callback->display_base;


	intel_psr_exit(intel_dp);
	intel_psr_wait_exit_locked(intel_dp);


	if (DISPLAY_VER(display) >= 11)
		NBlue::callback->intel_de_rmw( GEN8_CHICKEN_DCPR_1,
				 LATENCY_REPORTING_REMOVED(display->pipe0), 0);


	u8 val=0;

	Gen11::callback->writeAUX(linkp,DP_PSR_EN_CFG, &val,1);

}

static void psr_capability_changed_check(struct intel_dp *intel_dp)
{
	struct intel_display *display = &NBlue::callback->display_base;
	u8 val;
	int r;

	
	r=Gen11::callback->readAUX(linkp,DP_PSR_ESI, &val,1);
	if (r != 0) {
		return;
	}

	if (val & DP_PSR_CAPS_CHANGE) {
		intel_psr_disable_locked(intel_dp);
		Gen11::callback->writeAUX(linkp,DP_PSR_ESI, &val,1);
	}
}



static void intel_dp_enable_port(struct intel_dp *intel_dp)
{
	struct intel_display *display = &NBlue::callback->display_base;
	intel_dp_program_link_training_pattern(intel_dp,
						   DP_PHY_DPRX, DP_TRAINING_PATTERN_1);

	intel_dp->DP |= DP_PORT_EN;
	NBlue::callback->writeReg32( intel_dp->output_reg, intel_dp->DP);
	NBlue::callback->readReg32( intel_dp->output_reg);
}

static int
write_dsc_decompression_flag( u8 flag, bool set)
{
	int err;
	u8 val;

	err = Gen11::callback->readAUX(linkp,DP_DSC_ENABLE,&val,1);
	if (err < 0)
		return err;

	if (set)
		val |= flag;
	else
		val &= ~flag;
	
	return Gen11::callback->writeAUX(linkp,DP_DSC_ENABLE,&val,1);
}



uint64_t  Gen11::linkTraining(void *that,void *param_1)
{
	linkp=that;
	struct intel_display *display=&NBlue::callback->display_base;
	int port_clock= display->port_clock;
	int lane_count=display->panel.vbt.edp.lanes;
	enum phy phy=display->phy0;
	enum port port=display->port0;
	struct intel_dp *intel_dp=&display->intel_dp0;
	//hsw_get_pipe_config
	
	memset(intel_dp->train_set, 0, sizeof(intel_dp->train_set));
	intel_dp->link_rate = port_clock;
	intel_dp->lane_count = lane_count;
	intel_dp->output_reg = DDI_BUF_CTL(port);
	
	intel_ddi_init_dp_buf_reg(intel_dp);
	
	intel_dp_enable_port(intel_dp);

	enableVDDForAux(ccont2,that);
	hwSetPanelPower(ccont2,2);
	disableVDDForAux(ccont2);
	
	
	
	_icl_ddi_enable_clock(display, ICL_DPCLKA_CFGCR0,
				  ICL_DPCLKA_CFGCR0_DDI_CLK_SEL_MASK(phy),
				  ICL_DPCLKA_CFGCR0_DDI_CLK_SEL(DPLL_ID_ICL_DPLL0, phy),
				  ICL_DPCLKA_CFGCR0_DDI_CLK_OFF(phy));

	
	
	u32 val;
	val = TGL_TRANS_CLK_SEL_PORT(port);
	NBlue::callback->writeReg32( TRANS_CLK_SEL(display->cpu_transcoder), val);
	u32 ctl;

	ctl = NBlue::callback->readReg32(TRANS_DDI_FUNC_CTL(display, display->cpu_transcoder));
	ctl &= ~TRANS_DDI_FUNC_ENABLE;
	NBlue::callback->writeReg32( TRANS_DDI_FUNC_CTL(display, display->cpu_transcoder),
			   ctl);
	
	icl_combo_phy_set_signal_levels(display);
	
	intel_combo_phy_power_up_lanes(display, phy, false,
					   lane_count, display->child0->lane_reversal);

	
	u32 dss1 = 0;
	NBlue::callback->intel_de_rmw( ICL_PIPE_DSS_CTL1(display->pipe0),
			 SPLITTER_ENABLE | SPLITTER_CONFIGURATION_MASK |
			 OVERLAP_PIXELS_MASK, dss1);
	
	u8 oui[] = { 0x00, 0xaa, 0x01 };
	u8 buf[3] = {};
	
	readAUX(linkp,DP_SOURCE_OUI,buf,sizeof(buf));

	if (memcmp(oui, buf, sizeof(oui)) == 0) {

	}
	else
		writeAUX(linkp,DP_SOURCE_OUI,oui,sizeof(oui));
	
	u8 mode = DP_SET_POWER_D0;
	for (int i = 0; i < 3; i++) {
		int ret = writeAUX(linkp,DP_SET_POWER,&mode,1);
		if (ret == 0)
			break;
		IODelay(1);
	}
	
	intel_dp_configure_protocol_converter(intel_dp);
	
	u32 dss_ctl2;
	dss_ctl2 = NBlue::callback->readReg32( ICL_PIPE_DSS_CTL2(display->pipe0));
	//compression_enable
	if (dss_ctl2 & VDSC0_ENABLE) {
		write_dsc_decompression_flag(DP_DSC_PASSTHROUGH_EN, true);
		write_dsc_decompression_flag( DP_DECOMPRESSION_EN, true);
	}

	
	const u8 errors = DP_PSR_RFB_STORAGE_ERROR |
			  DP_PSR_VSC_SDP_UNCORRECTABLE_ERROR |
			  DP_PSR_LINK_CRC_ERROR;
	u8 status, error_status;
	psr_get_status_and_error_status(intel_dp, &status, &error_status);
	if (status == DP_PSR_SINK_INTERNAL_ERROR ||
		(error_status & errors)) {
		intel_psr_disable_locked(intel_dp);
	}
	writeAUX(linkp,DP_PSR_ERROR_STATUS,&error_status, 1);
	psr_capability_changed_check(intel_dp);

	//intel_hpd_block
	
	intel_ddi_prepare_link_retrain(intel_dp);
	
	u8 link_config[2];
	bool is_vrr=(DISPLAY_VER(display) >= 11);
	
	//drm_dp_sink_can_do_video_without_timing_msa
	link_config[0] = is_vrr ? DP_MSA_TIMING_PAR_IGNORE_EN : 0;
	link_config[1] = DP_SET_ANSI_8B10B;
	writeAUX(linkp,DP_DOWNSPREAD_CTRL,link_config, 2);
	
	u8 link_bw;
	link_bw = drm_dp_link_rate_to_bw_code(port_clock);
	
	//DP and eDP v1.3 and earlier link bw set method.
	link_config[0] = link_bw;
	link_config[1] = lane_count;
	link_config[1] |= DP_LANE_COUNT_ENHANCED_FRAME_EN;
	
	writeAUX(linkp,DP_LINK_BW_SET,link_config, ARRAY_SIZE(link_config));
		
	
	bool ret= intel_dp_link_train_phy(intel_dp,DP_PHY_DPRX);
	
	intel_dp_disable_dpcd_training_pattern(intel_dp, DP_PHY_DPRX);
	
	NBlue::callback->intel_de_rmw( TGL_DP_TP_CTL(display, display->cpu_transcoder),
			 DP_TP_CTL_LINK_TRAIN_MASK, DP_TP_CTL_LINK_TRAIN_IDLE);
	
	//intel_ddi_set_dp_msa
	//intel_hpd_unblock(encoder);
	
	return 0; // hack
	
	if (ret) return 0;
	panic("x");
	return -1;//FunctionCast(linkTraining, callback->olinkTraining)(that,param_1);
}


uint64_t Gen11::hwSetPanelPower(void *that,uint param_1)
{
	if (IS_DISPLAY_VER(&NBlue::callback->display_base, 13, 14))
		NBlue::callback->intel_de_rmw( SOUTH_DSPCLK_GATE_D,
				 0, PCH_DPLSUNIT_CLOCK_GATE_DISABLE);
	
	auto ret= FunctionCast(hwSetPanelPower, callback->ohwSetPanelPower)(that,param_1);
	
	if (IS_DISPLAY_VER(&NBlue::callback->display_base, 13, 14))
		NBlue::callback->intel_de_rmw( SOUTH_DSPCLK_GATE_D,
				 PCH_DPLSUNIT_CLOCK_GATE_DISABLE, 0);
	
	return ret;
};

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
