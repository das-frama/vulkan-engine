#include "renderer_frontend.h"

#include "renderer_backend.h"
#include "core/logger.h"
#include "core/memory.h"

// Backend render context.
static renderer_backend* backend = 0;

b8 renderer_init(const char* app_name, struct platform_state* plat_state) {
	backend = memory_allocate(sizeof(renderer_backend), MEMORY_TAG_RENDERER);

	renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN, plat_state, backend);
	backend->frame_number = 0;

	if (!backend->init(backend, app_name, plat_state)) {
		FATAL("Renderer backend failed to initialize. Shutting down...");
		return false;
	}

	return true;
}

void renderer_shut() {
	backend->shut(backend);
	memory_free(backend, sizeof(renderer_backend), MEMORY_TAG_RENDERER);
}

b8 renderer_begin_frame(f32 dt) {
	return backend->begin_frame(backend, dt);
}

b8 renderer_end_frame(f32 dt) {
	b8 result = backend->end_frame(backend, dt);
	backend->frame_number++;
	return result;
}

b8 renderer_draw_frame(render_packet* packet) {
	// if the begin frame returned successfully mid-frame ops may continue.
	if (renderer_begin_frame(packet->dt)) {
		// End the frame. If this fails, it is likely unrecoverable.
		b8 result = renderer_end_frame(packet->dt);
		if (!result) {
			ERROR("renderer_end_frame failed. Application shutting down...");
			return false;
		}
	}

	return true;
}
