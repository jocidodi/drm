
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
			
			/*{"__ZN31AppleIntelFramebufferController16hwRegsNeedUpdateEP21AppleIntelFramebufferP21AppleIntelDisplayPathP10CRTCParamsPK29IODetailedTimingInformationV2PN16AppleIntelScaler12SCALERPARAMSE",hwRegsNeedUpdate, this->ohwRegsNeedUpdate},
			{"__ZN21AppleIntelFramebuffer31frameBufferNotificationcallbackEP8OSObjectPvP13IOFramebufferiS2_",aframeBufferNotificationcallback, this->oaframeBufferNotificationcallback},
			{"__ZN31AppleIntelFramebufferController9hwSetModeEP21AppleIntelFramebufferP21AppleIntelDisplayPathiPK29IODetailedTimingInformationV2",hwSetMode, this->ohwSetMode},
			*/
			//{"__ZN21AppleIntelFramebuffer12setAttributeEjm",fsetAttribute, this->ofsetAttribute},
			
			
		};
		PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route symbols");
		
		//linktrainig 2 lines
		static const uint8_t f25[]= {0x77, 0x77, 0x00, 0x00};
		static const uint8_t r25[]= {0x77, 0x00, 0x00, 0x00};
		
		//TRANS_CLK_SE
		static const uint8_t f6c[]= {0xc1, 0xe0, 0x1d, 0x41, 0x81, 0xe1, 0xff, 0xff, 0xff, 0x1f};
		static const uint8_t r6c[]= {0xc1, 0xe0, 0x1c, 0x41, 0x81, 0xe1, 0xff, 0xff, 0xff, 0x1f};
		
		static const uint8_t f6c2[]= {0xb8, 0x00, 0x00, 0x00, 0x00, 0x74, 0x04};
		static const uint8_t r6c2[]= {0xb8, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90};
		
		//force frame 0
		static const uint8_t f7[]= {0x80, 0x7d, 0xd7, 0x00, 0x75, 0x23};
		static const uint8_t r7[]= {0x80, 0x7d, 0xd7, 0x00, 0xeb, 0x23};
		
		static const uint8_t f7b[]= {0x8b, 0x40, 0x08, 0x85, 0xc0, 0x74, 0x51};
		static const uint8_t r7b[]= {0x8b, 0x40, 0x08, 0x85, 0xc0, 0xeb, 0x51};
		
		static const uint8_t f7c[]= {0x83, 0x78, 0x08, 0x00, 0x75, 0x46};
		static const uint8_t r7c[]= {0x83, 0x78, 0x08, 0x00, 0x90, 0x90};
		
		static const uint8_t f7d[]= {0x83, 0x78, 0x08, 0x00, 0x75, 0x37};
		static const uint8_t r7d[]= {0x83, 0x78, 0x08, 0x00, 0x90, 0x90};
		
		//TRANS_
		static const uint8_t f8[]= {0xff, 0xc9, 0x83, 0xf9, 0x04, 0x77, 0x1c, 0x48, 0x8d, 0x35, 0xe9, 0x03, 0x00, 0x00, 0x48, 0x63, 0x0c, 0x8e, 0x48, 0x01, 0xf1, 0xff, 0xe1, 0x25, 0xff, 0xff, 0xff, 0x8f, 0x0d, 0x00, 0x00, 0x00, 0x10};
		static const uint8_t r8[]= {0x90, 0x90, 0x83, 0xf9, 0x04, 0x90, 0x90, 0x48, 0x8d, 0x35, 0xe9, 0x03, 0x00, 0x00, 0x48, 0x63, 0x0c, 0x8e, 0x48, 0x01, 0xf1, 0x90, 0x90, 0x25, 0xff, 0xff, 0xff, 0x8f, 0x0d, 0x00, 0x00, 0x00, 0x08};
		
		static const uint8_t f8b[]= {0x41, 0x81, 0xce, 0x24, 0x00, 0x00, 0x80};
		static const uint8_t r8b[]= {0x41, 0x81, 0xce, 0x24, 0x00, 0x00, 0xC0};
		
		//builtin
		static const uint8_t f9[]= {0x48, 0x8b, 0xb8, 0x40, 0x04, 0x00, 0x00, 0xf6, 0x47, 0x14, 0x08, 0x75, 0x0a};
		static const uint8_t r9[]= {0x48, 0x8b, 0xb8, 0x40, 0x04, 0x00, 0x00, 0xf6, 0x47, 0x14, 0x08, 0xeb, 0x0a};
		

		//getHPDState register
		static const uint8_t f19[]= {0xbe, 0xa0, 0x38, 0x16, 0x00};
		static const uint8_t r19[]= {0xbe, 0x70, 0x44, 0x04, 0x00};
		
		LookupPatchPlus const patches[] = {
			{&kextG11FB, f25, r25, arrsize(f25),    7},
			{&kextG11FB, f6c, r6c, arrsize(f6c),    1},
			{&kextG11FB, f6c2, r6c2, arrsize(f6c2),    1},
			{&kextG11FB, f7, r7, arrsize(f7),    1},
			{&kextG11FB, f7b, r7b, arrsize(f7b),    1},
			{&kextG11FB, f7c, r7c, arrsize(f7c),    1},
			{&kextG11FB, f7d, r7d, arrsize(f7d),    1},
			{&kextG11FB, f8, r8, arrsize(f8),    1},
			{&kextG11FB, f8b, r8b, arrsize(f8b),    1},
			{&kextG11FB, f9, r9, arrsize(f9),    1},
			{&kextG11FB, f19, r19, arrsize(f19),    5},
			
			
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
			
			
		};
		PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route dp symbols");
		
		if (isprod) {
			RouteRequestPlus requests[] = {
				{"__ZN31AppleIntelFramebufferController21probeCDClockFrequencyEv",wrapProbeCDClockFrequency,	this->orgProbeCDClockFrequency},
				{"__ZN31AppleIntelFramebufferController17updateSliceConfigEj",updateSliceConfig, this->oupdateSliceConfig},
				{"__ZN31AppleIntelFramebufferController18setAsyncSliceCountE13IGSliceConfig",setAsyncSliceCount, this->osetAsyncSliceCount},
				{"__ZN31AppleIntelFramebufferController9hwGetCRTCEP21AppleIntelFramebufferP21AppleIntelDisplayPath",hwGetCRTC, this->ohwGetCRTC},
				{"__ZN31AppleIntelFramebufferController21hwSetPanelPowerConfigEj", hwSetPanelPowerConfig,this->ohwSetPanelPowerConfig},
				{"__ZN31AppleIntelFramebufferController15enableVDDForAuxEP14AppleIntelPort",enableVDDForAux, this->oenableVDDForAux},
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
				{"__ZN24AppleIntelBaseController21probeCDClockFrequencyEv",wrapProbeCDClockFrequency,	this->orgProbeCDClockFrequency},
				{"__ZN24AppleIntelBaseController17updateSliceConfigEj",updateSliceConfig, this->oupdateSliceConfig},
				{"__ZN24AppleIntelBaseController18setAsyncSliceCountE13IGSliceConfig",setAsyncSliceCount, this->osetAsyncSliceCount},
				{"__ZN24AppleIntelBaseController9hwGetCRTCEP21AppleIntelFramebufferP21AppleIntelDisplayPath",hwGetCRTC, this->ohwGetCRTC},
				{"__ZN24AppleIntelBaseController21hwSetPanelPowerConfigEj", hwSetPanelPowerConfig,this->ohwSetPanelPowerConfig},
				{"__ZN24AppleIntelBaseController15enableVDDForAuxEP14AppleIntelPort",enableVDDForAux, this->oenableVDDForAux},
				{"__ZN21AppleIntelFramebuffer4initEP24AppleIntelBaseControllerj",AppleIntelFramebufferinit, this->oAppleIntelFramebufferinit},
				{"__ZN24AppleIntelBaseController13FBMemMgr_InitEv", FBMemMgr_Init,this->oFBMemMgr_Init},
				{"__ZN24AppleIntelBaseController23initPlatformWorkaroundsEv",initPlatformWorkarounds, this->oinitPlatformWorkarounds},
				{"__ZN24AppleIntelBaseController16getOSInformationEv",getOSInformation, this->ogetOSInformation},
				{"__ZN24AppleIntelBaseController18hwInitializeCStateEv",hwInitializeCState, this->ohwInitializeCState},
				{"__ZN24AppleIntelBaseController20hwConfigureCustomAUXEb",hwConfigureCustomAUX, this->ohwConfigureCustomAUX},
				
			};
			PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route d symbols");
			
		}
		
		//regs
		static const uint8_t f4[]= {0x83, 0x78, 0x08, 0x00, 0x75, 0x71};
		static const uint8_t r4[]= {0x83, 0x78, 0x08, 0x00, 0x90, 0x90};
		
		static const uint8_t f4a[]= {0x83, 0x78, 0x08, 0x00, 0x0f, 0x84, 0xe9, 0x06, 0x00, 0x00};
		static const uint8_t r4a[]= {0x83, 0x78, 0x08, 0x00, 0x0f, 0x85, 0xe9, 0x06, 0x00, 0x00};
		
		static const uint8_t f4ap[]= {0x83, 0x78, 0x08, 0x00, 0x75, 0x50};
		static const uint8_t r4ap[]= {0x83, 0x78, 0x08, 0x00, 0x90, 0x90};
		
		
		
		//dbuff
		static const uint8_t f5[]= {0x74, 0x5a, 0xc7, 0x83, 0x48, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		static const uint8_t r5[]= {0x90, 0x90, 0xc7, 0x83, 0x48, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		
		//edp
		static const uint8_t f6a[]= {0x74, 0x2a, 0x83, 0xf8, 0x01, 0x74, 0x43, 0x85, 0xc0, 0x75, 0x60};
		static const uint8_t r6a[]= {0x90, 0x90, 0x83, 0xf8, 0x01, 0x90, 0x90, 0x85, 0xc0, 0x90, 0x90};

		static const uint8_t f6ap[]= {0x74, 0x2a, 0x83, 0xf8, 0x01, 0x74, 0x43, 0x85, 0xc0, 0x75, 0x60};
		static const uint8_t r6ap[]= {0x90, 0x90, 0x83, 0xf8, 0x01, 0x90, 0x90, 0x85, 0xc0, 0x90, 0x90};
		
		//TRANS_CLK_SE
		static const uint8_t f6c[]= {0x83, 0x79, 0x08, 0x00, 0x74, 0x6c};
		static const uint8_t r6c[]= {0x83, 0x79, 0x08, 0x00, 0x90, 0x90};

		static const uint8_t f6cp[]= {0x41, 0x8b, 0x49, 0x08, 0x85, 0xc9, 0x74, 0x3f};
		static const uint8_t r6cp[]= {0x41, 0x8b, 0x49, 0x08, 0x85, 0xc9, 0x90, 0x90};
		
		
		//ReadRegister64
		static const uint8_t f7[]= {0x83, 0xc0, 0xfc, 0x48, 0x39, 0xf0, 0x76, 0x11, 0x48, 0x8b, 0x47, 0x50, 0x48, 0xff, 0x05, 0xca, 0xf5, 0x0c, 0x00};
		static const uint8_t r7[]= {0x83, 0xc0, 0xf8, 0x48, 0x39, 0xf0, 0x76, 0x11, 0x48, 0x8b, 0x47, 0x50, 0x48, 0xff, 0x05, 0xca, 0xf5, 0x0c, 0x00};
		
		static const uint8_t f7p[]= {0x83, 0xc0, 0xfc, 0x48, 0x39, 0xf0, 0x76, 0x11, 0x48, 0x8b, 0x47, 0x50, 0x48, 0xff, 0x05, 0x84, 0x40, 0x08, 0x00};
		static const uint8_t r7p[]= {0x83, 0xc0, 0xf8, 0x48, 0x39, 0xf0, 0x76, 0x11, 0x48, 0x8b, 0x47, 0x50, 0x48, 0xff, 0x05, 0x84, 0x40, 0x08, 0x00};

		
		
		//TRANS_
		static const uint8_t f8[]= {0x81, 0xe1, 0x00, 0x00, 0x03, 0x00, 0xb8, 0xff, 0xff, 0xfc, 0x7f};
		static const uint8_t r8[]= {0x81, 0xe1, 0x00, 0x00, 0x03, 0x00, 0xb8, 0xff, 0xfe, 0xfc, 0x7f};
		
		static const uint8_t f8p[]= {0x09, 0xca, 0xb9, 0xff, 0xff, 0xfc, 0x7f};
		static const uint8_t r8p[]= {0x09, 0xca, 0xb9, 0xff, 0xfe, 0xfc, 0x7f};
		
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

		
		//linktrainig 2 lines
		static const uint8_t f25[]= {0x77, 0x77, 0x00, 0x00};
		static const uint8_t r25[]= {0x33, 0x00, 0x00, 0x00};



		
		if (isprod){
			LookupPatchPlus const patchesp[] = {// tgl production kext
				
				{&kextG11FBT, f4, r4, arrsize(f4),	1},
				{&kextG11FBT, f4ap, r4ap, arrsize(f4ap),	1},
				{&kextG11FBT, f5, r5, arrsize(f5),	1},
				{&kextG11FBT, f6ap, r6ap, arrsize(f6ap),    1},
				{&kextG11FBT, f6cp, r6cp, arrsize(f6cp),    1},
				{&kextG11FBT, f7p, r7p, arrsize(f7p),	1},
				//{&kextG11FBT, f8p, r8p, arrsize(f8p),	1},
				{&kextG11FBT, f9p, r9p, arrsize(f9p),	1},
				{&kextG11FBT, f13p, r13p, arrsize(f13p),	1},
				{&kextG11FBT, f13pb, r13pb, arrsize(f13pb),	1},
				{&kextG11FBT, f20p, r20p, arrsize(f20p),	1},
				{&kextG11FBT, f25, r25, arrsize(f25),	6},

			};
			
			PANIC_COND(!LookupPatchPlus::applyAll(patcher, patchesp , address, size), "nblue", "kextG11FBT Failed to apply production patches!");
		}
		else {
			LookupPatchPlus const patches[] = {// tgl debug kext
				
				{&kextG11FBT, f4, r4, arrsize(f4),	1},
				{&kextG11FBT, f4a, r4a, arrsize(f4a),	1},
				{&kextG11FBT, f5, r5, arrsize(f5),	1},
				{&kextG11FBT, f6a, r6a, arrsize(f6a),    1},
				{&kextG11FBT, f6c, r6c, arrsize(f6c),    1},
				{&kextG11FBT, f7, r7, arrsize(f7),	1},
				//{&kextG11FBT, f8, r8, arrsize(f8),	1},
				{&kextG11FBT, f9, r9, arrsize(f9),	1},
				{&kextG11FBT, f13, r13, arrsize(f13),	1},
				{&kextG11FBT, f13b, r13b, arrsize(f13b),	1},
				{&kextG11FBT, f15, r15, arrsize(f15),	1},
				{&kextG11FBT, f20, r20, arrsize(f20),	1},
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
	
	getMember<uint8_t>(that, 0x1b36)=1; //dither off
	
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
	
	getMember<uint8_t>(that, 0x1b12)=1; //dither off
	
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
	
	OSArray *connectorArray = OSArray::withCapacity(6);
	for (int i = 0; i < 6; i++) {
		OSDictionary *connectorDict = OSDictionary::withCapacity(10);
		connectorDict->setObject("Index", OSNumber::withNumber(pinfo[1].connectors[i].index, 32));
		connectorDict->setObject("busId", OSNumber::withNumber(pinfo[1].connectors[i].busId, 32));
		connectorDict->setObject("pipe", OSNumber::withNumber(pinfo[1].connectors[i].pipe, 32));
		connectorDict->setObject("pad", OSNumber::withNumber(pinfo[1].connectors[i].pad, 32));
		connectorDict->setObject("type", OSNumber::withNumber(pinfo[1].connectors[i].type, 32));
		connectorDict->setObject("flags", OSNumber::withNumber(pinfo[1].connectors[i].flags, 32));
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

	if (hwu==1){
		hwu=0;
		return 1;
	}
	return ret;
}

int Gen11::getTranscoderOffset(void *that,void *param_1,uint param_2)
{
	FunctionCast(getTranscoderOffset, callback->ogetTranscoderOffset)(that,param_1,param_2 );
	return 0;
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

void Gen11::hwInitializeCState(void *that)
{
	
	if (getMember<int>(that, kexticl ? 0xb38 : 0xb48) != 1) return;
	
	gen9_set_dc_state(&NBlue::callback->display_base,DC_STATE_DISABLE);
	
	NBlue::callback->intel_de_rmw( SOUTH_DSPCLK_GATE_D, 0,PCH_DPMGUNIT_CLOCK_GATE_DISABLE);
	
	if (DISPLAY_VER(&NBlue::callback->display_base)== 12){
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

	if (!intel_dmc_has_payload(&NBlue::callback->display_base)) {
		return ;
	}
	
	
	for (uint8_t i = 0; i < DMC_FW_MAX; i++) {
		if (!dmc->dmc_info[i].present)
			continue;
		dmc_load_program(&NBlue::callback->display_base, static_cast<intel_dmc_id>(i));
	}
	
	
	NBlue::callback->intel_de_rmw( GEN11_CHICKEN_DCPR_2, 0,
			 DCPR_CLEAR_MEMSTAT_DIS | DCPR_SEND_RESP_IMM |
			 DCPR_MASK_LPMODE | DCPR_MASK_MAXLATENCY_MEMUP_CLR);
	
	
	NBlue::callback->intel_de_rmw( DC_STATE_DEBUG, 0,
			 DC_STATE_DEBUG_MASK_CORES | DC_STATE_DEBUG_MASK_MEMORY_UP);
	NBlue::callback->readReg32(DC_STATE_DEBUG);
	
	if (DISPLAY_VER(&NBlue::callback->display_base) == 12 && NBlue::callback->display_base.info.__device_info->has_psr)
	{
		NBlue::callback->intel_de_rmw( _PIPEDMC_CONTROL_A, PIPEDMC_ENABLE, 0);
	}
	else
		NBlue::callback->intel_de_rmw( _PIPEDMC_CONTROL_A, 0, PIPEDMC_ENABLE);

	//hwConfigureCustomAUX(that, true);
	
	
}
