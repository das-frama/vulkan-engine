#include "game.h"

#include <core/logger.h>

b8 game_init(game* game_inst) {
	DEBUG("game_init() called");

	return true;
}

b8 game_update(game* game_inst, f32 dt) {
	// DEBUG("game_update() called");
	return true;
}

b8 game_render(game* game_inst, f32 dt) {
	// DEBUG("game_render() called");
	return true;
}

void game_on_resize(game* game_inst, u32 width, u32 height) {
}
