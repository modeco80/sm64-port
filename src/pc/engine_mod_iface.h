// This stuff is meant for the engine
// to interact with the mod api

#include "types.h"

#ifdef _cplusplus
extern "C" {
#endif

// load all mods (at start)
void mod_load_all();

// initalize all enabled mods
void mod_init_all();

void mod_frame_all();

void mod_exit_all();

#ifdef _cplusplus
}
#endif