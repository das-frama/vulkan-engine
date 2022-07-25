#include "application.h"
#include "game_types.h"

#include "logger.h"
#include "platform/platform.h"
#include "core/memory.h"
#include "core/event.h"

typedef struct {
	game* game_inst;
	b8 is_running;
	b8 is_suspended;
	platform_state platform;
	i16 width;
	i16 height;
	f64 last_time;
} application_state;

static b8 initialized = false;
static application_state app_state;

b8 application_create(game* game_inst) {
	if (initialized) {
		ERROR("Application already initialized.");
		return false;
	}
    
	app_state.game_inst = game_inst;
    
	// Init subsystems.
	init_logging();
    
	app_state.is_running = true;
	app_state.is_suspended = false;

	if (!event_init()) {
		ERROR("Event system failed initialization. Application cannot continue.");
		return false;
	}
    
	if (!platform_start(
                        &app_state.platform, 
                        game_inst->app_config.name, 
                        game_inst->app_config.start_pos_x, 
                        game_inst->app_config.start_pos_y,
                        game_inst->app_config.start_width, 
                        game_inst->app_config.start_height)) {
		return false;
    }
    
    // Initialize the game.
    if (!app_state.game_inst->init(app_state.game_inst)) {
    	FATAL("Game failed to initialize.");
    	return false;
    }
    
    app_state.game_inst->on_resize(app_state.game_inst, app_state.width, app_state.height);
    
    initialized = true;
    
    return true;
}

b8 application_run() {
	// INFO(get_memory_usage_str());
	
	while (app_state.is_running) {
		if (!platform_pump_messages(&app_state.platform)) {
			app_state.is_running = false;
		}
        
		if (!app_state.is_suspended) {
			// Update.
			if (!app_state.game_inst->update(app_state.game_inst, (f32)0)) {
				FATAL("Game update failed, shutting down.");
				app_state.is_running = false;
				break;
			}

			// Render.
			if (!app_state.game_inst->render(app_state.game_inst, (f32)0)) {
				FATAL("Game render failed, shutting down.");
				app_state.is_running = false;
				break;
			}
		}
    }
    
    app_state.is_running = false;

    event_shut();
    
    platform_shut(&app_state.platform);
    
    return true;
}
