#include "types.h"
#include <vector>

#include <cstdio>

#define MARIO64_EXE
#include "modapi.h"

// include windows API but very slim to not break shit
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

std::vector<MarioMod> all_mods;

// engine stuff we want
extern MarioState gMarioStates[];
extern MarioState *gMarioState;
extern ObjectNode *gObjectLists;

static APIFunctions apiFunctions {
	"64DLL01", // TODO
	
	[](uint64_t size) {
		return malloc(size);
	},
	
	[](void* ptr) {
		if(ptr)
			free(ptr);
	},
	
	[](const char* str) {
		printf("Log(): %s\n", str);
	}
};


// TODO: Allow this to work on Linux


// template to avoid annoying casting
template<typename T>
inline T GetProcSafe(HMODULE module, const char* symname) {
	return (T)GetProcAddress(module, symname);
}

extern "C" {

	void mod_load_all() {
		printf("mod_load_all()\n");
		WIN32_FIND_DATA finddata;
		LARGE_INTEGER size;
		HANDLE hFindInfo;
		
		char temp_path[64];
		
		hFindInfo = FindFirstFile("mods\\*.dll", &finddata);
		
		if(hFindInfo == INVALID_HANDLE_VALUE) {
			printf("Directory \"mods\\\" doesn't exist, giving up.");
			return;
		}
		
		do {
			// Try to load this DLL.
			MarioMod mod;
			
			strcat(temp_path, "mods\\");
			strcat(temp_path, finddata.cFileName);
			
			HMODULE module = LoadLibrary(temp_path);
			
			if(!module)
				continue;
			
			int(*GetAPI)(APIFunctions*, MarioMod*) = GetProcSafe<int(*)(APIFunctions*, MarioMod*)>(module, "GetAPI");
			int result = 0;
			
			if(GetAPI == nullptr) {
				printf("Mod \"%s\"  isn't a Mario Mod.\n", temp_path);
				FreeLibrary(module);
				continue;
			}
			
			result = GetAPI(&apiFunctions, &mod);
			
			if(result == 1) {
				printf("Mod \"%s\" not compatiable with this EXE. Ask them to recompile it!\n", temp_path);
				// welp, that didn't work.
				FreeLibrary(module);
				continue;
			} else {
				// That worked.
				mod.module = (void*)module;
				all_mods.push_back(mod);
			}
			
			memset(temp_path, 0, 64*sizeof(char));
		} while(FindNextFile(hFindInfo, &finddata) != 0);
		
		// all done!
	}

	// initalize all enabled mods
	void mod_init_all() {
	
		for(auto& mod : all_mods) {
			mod.mario = &gMarioStates[0];
			mod.objects = &gObjectLists[0];
			mod.OnInit(&mod);
		}
		
	}

	void mod_frame_all() {
		for(auto& mod : all_mods)
			mod.OnFrame(&mod);
	}

	void mod_exit_all() {
		for(auto& mod : all_mods) {
			mod.OnExit(&mod, ExitReason::GameExiting);
			FreeLibrary((HMODULE)mod.module);
		}
	}

}