
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
		
		
		RouteRequestPlus requests[] = {

			{"__ZN21AppleIntelFramebuffer13SaveNVRAMModeEv",dofalse},
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
			{"__ZN19AppleIntelPowerWell21hwSetPowerWellStatePGEbj",hwSetPowerWellStatePG, this->ohwSetPowerWellStatePG},
			{"__ZN14AppleIntelPort8isHPDLowEv",isHPDLow, this->oisHPDLow},
			
			
			
		};
		PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route dp symbols");
		
		if (isprod) {
			RouteRequestPlus requests[] = {
				{"__ZN31AppleIntelFramebufferController13hwReadMailboxEj",hwReadMailbox, this->ohwReadMailbox},
				{"__ZN31AppleIntelFramebufferController14hwWriteMailboxEjjb",hwWriteMailbox, this->ohwWriteMailbox},
				{"__ZN31AppleIntelFramebufferController38mapIOBitsPerColorToEncoderBitsPerColorEjPj",mapIOBitsPerColorToEncoderBitsPerColor, this->omapIOBitsPerColorToEncoderBitsPerColor},
				{"__ZN31AppleIntelFramebufferController18hasExternalDisplayEv",hasExternalDispla},// external wrong detection
				{"__ZN31AppleIntelFramebufferController16hwRegsNeedUpdateEP21AppleIntelFramebufferP21AppleIntelDisplayPathP10CRTCParamsPK29IODetailedTimingInformationV2PN16AppleIntelScaler12SCALERPARAMSE",hwRegsNeedUpdate, this->ohwRegsNeedUpdate},
				{"__ZN21AppleIntelFramebuffer4initEP31AppleIntelFramebufferControllerj",AppleIntelFramebufferinit, this->oAppleIntelFramebufferinit},
				{"__ZN31AppleIntelFramebufferController18hwInitializeCStateEv",hwInitializeCState, this->ohwInitializeCState},
				{"__ZN31AppleIntelFramebufferController20hwConfigureCustomAUXEb",hwConfigureCustomAUX, this->ohwConfigureCustomAUX},
				{"__ZN31AppleIntelFramebufferController13FBMemMgr_InitEv", FBMemMgr_Init,this->oFBMemMgr_Init},
				{"__ZN31AppleIntelFramebufferController23initPlatformWorkaroundsEv",initPlatformWorkarounds, this->oinitPlatformWorkarounds},
				{"__ZN31AppleIntelFramebufferController16getOSInformationEv",getOSInformation, this->ogetOSInformation},
				{"__ZN19AppleIntelPowerWell4initEP31AppleIntelFramebufferController",AppleIntelPowerWellinit, this->oAppleIntelPowerWellinit},
				{"__ZN31AppleIntelFramebufferController16disableVDDForAuxEv",disableVDDForAux, this->odisableVDDForAux},
				{"__ZN31AppleIntelFramebufferController15enableVDDForAuxEP14AppleIntelPort",enableVDDForAux, this->oenableVDDForAux},
				
			};
			PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route p symbols");
			
		} else //debug version
		{
			RouteRequestPlus requests[] = {
				{"__ZN24AppleIntelBaseController13hwReadMailboxEj",hwReadMailbox, this->ohwReadMailbox},
				{"__ZN24AppleIntelBaseController14hwWriteMailboxEjjb",hwWriteMailbox, this->ohwWriteMailbox},
				{"__ZN24AppleIntelBaseController38mapIOBitsPerColorToEncoderBitsPerColorEjPj",mapIOBitsPerColorToEncoderBitsPerColor, this->omapIOBitsPerColorToEncoderBitsPerColor},
				{"__ZN24AppleIntelBaseController18hasExternalDisplayEv",hasExternalDispla},// external wrong detection
				{"__ZN24AppleIntelBaseController16hwRegsNeedUpdateEP21AppleIntelFramebufferP21AppleIntelDisplayPathP10CRTCParamsPK29IODetailedTimingInformationV2PN16AppleIntelScaler12SCALERPARAMSE",hwRegsNeedUpdate, this->ohwRegsNeedUpdate},
				
				{"__ZN21AppleIntelFramebuffer4initEP24AppleIntelBaseControllerj",AppleIntelFramebufferinit, this->oAppleIntelFramebufferinit},
				{"__ZN24AppleIntelBaseController18hwInitializeCStateEv",hwInitializeCState, this->ohwInitializeCState},
				{"__ZN24AppleIntelBaseController20hwConfigureCustomAUXEb",hwConfigureCustomAUX, this->ohwConfigureCustomAUX},
				{"__ZN24AppleIntelBaseController13FBMemMgr_InitEv", FBMemMgr_Init,this->oFBMemMgr_Init},
				{"__ZN24AppleIntelBaseController23initPlatformWorkaroundsEv",initPlatformWorkarounds, this->oinitPlatformWorkarounds},
				{"__ZN24AppleIntelBaseController16getOSInformationEv",getOSInformation, this->ogetOSInformation},
				{"__ZN19AppleIntelPowerWell4initEP24AppleIntelBaseController",AppleIntelPowerWellinit, this->oAppleIntelPowerWellinit},
				{"__ZN24AppleIntelBaseController16disableVDDForAuxEv",disableVDDForAux, this->odisableVDDForAux},
				{"__ZN24AppleIntelBaseController15enableVDDForAuxEP14AppleIntelPort",enableVDDForAux, this->oenableVDDForAux},
				
			};
			PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route d symbols");
			
		}
		
		
	
		
		//CamelliaTcon2 ID replace
		static const uint8_t f1b[] = {0x11, 0x0a, 0x84, 0x41};
		static const uint8_t r1b[] = {0x11, 0x0a, 0x82, 0x41};
		
		//BanksiaTcon3
		static const uint8_t f1c[] = {0x12, 0x14, 0xc4, 0x41};
		static const uint8_t r1c[] = {0x11, 0x0a, 0x82, 0x41};
		
		static const uint8_t f1p[]= {0xe8, 0x66, 0xb0, 0xfe, 0xff, 0x89, 0x45, 0xc8, 0x3d, 0xff, 0xff, 0x00, 0x00, 0x74, 0x45};
		static const uint8_t r1p[]= {0xe8, 0x66, 0xb0, 0xfe, 0xff, 0x89, 0x45, 0xc8, 0x3d, 0xff, 0xff, 0x00, 0x00, 0xeb, 0x45};
		

		//force edp panel
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

		
		//fix register adresses if pipe=0
		static const uint8_t f24bp[]= {0x83, 0x78, 0x08, 0x00, 0x75, 0x0c};
		static const uint8_t r24bp[]= {0x83, 0x78, 0x08, 0x00, 0xeb, 0x0c};
		
		static const uint8_t f24cp[]= {0x00, 0x4c, 0x89, 0xea, 0x75, 0x12};
		static const uint8_t r24cp[]= {0x00, 0x4c, 0x89, 0xea, 0xeb, 0x12};
		
		static const uint8_t f24dp[]= {0x83, 0x78, 0x08, 0x00, 0x75, 0x0d};
		static const uint8_t r24dp[]= {0x83, 0x78, 0x08, 0x00, 0xeb, 0x0d};
		
		//fix register adresses if pipe=0
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
		 };
		SYSLOG_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route symbols");
		
		//sku bypass IntelAccelerator::getGPUInfo
		static const uint8_t f2[] = {0x0F, 0x87, 0x17, 0x01, 0x00, 0x00, 0x48, 0x8D, 0x0D, 0x96, 0x02, 0x00, 0x00};
		static const uint8_t r2[] = {0xe9, 0x8b, 0x00, 0x00, 0x00, 0x90, 0x48, 0x8D, 0x0D, 0x96, 0x02, 0x00, 0x00};
    
		LookupPatchPlus const patches[] = {
			{&kextG11HW, f2, r2, arrsize(f2),	1},
		};
		

		SYSLOG_COND(!LookupPatchPlus::applyAll(patcher, patches , address, size), "nblue", "kextG11HW Failed to apply patches!");

		return true;
		
		
    } else if (kextG11HWT.loadIndex == index || kextG11HWTe.loadIndex == index) {
		NBlue::callback->setRMMIOIfNecessary();
		auto kext=kextG11HWT.loadIndex == index ? kextG11HWT: kextG11HWTe;
		
		SolveRequestPlus solveRequests[] = {
			//apple rings control variable
			{"__ZL11kIGHwCsDesc", this->kIGHwCsDesc},
			
		};
		//PANIC_COND(!SolveRequestPlus::solveAll(patcher, index, solveRequests, address, size), "nblue",	"Failed to resolve symbols");
		 
		
		 RouteRequestPlus requests[] = {
			 
			 {"__ZN16IntelAccelerator19PAVPCommandCallbackE22PAVPSessionCommandID_tjPjb", wrapPavpSessionCallback, this->orgPavpSessionCallback},

			 
		 };
		//PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route symbols");
		
		//sku bypass IntelAccelerator::getGPUInfo
		static const uint8_t f3[] = {0x8b, 0x3e, 0x81, 0xff, 0xee, 0xbe, 0xaf, 0xde, 0x7f, 0x15, 0x81, 0xff, 0x86, 0x80, 0x40, 0x9a, 0x74, 0x2d};
		static const uint8_t r3[] = {0x8b, 0x3e, 0x81, 0xff, 0xee, 0xbe, 0xaf, 0xde, 0x90, 0x90, 0x81, 0xff, 0x86, 0x80, 0x40, 0x9a, 0xeb, 0x2d};
		
		// 12 to 10 slices + L3BankCount = 8
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
		pinfo[1].fsubslices=10;
	
	//pinfo[1].fInfoFBCompressionMemorySize=	0xc00000;
	//pinfo[1].fVideoTurboFreq=270000000;
	//pinfo[1].VCLK=1000*0x438;
	
	
	pinfo[1].connectors[0].index=0;//DDI0
	pinfo[1].connectors[0].busId=0;
	pinfo[1].connectors[0].pipe=0;//fix wrong register use patch or set = 1
	pinfo[1].connectors[0].pad=0;
	pinfo[1].connectors[0].type=ConnectorLVDS;
	pinfo[1].connectors[0].flags=0x8;
	
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



IOReturn Gen11::wrapPavpSessionCallback( void *intelAccelerator, int32_t sessionCommand, uint32_t sessionAppId, uint32_t *a4, bool flag) {
	
	
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



unsigned long Gen11::hwRegsNeedUpdate
		  (void *that,void *param_1,
		   void *param_2,void *param_3,void *param_4,
		   void *param_5)
{
	
	auto ret=FunctionCast(hwRegsNeedUpdate, callback->ohwRegsNeedUpdate)(that,param_1,param_2,param_3,param_4,param_5 );

//return 1 linktraining + enable pipe
//return 1;

	return ret;
}


unsigned long  Gen11::isHPDLow(void *that)
{
	auto ret= FunctionCast(isHPDLow, callback->oisHPDLow)(that);
	return ret;
};

uint64_t Gen11::mapIOBitsPerColorToEncoderBitsPerColor(void *that,uint param_1,uint *param_2)
{
	//TODO: filter for edp only + check other options
	//[drm:intel_edp_fixup_vbt_bpp [i915]] pipe has 24 bpp for eDP panel, overriding BIOS-provided max 18 bpp
	if (param_2 != (uint *)0x0) {
	  param_1 = 2;
	}
	
	return FunctionCast(mapIOBitsPerColorToEncoderBitsPerColor, callback->omapIOBitsPerColorToEncoderBitsPerColor)(that,param_1,param_2);;
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
	ccont = getMember<void *>(that, 0xc40);

}


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


void Gen11::AppleIntelPowerWellinit(void *that,void *param_1)
{
	
	FunctionCast(AppleIntelPowerWellinit, callback->oAppleIntelPowerWellinit)(that,param_1);
	
	if (NBlue::callback->isRealTGL) {
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

unsigned long Gen11::AppleIntelScalerinit(void *that,uint8_t param_1)
{
	
	auto ret=  FunctionCast(AppleIntelScalerinit, callback->oAppleIntelScalerinit)(that,param_1 );
	getMember<void *>(that, 0x28) = ccont;
	getMember<void *>(that, 0x10) = ccont2;
	return ret;
}

uint32_t Gen11::hwReadMailbox(void *that,uint param_1)
{
	return FunctionCast(hwReadMailbox, callback->ohwReadMailbox)(that,param_1 );
}

void Gen11::hwWriteMailbox(void *that,uint param_1,uint param_2,bool param_3)
{
	FunctionCast(hwWriteMailbox, callback->ohwWriteMailbox)(that,param_1,param_2,param_3 );
}


void Gen11::hwSetPowerWellStatePG(void *that,bool param_1,uint param_2)
{
	if (!NBlue::callback->isRealTGL) {
		uint32_t ctl2 = NBlue::callback->readReg32(0x45404);
		uint32_t newVal;
		if (param_1) {
			// Enable: clear all REQ bits then set the requested ones.
			newVal = (ctl2 & 0xFFFFFF55U) | (param_2 & 0xAAU);
		} else {
			// Disable: clear the requested REQ bits.
			newVal = ctl2 & ~(param_2 & 0xAAU);
		}
		NBlue::callback->writeReg32(0x45404, newVal);
		return;
	}
	FunctionCast(hwSetPowerWellStatePG, callback->ohwSetPowerWellStatePG)(that,param_1,param_2);
}

void Gen11::hwInitializeCState(void *that)
{
	FunctionCast(hwInitializeCState, callback->ohwInitializeCState)(that);
}

void Gen11::hwConfigureCustomAUX(void *that,bool param_1)
{
	FunctionCast(hwConfigureCustomAUX, callback->ohwConfigureCustomAUX)(that,param_1 );
}


int Gen11::hasExternalDispla()
{
	return 0;
}
