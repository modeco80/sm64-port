// Mario64 PC port DLL API.
// NOTE: The C support in this API header file is ONLY intended
// for the API client side (i.e, the Mario 64 PC port). 
// All mods should prefer to take advantage of the C++ stuff given here.

#ifndef _MARIO64MOD_H
#define _MARIO64MOD_H

// Mod api expects the ability to import SM64 types..
#include "types.h"

#include <type_traits>
#include <cstdint>

// this is the DLL version this API header is targeting
// changing this can, will, and always has the ability to break stuff
// only do so if there's a huge breaking change
constexpr static char CurrentAPIVersion[8] = "64DLL01";


enum class ExitReason : uint8_t {
	NotApplicable,
	
	// The mod was disabled explicitly during play.
	ModDisable,
	
	// The game play session is over.
	GameExiting
};


constexpr auto max_name_length = 64;


// These API functions are passed to the DLL by the pc port.
struct APIFunctions {
	// the given API version.
	// This field is always first,
	// and will never change its size (unless required)
	const char APIVersion[8];
	
	// allocate game memory chunk
	void*(*malloc)(uint64_t);
	
	// free a chunk of game memory
	void(*free)(void*);
	
	// Log a message to the screen
	void(*Log)(const char*);
	
	
	// TODO: API to do things with mario and stuff....
	
	// TODO: could there be an API for generating display lists on the DLL side?
	// Kinda like GoldSrc's TRIAPI but for f3dex.
	// This would make DLL modding kind of cool as we could create and draw our own objects.
};

// mod meta-info structure
struct ModInfo {
	char Name[max_name_length]; // "My Mario 64 Mod"
	char Author[max_name_length]; // "Developer Here"
	char Version[16]; // able to hold "v0000.0000.0000"
};

// TODO: Import m64PC structures

struct MarioMod {
	// TODO these will get a bunch of fun
	void (*OnInit)(MarioMod*);
	void (*OnFrame)(MarioMod*);
	void (*OnExit)(MarioMod*, ExitReason);
	
	ModInfo* (*GetModInfo)();
	
	// For internal use. Don't touch.
	void* module;
	
	// Mario state
	MarioState* mario;
	
	// all scene objects
	ObjectNode* objects;
	
	// A 4KB section of scratchpad memory. This memory should be preferred before the Game API heap, 
	// and is designed for SMALL and fast-to-lookup data to be placed in it.
	uint8_t scratchpad[4096];
	
	// TODO: Other variables.
	
	// C++ API helpers should be excluded from 
	// C "API" build
#ifndef MARIO64_EXE

	// Helper to write a value into scratchpad memory.
	template<class T, typename = std::is_standard_layout_v<T>>
	void WriteScratchpad(size_t addr, T value) {
		memcpy(&scratchpad[addr], &value, sizeof(T));
	}
	
	// Helper to read a value from the scratchpad memory.
	template<class T, typename = std::is_standard_layout_v<T>>
	T ReadScratchpad(size_t addr) {
		T temp;
		memcpy(&temp, &scratchpad[addr], sizeof(T));
		return temp;
	}
	
#endif
};


// More C++ api... Boring!!!
#ifndef MARIO64_EXE

// Template structure for handling 
// scratchpad variables in a *very* type safe way.
// Hopefully with optimizations, this structure
// will be boiled out..!
template<typename T, size_t addr = 0>
struct ScratchpadVariable {
	
	constexpr T Get(MarioMod* pMod) {
		return pMod->ReadScratchpad<T>(addr);
	}
	
	constexpr void Set(MarioMod* pMod, T value) {
		pMod->WriteScratchpad<T>(addr);
	}
	
};

#endif

#ifndef MARIO64_EXE	
	#define DLLEXP __declspec(dllexport)
#endif

#endif