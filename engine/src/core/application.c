#include "application.h"
#include "game_types.h"

#include "logger.h"
#include "platform/platform.h"
#include "core/memory.h"
#include "core/event.h"
#include "core/input.h"
#include "core/clock.h"

typedef struct {
	game* game_inst;
	b8 is_running;
	b8 is_suspended;
	platform_state platform;
	i16 width;
	i16 height;
	clock clock;
	f64 last_time;
} application_state;

static b8 initialized = false;
static application_state app_state;

b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context);
b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context);

b8 application_create(game* game_inst) {
	if (initialized) {
		ERROR("Application already initialized.");
		return false;
	}
    
	app_state.game_inst = game_inst;
    
	// Init subsystems.
	init_logging();
	input_init();
    
	app_state.is_running = true;
	app_state.is_suspended = false;

	if (!event_init()) {
		ERROR("Event system failed initialization. Application cannot continue.");
		return false;
	}

	// Listen events.
	event_register(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
	event_register(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
	event_register(EVENT_CODE_KEY_RELEASED, 0, application_on_key);
    
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
	clock_start(&app_state.clock);
	clock_update(&app_state.clock);
	app_state.last_time = app_state.clock.elapsed;
	f64 running_time = 0;
	u8 frame_count = 0;
	f64 target_frame_seconds = 1.0f / 60.0f; // 16.6ms
	// INFO(get_memory_usage_str());
	
	while (app_state.is_running) {
		if (!platform_pump_messages(&app_state.platform)) {
			app_state.is_running = false;
		}
        
		if (!app_state.is_suspended) {
			// Update clock.
			clock_update(&app_state.clock);
			f64 current_time = app_state.clock.elapsed;
			f64 dt = current_time - app_state.last_time;
			f64 frame_start_time = platform_get_abs_time();

			// Update.
			if (!app_state.game_inst->update(app_state.game_inst, (f32)delta)) {
				FATAL("Game update failed, shutting down.");
				app_state.is_running = false;
				break;
			}

			// Render.
			if (!app_state.game_inst->render(app_state.game_inst, (f32)delta)) {
				FATAL("Game render failed, shutting down.");
				app_state.is_running = false;
				break;
			}

			// How long the frame took.
			f64 frame_end_time = platform_get_abs_time();
			f64 frame_elapsed_time = frame_end_time - frame_start_time;
			running_time += frame_elapsed_time;
			f64 remaining_seconds = target_frame_seconds - frame_elapsed_time;

			if (remaining_seconds > 0) {
				u64 remaining_ms = (remaining_seconds * 1000);
				// if tehere is time left, give it back to the OS.
				b8 limit_frames = false;
				if (remaining_ms > 0 && limit_frames) {
					platform_sleep(remaining_ms - 1);
				}

				frame_count++;
			}

			// Process input.
			input_update(delta);

			// Update last time. 
			app_state.last_time = current_time;
		}
    }
    
    app_state.is_running = false;

    event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
	event_unregister(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
	event_unregister(EVENT_CODE_KEY_RELEASED, 0, application_on_key);
    event_shut();
    input_shut();
    
    platform_shut(&app_state.platform);
    
    return true;
}

b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context) {
	switch (code) {
		case EVENT_CODE_APPLICATION_QUIT: {
			INFO("EVENT_CODE_APPLICATION_QUIT recieved, shutting down");
			app_state.is_running = false;
			return true;
		}
	}

	return false;
}

b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context) {
	if (code == EVENT_CODE_KEY_PRESSED) {
		u16 key_code = context.data.u16[0];
		if (key_code == KEY_ESCAPE) {
			event_context data = {};
			event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);
			return true;
		} else if (key_code == KEY_A) {
			DEBUG("Explicit - A key pressed.");
		} else {
			DEBUG("'%c' key pressed in window.", key_code);
		}
	} else if (code == EVENT_CODE_KEY_RELEASED) {
		u16 key_code = context.data.u16[0];
		if (key_code == KEY_B) {
			DEBUG("Explicit - B key released.");
		} else {
			DEBUG("'%c' key released in window.", key_code);
		}
	}

	return false;
}
