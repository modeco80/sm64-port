#include "../src/pc/modapi.h"

// API functions.
static APIFunctions g_apiFunctions;

// You should put ScratchpadVariable<T, addr> instances here.
// That way, developers can easily work out what type a thing is, and where it is in scratchpad.
// Scratchpad can be used to also hold permenant object pointers if required.
constexpr static ScratchpadVariable<bool, 0> IsInitalized;

// Implementation of your mod!
static MarioMod MyMod {
	
	// OnInit(): Called when your mod is enabled.
	// Set up states here.
	[](MarioMod* mod) {
		// Write a value into the scratchpad indicating that we initalized
		IsInitalized.Set(mod, true);
	},
	
	// OnFrame(): Called on every frame.
	[](MarioMod* mod) {
	},
	
	// OnExit(): Called when your mod is disabled or the game is exited.
	// Clean up everything to a clean slate (or even zero your scratchpad)
	[](MarioMod* mod, ExitReason reason) {
		
		if(reason == ExitReason::ModDisable)
			g_apiFunctions.Log("MyMod OnExit(): mod disabled");
		
		IsInitalized.Set(mod, false);
	},
	
	// GetModInfo(): Called when the game needs to retrieve
	// information about your mod.
	// Ideally, this only needs to be done once.
	[]() {
		static ModInfo modinfo {
			"My Mario 64 Mod",
			"Developer Here",
			"0.1"
		};
		return &modinfo;
	}
};


// Lame boring bootstrap code that nobody likes
extern "C" {
	
	DLLEXPORT int GetMarioAPI(APIFunctions* apiFuncs, MarioMod* mod) {
		if(!strncmp(apiFuncs->APIVersion, CurrentAPIVersion, 8)) {
			
			// Copy the API functions that the game gave us
			// to our global API function table and the mario mod.
			memcpy(&g_apiFunctions, apiFuncs, sizeof(APIFunctions));
			memcpy(mod, &MyMod, sizeof(MarioMod));
			
			return 0;
		}
		
		// We are not compatible by any means.
		// Tell the game this.
		return 1;
	}

}