
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


bool kexticl=false;
bool kexttgld=false;
bool kexttglp=false;

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
			{"_gPlatformInformationList", this->gPlatformInformationList2},
			
		};
		PANIC_COND(!SolveRequestPlus::solveAll(patcher, index, solveRequests, address, size), "nblue",	"Failed to resolve symbols");
		
		RouteRequestPlus requests[] = {
			{"__ZN31AppleIntelFramebufferController16hwRegsNeedUpdateEP21AppleIntelFramebufferP21AppleIntelDisplayPathP10CRTCParamsPK29IODetailedTimingInformationV2PN16AppleIntelScaler12SCALERPARAMSE",dofalse},
			{"__ZN31AppleIntelFramebufferController23initPlatformWorkaroundsEv",initPlatformWorkarounds2, this->oinitPlatformWorkarounds2},
			{"__ZN31AppleIntelFramebufferController16getOSInformationEv",getOSInformation2, this->ogetOSInformation2},
			{"__ZN31AppleIntelFramebufferController19getTranscoderOffsetEP14AppleIntelPortj",dozero},// reg fix
			
			{"__ZN31AppleIntelFramebufferController14ReadRegister32Em",raReadRegister32, this->oraReadRegister32},
			{"__ZN31AppleIntelFramebufferController15WriteRegister32Emj",raWriteRegister32, this->oraWriteRegister32},
			
			{"__ZN21AppleIntelFramebuffer25setAttributeForConnectionEijm",wrapSetAttributeForConnection, this->owrapSetAttributeForConnection},
			{"__ZN21AppleIntelFramebuffer25getAttributeForConnectionEijPm",getAttributeForConnection, this->ogetAttributeForConnection},
			{"__ZN31AppleIntelFramebufferController13FBMemMgr_InitEv", FBMemMgr_Init,this->oFBMemMgr_Init},
			{"__ZN31AppleIntelFramebufferController9hwGetCRTCEP21AppleIntelFramebufferP21AppleIntelDisplayPath",hwGetCRTC, this->ohwGetCRTC},
			{"__ZN31AppleIntelFramebufferController21hwSetPanelPowerConfigEj", hwSetPanelPowerConfig,this->ohwSetPanelPowerConfig},
			
		};
		PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route symbols");
		
		//platform id
		static const uint8_t f15[]= {0x00,0x00, 0x00, 0x5d, 0x8a};
		static const uint8_t r15[]= {0x00,0x00, 0x00, 0x49, 0x9a};

		LookupPatchPlus const patches[] = {
			{&kextG11FB, f15, r15, arrsize(f15),    1},
		};
		//PANIC_COND(!LookupPatchPlus::applyAll(patcher, patches , address, size), "nblue", "kextG11FB Failed to apply patches!");

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
			};
			PANIC_COND(!SolveRequestPlus::solveAll(patcher, index, solveRequests, address, size), "nblue",	"Failed to resolve symbols");
		}
		else
		{
			SolveRequestPlus solveRequests[] = {
				{"_gPlatformInformationList", this->gPlatformInformationList},
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
			
			
		};
		PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route dp symbols");
		
		if (isprod) {
			RouteRequestPlus requests[] = {
				{"__ZN31AppleIntelFramebufferController9hwGetCRTCEP21AppleIntelFramebufferP21AppleIntelDisplayPath",hwGetCRTC, this->ohwGetCRTC},
				{"__ZN31AppleIntelFramebufferController21hwSetPanelPowerConfigEj", hwSetPanelPowerConfig,this->ohwSetPanelPowerConfig},
				{"__ZN31AppleIntelFramebufferController15enableVDDForAuxEP14AppleIntelPort",enableVDDForAux, this->oenableVDDForAux},
				{"__ZN21AppleIntelFramebuffer4initEP31AppleIntelFramebufferControllerj",AppleIntelFramebufferinit, this->oAppleIntelFramebufferinit},
				{"__ZN31AppleIntelFramebufferController13FBMemMgr_InitEv", FBMemMgr_Init,this->oFBMemMgr_Init},
				{"__ZN31AppleIntelFramebufferController23initPlatformWorkaroundsEv",initPlatformWorkarounds, this->oinitPlatformWorkarounds},
				{"__ZN31AppleIntelFramebufferController16getOSInformationEv",getOSInformation, this->ogetOSInformation},

			};
			PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route p symbols");
			
		} else //debug version
		{
			RouteRequestPlus requests[] = {
				{"__ZN24AppleIntelBaseController9hwGetCRTCEP21AppleIntelFramebufferP21AppleIntelDisplayPath",hwGetCRTC, this->ohwGetCRTC},
				{"__ZN24AppleIntelBaseController21hwSetPanelPowerConfigEj", hwSetPanelPowerConfig,this->ohwSetPanelPowerConfig},
				{"__ZN24AppleIntelBaseController15enableVDDForAuxEP14AppleIntelPort",enableVDDForAux, this->oenableVDDForAux},
				{"__ZN21AppleIntelFramebuffer4initEP24AppleIntelBaseControllerj",AppleIntelFramebufferinit, this->oAppleIntelFramebufferinit},
				{"__ZN24AppleIntelBaseController13FBMemMgr_InitEv", FBMemMgr_Init,this->oFBMemMgr_Init},
				{"__ZN24AppleIntelBaseController23initPlatformWorkaroundsEv",initPlatformWorkarounds, this->oinitPlatformWorkarounds},
				{"__ZN24AppleIntelBaseController16getOSInformationEv",getOSInformation, this->ogetOSInformation},
				
			};
			PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route d symbols");
			
		}
		
	

		//force edp panel if pipe=1
		static const uint8_t f6a[]= {0x74, 0x2a, 0x83, 0xf8, 0x01, 0x74, 0x43, 0x85, 0xc0, 0x75, 0x60};
		static const uint8_t r6a[]= {0x90, 0x90, 0x83, 0xf8, 0x01, 0x90, 0x90, 0x85, 0xc0, 0x90, 0x90};

		static const uint8_t f6ap[]= {0x74, 0x2a, 0x83, 0xf8, 0x01, 0x74, 0x43, 0x85, 0xc0, 0x75, 0x60};
		static const uint8_t r6ap[]= {0x90, 0x90, 0x83, 0xf8, 0x01, 0x90, 0x90, 0x85, 0xc0, 0x90, 0x90};
		
		//ReadRegister64
		static const uint8_t f7[]= {0x83, 0xc0, 0xfc, 0x48, 0x39, 0xf0, 0x76, 0x11, 0x48, 0x8b, 0x47, 0x50, 0x48, 0xff, 0x05, 0xca, 0xf5, 0x0c, 0x00};
		static const uint8_t r7[]= {0x83, 0xc0, 0xf8, 0x48, 0x39, 0xf0, 0x76, 0x11, 0x48, 0x8b, 0x47, 0x50, 0x48, 0xff, 0x05, 0xca, 0xf5, 0x0c, 0x00};
		
		static const uint8_t f7p[]= {0x83, 0xc0, 0xfc, 0x48, 0x39, 0xf0, 0x76, 0x11, 0x48, 0x8b, 0x47, 0x50, 0x48, 0xff, 0x05, 0x84, 0x40, 0x08, 0x00};
		static const uint8_t r7p[]= {0x83, 0xc0, 0xf8, 0x48, 0x39, 0xf0, 0x76, 0x11, 0x48, 0x8b, 0x47, 0x50, 0x48, 0xff, 0x05, 0x84, 0x40, 0x08, 0x00};

		
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
		

		//getHPDState register
		static const uint8_t f19[]= {0xbe, 0x70, 0x44, 0x04, 0x00};
		static const uint8_t r19[]= {0xbe, 0xa0, 0x38, 0x16, 0x00};
		
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
		
		//linktrainig bad hack
		static const uint8_t f25[]= {0x77, 0x77, 0x00, 0x00};
		static const uint8_t r25[]= {0x33, 0x00, 0x00, 0x00};



		
		if (isprod){
			LookupPatchPlus const patchesp[] = {// tgl production kext
				
				{&kextG11FBT, f6ap, r6ap, arrsize(f6ap),	1},
				{&kextG11FBT, f7p, r7p, arrsize(f7p),	1},
				{&kextG11FBT, f13p, r13p, arrsize(f13p),	1},
				{&kextG11FBT, f13pb, r13pb, arrsize(f13pb),	1},
				{&kextG11FBT, f19, r19, arrsize(f19),	1},
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

				{&kextG11FBT, f6a, r6a, arrsize(f6a),	1},
				{&kextG11FBT, f7, r7, arrsize(f7),	1},
				{&kextG11FBT, f13, r13, arrsize(f13),	1},
				{&kextG11FBT, f13b, r13b, arrsize(f13b),	1},
				{&kextG11FBT, f15, r15, arrsize(f15),	1},
				{&kextG11FBT, f19, r19, arrsize(f19),	1},
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
			 
			 
		 };
		PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route symbols");
		
		//sku
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
		 };
		PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route symbols");
		
		//sku bypass IntelAccelerator::getGPUInfo
		static const uint8_t f3[] = {0x8b, 0x3e, 0x81, 0xff, 0xee, 0xbe, 0xaf, 0xde, 0x7f, 0x15, 0x81, 0xff, 0x86, 0x80, 0x40, 0x9a, 0x74, 0x2d};
		static const uint8_t r3[] = {0x8b, 0x3e, 0x81, 0xff, 0xee, 0xbe, 0xaf, 0xde, 0x90, 0x90, 0x81, 0xff, 0x86, 0x80, 0x40, 0x9a, 0xeb, 0x2d};
		
		// 12 to 10 subslices + L3BankCount = 8
		static const uint8_t f3a[] = {0x83, 0xfe, 0x01, 0x75, 0x59, 0x83, 0xfa, 0x0c};
		static const uint8_t r3a[] = {0x83, 0xfe, 0x01, 0x75, 0x59, 0x83, 0xfa, 0x0a};
		
		
		//blit3d mem alloc patch
		static const uint8_t f5[] = {0x40, 0xd2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		static const uint8_t r5[] = {0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		
		
			LookupPatchPlus const patches[] = {
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


//globals
void *ccont;
void *ccont2;
int bk=2;

void  Gen11::initPlatformWorkarounds(void *that)
{
	//PlatformWorkarounds
	getMember<volatile uint32_t>(that, 0xc5c)=
	FB_FLAG_ALTERNATE_PWM_INCREMENT1|/*FB_FLAG_ENABLE_SLICE_FEATURES|*/
	FB_FLAG_FORCE_POWER_ALWAYS_CONNECTED|FB_FLAG_AVOID_FAST_LINK_TRAINING/*| FB_FLAG_ENABLE_BACKLIGHT_REG_CONTROL|FB_FLAG_LIMIT_4K_SOURCE_SIZE*/;
	
	//ig boot flags
	getMember<volatile uint32_t>(that, 0xc58)=/*FB_FLAG_ENABLE_BACKLIGHT_REG_CONTROL|*/FB_FLAG_BOOST_PIXEL_FREQUENCY_LIMIT;
	
}
uint64_t  Gen11::getOSInformation(void *that)
{
	
	if (NBlue::callback->intel_opregion_setup()!=0) panic("BAD BIOS");
	
	//if (intel_display_wa(display, INTEL_DISPLAY_WA_14010480278))
	//	NBlue::callback->intel_de_rmw( 0x46530, 0, REG_BIT(27));
	
	
	//fPCIConfigRevisionID
	getMember<int32_t>(that, 0xce4)=1; //PHYA
	struct PlatformInfo *pinfo =static_cast<PlatformInfo *>(callback->gPlatformInformationList);
	
	pinfo[1].fInfoFlags=
	FB_FLAG_DISABLE_PIPE_SCRAMBLE|FB_FLAG_FRAMEBUFFER_COMPRESSION|FB_FLAG_ALLOW_CONNECTOR_RECOVER
	/*|FB_FLAG_ENABLE_BACKLIGHT_REG_CONTROL*/|FB_FLAG_FORCE_POWER_ALWAYS_CONNECTED|FB_FLAG_AVOID_FAST_LINK_TRAINING
	/*|FB_FLAG_USE_VIDEO_TURBO|FB_FLAG_ALTERNATE_PWM_INCREMENT2*/;
	
	
		pinfo[1].cameliav=0;
		//CamelliaTcon2=2 BanksiaTcon=3
	
		pinfo[1].fMobile=1;
		pinfo[1].fPipeCount=3;
		pinfo[1].fInfoPortCount=3;
		pinfo[1].fInfoFramebufferCount=2;

		pinfo[1].fSliceCount=1;
		pinfo[1].fmaxEuCount=8;
		pinfo[1].fsubslices=8;
	
	//pinfo[1].fInfoFBCompressionMemorySize=	0xc00000;
	//pinfo[1].fVideoTurboFreq=270000000;
	//pinfo[1].VCLK=1000*0x438;
	
	int p=1;
	for (int i = 0; i < 6; i++) {
		pinfo[p].connectors[i].index=NBlue::callback->display_ctx.bconnectors[i].index;
		pinfo[p].connectors[i].busId=NBlue::callback->display_ctx.bconnectors[i].busId;
		pinfo[p].connectors[i].pipe=NBlue::callback->display_ctx.bconnectors[i].pipe;
		pinfo[p].connectors[i].pad=NBlue::callback->display_ctx.bconnectors[i].pad;
		pinfo[p].connectors[i].type=NBlue::callback->display_ctx.bconnectors[i].type;
		pinfo[p].connectors[i].flags=NBlue::callback->display_ctx.bconnectors[i].flags;
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
	
	/*pinfo[1].connectors[0].index=0;//DDI0
	pinfo[1].connectors[0].busId=0;
	pinfo[1].connectors[0].pipe=0;//1 dp power 0 edp power
	pinfo[1].connectors[0].pad=0;
	pinfo[1].connectors[0].type=ConnectorLVDS;
	pinfo[1].connectors[0].flags=0x1+0x8+0x10;
	
	pinfo[1].connectors[1].index=1;//DDI1
	pinfo[1].connectors[1].busId=0;
	pinfo[1].connectors[1].pipe=1;
	pinfo[1].connectors[1].pad=0;
	pinfo[1].connectors[1].type=ConnectorDummy;
	pinfo[1].connectors[1].flags=0;
	
*/
	
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
	getMember<void *>(frame, 0x4a40) = ccont;
	getMember<void *>(frame, 0xc40) = ccont;
	auto ret=FunctionCast(AppleIntelFramebufferinit, callback->oAppleIntelFramebufferinit)(frame,cont,param_2 );
	
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

void  Gen11::initPlatformWorkarounds2(void *that)
{
	//PlatformWorkarounds
	getMember<volatile uint32_t>(that, 0xc1c)=
 /*FB_FLAG_ENABLE_SLICE_FEATURES|FB_FLAG_ENABLE_BACKLIGHT_REG_CONTROL|FB_FLAG_LIMIT_4K_SOURCE_SIZE|*/
	/*FB_FLAG_DISABLE_FEATURE_IPS|FB_FLAG_ALTERNATE_PWM_INCREMENT2|*/
	FB_FLAG_ALTERNATE_PWM_INCREMENT1|/*FB_FLAG_DISABLE_HIGH_BITRATE_MODE2|*/
	FB_FLAG_FORCE_POWER_ALWAYS_CONNECTED|FB_FLAG_AVOID_FAST_LINK_TRAINING;
	
	//ig boot flags
	getMember<volatile uint32_t>(that, 0xc10)=/*FB_FLAG_DISABLE_HIGH_BITRATE_MODE2|*/FB_FLAG_BOOST_PIXEL_FREQUENCY_LIMIT;
	
}
uint64_t  Gen11::getOSInformation2(void *that)
{
	if (NBlue::callback->intel_opregion_setup()!=0) panic("BAD BIOS");
	
	//if (intel_display_wa(display, INTEL_DISPLAY_WA_14010480278))
	//	NBlue::callback->intel_de_rmw( 0x46530, 0, REG_BIT(27));
	
	//fPCIConfigRevisionID
	getMember<int32_t>(that, 0xc9c)=1;
	struct FramebufferICLLP *pinfo =static_cast<FramebufferICLLP *>(callback->gPlatformInformationList2);
	int p=0x5;
	
	pinfo[p].flags=
	FB_FLAG_DISABLE_PIPE_SCRAMBLE|FB_FLAG_ALLOW_CONNECTOR_RECOVER|FB_FLAG_ENABLE_DITHERING|
	/*FB_FLAG_LIMIT_4K_SOURCE_SIZE|*/FB_FLAG_BOOST_PIXEL_FREQUENCY_LIMIT|
	FB_FLAG_FRAMEBUFFER_COMPRESSION;
	
	
		pinfo[p].camelliaVersion=0;
		//CamelliaTcon2=2 BanksiaTcon=3
	
		pinfo[p].fMobile=1;
		pinfo[p].fPipeCount=3;
		pinfo[p].fPortCount=3;
		pinfo[p].fInfoFramebufferCount=2;

		pinfo[p].fSliceCount=1;
		pinfo[p].fEuCount=8;
		pinfo[p].fSubSliceCount=8;
	
	//pinfo[p].fInfoFBCompressionMemorySize=	0xc00000;
	//pinfo[p].fVideoTurboFreq=270000000;
	//pinfo[p].VCLK=1000*0x438;
	
	
	for (int i = 0; i < 6; i++) {
		pinfo[p].connectors[i].index=NBlue::callback->display_ctx.bconnectors[i].index;
		pinfo[p].connectors[i].busId=NBlue::callback->display_ctx.bconnectors[i].busId;
		pinfo[p].connectors[i].pipe=NBlue::callback->display_ctx.bconnectors[i].pipe;
		pinfo[p].connectors[i].pad=NBlue::callback->display_ctx.bconnectors[i].pad;
		pinfo[p].connectors[i].type=NBlue::callback->display_ctx.bconnectors[i].type;
		pinfo[p].connectors[i].flags=NBlue::callback->display_ctx.bconnectors[i].flags;
	}
	
	
	pinfo[p].connectors[0].pipe=1;
	pinfo[p].connectors[0].flags=0x1+0x10; //force display to frame zero
	
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
	
	/*pinfo[p].connectors[0].index=0;
	pinfo[p].connectors[0].busId=0;
	pinfo[p].connectors[0].pipe=1;
	pinfo[p].connectors[0].pad=0;
	pinfo[p].connectors[0].type=ConnectorLVDS;
	pinfo[p].connectors[0].flags=0x1+0x10; //force display to frame zero
	
	pinfo[p].connectors[1].index=1;
	pinfo[p].connectors[1].busId=0;
	pinfo[p].connectors[1].pipe=0;
	pinfo[p].connectors[1].pad=0;
	pinfo[p].connectors[1].type=ConnectorDummy;
	pinfo[p].connectors[1].flags=0;
	
*/
	
	auto ret=FunctionCast(getOSInformation2, callback->ogetOSInformation2)(that );
	return ret;
}



void Gen11::hwGetCRTC(void *that,void *param_1,void *param_2)
{
	FunctionCast(hwGetCRTC, callback->ohwGetCRTC)(that,param_1,param_2 );
	
	if (bk==1){
		
		uint32_t lf=NBlue::callback->display_ctx.panel.backlight.level*0xffff;
		lf=lf/NBlue::callback->display_ctx.panel.backlight.pwm_level_max;
		
		
		//bklfrequency
		getMember<uint32_t>(that, kexticl ? 0xe54 : kexttgld ? 0xe6c : 0x60)=NBlue::callback->display_ctx.panel.backlight.pwm_level_max;
		
		//bkllvl
		if (getMember<uint32_t>(that, kexticl ? 0xe4c : kexttgld ? 0xe64 : 0xe58)<NBlue::callback->display_ctx.panel.backlight.level)
		getMember<uint32_t>(that, kexticl ? 0xe4c : kexttgld ? 0xe64 : 0xe58)=NBlue::callback->display_ctx.panel.backlight.level;
		
		if (getMember<uint32_t>(that, kexticl ? 0xe50 : kexttgld ? 0xe68 : 0xe5c)>NBlue::callback->display_ctx.panel.backlight.level)
			getMember<uint32_t>(that, kexticl ? 0xe4c : kexttgld ? 0xe64 : 0xe58)=getMember<uint32_t>(that, kexticl ? 0xe50 : kexttgld ? 0xe68 : 0xe5c);
		
		//bkllvl_saved
		if (getMember<uint32_t>(that, kexticl ? 0xe50 : kexttgld ? 0xe68 : 0xe5c)<NBlue::callback->display_ctx.panel.backlight.level)
		getMember<uint32_t>(that, kexticl ? 0xe50 : kexttgld ? 0xe68 : 0xe5c)=NBlue::callback->display_ctx.panel.backlight.level;
		
		
		u32 pwm_ctl;

		pwm_ctl = NBlue::callback->readReg32( BXT_BLC_PWM_CTL(NBlue::callback->display_ctx.panel.backlight.controller));
		if (pwm_ctl & BXT_BLC_PWM_ENABLE) {
			pwm_ctl &= ~BXT_BLC_PWM_ENABLE;
			NBlue::callback->writeReg32( BXT_BLC_PWM_CTL(NBlue::callback->display_ctx.panel.backlight.controller),
					   pwm_ctl);
		}

		NBlue::callback->writeReg32( BXT_BLC_PWM_FREQ(NBlue::callback->display_ctx.panel.backlight.controller),
									NBlue::callback->display_ctx.panel.backlight.pwm_level_max);

		NBlue::callback->writeReg32( BXT_BLC_PWM_DUTY(NBlue::callback->display_ctx.panel.backlight.controller), NBlue::callback->display_ctx.panel.backlight.level);

		pwm_ctl = 0;
		if (NBlue::callback->display_ctx.panel.backlight.active_low_pwm)
			pwm_ctl |= BXT_BLC_PWM_POLARITY;

		NBlue::callback->writeReg32( BXT_BLC_PWM_CTL(NBlue::callback->display_ctx.panel.backlight.controller), pwm_ctl);
		NBlue::callback->readReg32( BXT_BLC_PWM_CTL(NBlue::callback->display_ctx.panel.backlight.controller));
		NBlue::callback->writeReg32( BXT_BLC_PWM_CTL(NBlue::callback->display_ctx.panel.backlight.controller),
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
	
	struct intel_pps_delays p = NBlue::callback->display_ctx.panel.pps.pps_delays;


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
	
	struct pps_registers regs=NBlue::callback->display_ctx.panel.regs;
	
	
	uint32_t PCH_PP_ON_DELAYS = REG_FIELD_PREP(PANEL_POWER_UP_DELAY_MASK, p.power_up) |
		REG_FIELD_PREP(PANEL_LIGHT_ON_DELAY_MASK, p.backlight_on);
	uint32_t PCH_PP_OFF_DELAYS = REG_FIELD_PREP(PANEL_LIGHT_OFF_DELAY_MASK, p.backlight_off) |
		REG_FIELD_PREP(PANEL_POWER_DOWN_DELAY_MASK, p.power_down);

	NBlue::callback->writeReg32(regs.pp_on, PCH_PP_ON_DELAYS);
	NBlue::callback->writeReg32(regs.pp_off, PCH_PP_OFF_DELAYS);

	int div = NBlue::callback->display_ctx.panel.rawclk_freq / 1000;
	
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
	
	unsigned long v=NBlue::callback->display_ctx.panel.backlight.level;
	if (getMember<uint32_t>(ccont2, kexticl ? 0xe4c: kexttgld ? 0xe64 : 0xe58)<v)  getMember<uint32_t>(ccont2, kexticl ? 0xe4c: kexttgld ? 0xe64 : 0xe58)=v;
	
	*value=getMember<uint32_t>(ccont2, kexticl ? 0xe4c: kexttgld ? 0xe64 : 0xe58);
	value[1] = 0;
	value[2] = 0xffff;
	
	return kIOReturnSuccess;
};

IOReturn Gen11::wrapSetAttributeForConnection(void* framebuffer, int32_t connectIndex, uint32_t attribute,
											  unsigned long value)
{
	const auto ret = FunctionCast(wrapSetAttributeForConnection, callback->owrapSetAttributeForConnection)(
																										   framebuffer, connectIndex, attribute, value);
	if (attribute != 'bklt') { return ret; }
	
	
	if (value<NBlue::callback->display_ctx.panel.backlight.level)  value=NBlue::callback->display_ctx.panel.backlight.level;
	
	NBlue::callback->writeReg32( BXT_BLC_PWM_DUTY(NBlue::callback->display_ctx.panel.backlight.controller), value);
	getMember<uint32_t>(ccont2, kexticl ? 0xe4c: kexttgld ? 0xe64 : 0xe58)=value;
	return kIOReturnSuccess;
};

