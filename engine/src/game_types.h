#pragma once

#include "core/application.h"

typedef struct game {
	application_config app_config;
    
	b8 (*init)(struct game* game_inst);
	b8 (*update)(struct game* game_inst, f32 dt);
	b8 (*render)(struct game* game_inst, f32 dt);
	void (*on_resize)(struct game* game_inst, u32 width, u32 height);
    
	void* state;
} game;
