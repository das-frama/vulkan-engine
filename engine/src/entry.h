#pragma once

#include "core/application.h"
#include "core/logger.h"
#include "core/memory.h"
#include "game_types.h"

extern b8 create_game(game* out_game);

/**
 * The main entry of the application.
 */
int main(void) {
	// Pre init.
	memory_init();
    
	// Create a game instance.
	game game_inst;
	if (!create_game(&game_inst)) {
		ERROR("Could not create game!");
		return -1;
	}
    
	// Ensure the function pointer exist.
	if (!game_inst.render || !game_inst.update || !game_inst.init || !game_inst.on_resize) {
		FATAL("The game's function pointers must be assigned!");
		return -2;
	}
    
	// Initialization.
    if (!application_create(&game_inst)) {
    	INFO("Application failed to create!");
    	return 1;
    }
    
    // Begin game loop.
    if (!application_run()) {
		INFO("Application did not shutdown gracefully,");
    	return 2;
    }
    
    memory_shut();
    
    return 0;
}