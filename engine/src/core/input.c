#include "input.h"

#include "core/event.h"
#include "core/memory.h"
#include "core/logger.h"

typedef struct keyboard_state {
	b8 keys[256];
} keyboard_state;

typedef struct mouse_state {
	i16 x;
	i16 y;
	u8 buttons[BUTTON_MAX_BUTTONS];
} mouse_state;

typedef struct input_state {
	keyboard_state keyboard_curr;
	keyboard_state keyboard_prev;
	mouse_state mouse_curr;
	mouse_state mouse_prev;
} input_state;

static b8 initialized = false;
static input_state state = {};

void input_init() {
	memory_zero(&state, sizeof(input_state));
	initialized = true;
	INFO("Input subsystem initialized.");
}

void input_shut() {
	initialized = false;
}

void input_update(f64 dt) {
	if (!initialized) {
		return;
	}

	// Copy current state to previous state.
	memory_copy(&state.keyboard_prev, &state.keyboard_curr, sizeof(keyboard_state));
	memory_copy(&state.mouse_prev, &state.mouse_curr, sizeof(mouse_state));
}


void input_process_key(keys key, b8 pressed) {
	// Only handle this if the state actually changed.
	if (state.keyboard_curr.keys[key] != pressed) {
		state.keyboard_curr.keys[key] = pressed;

		// Fire event.
		event_context context;
		context.data.u16[0] = key;
		event_fire(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED, 0, context);
	}
}

void input_process_button(buttons button, b8 pressed) {
	if (state.mouse_curr.buttons[button] != pressed) {
		state.mouse_curr.buttons[button] = pressed;

		// Fire event.
		event_context context;
		context.data.u16[0] = button;
		event_fire(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED, 0, context);
	}
}

void input_process_mouse_move(i16 x, i16 y) {
	if (state.mouse_curr.x != x || state.mouse_curr.y != y) {
		// DEBUG("Mouse pos: %i, %i", x, y);

		// Update internal state.
		state.mouse_curr.x = x;
		state.mouse_curr.y = y;

		// Fire event.
		event_context context;
		context.data.u16[0] = x;
		context.data.u16[1] = y;
		event_fire(EVENT_CODE_MOUSE_MOVED, 0, context);
	}
}

void input_process_mouse_wheel(i8 z_delta) {
	event_context context;
	context.data.u8[0] = z_delta;
	event_fire(EVENT_CODE_MOUSE_WHEEL, 0, context);
}

b8 input_is_key_down(keys key) {
	if (!initialized) {
		return false;
	}	

	return state.keyboard_curr.keys[key] == true;
}

b8 input_is_key_up(keys key) {
	if (!initialized) {
		return true;
	}	

	return state.keyboard_curr.keys[key] == false;
}

b8 input_was_key_down(keys key) {
	if (!initialized) {
		return false;
	}	

	return state.keyboard_prev.keys[key] == true;
}

b8 input_was_key_up(keys key) {
	if (!initialized) {
		return true;
	}	

	return state.keyboard_prev.keys[key] == false;
}


b8 input_is_button_down(buttons button) {
	if (!initialized) {
		return false;
	}	

	return state.mouse_curr.buttons[button] == true;
}

b8 input_is_button_up(buttons button) {
	if (!initialized) {
		return true;
	}	

	return state.mouse_curr.buttons[button] == false;
}

b8 input_was_button_down(buttons button) {
	if (!initialized) {
		return false;
	}	

	return state.mouse_prev.buttons[button] == true;
}

b8 input_was_button_up(buttons button) {
	if (!initialized) {
		return true;
	}	

	return state.mouse_prev.buttons[button] == false;
}

void input_get_mouse_pos(i32* x, i32* y) {
	if (!initialized) {
		*x = *y = 0;
		return;	
	}

	*x = state.mouse_curr.x;
	*y = state.mouse_curr.y;
}

void input_get_prev_mouse_pos(i32* x, i32* y) {
	if (!initialized) {
		*x = *y = 0;
		return;	
	}

	*x = state.mouse_prev.x;
	*y = state.mouse_prev.y;
}
