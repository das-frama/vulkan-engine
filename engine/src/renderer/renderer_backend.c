#include "renderer_backend.h"

#include "vulkan/vulkan_backend.h"

b8 renderer_backend_create(renderer_backend_type type, struct platform_state* plat_state, renderer_backend* out_renderer_backend) {
	out_renderer_backend->plat_state = plat_state;

	if (type == RENDERER_BACKEND_TYPE_VULKAN) {
		out_renderer_backend->init = vulkan_renderer_backend_init;
		out_renderer_backend->shut = vulkan_renderer_backend_shut;
		out_renderer_backend->begin_frame = vulkan_renderer_backend_begin_frame;
		out_renderer_backend->end_frame = vulkan_renderer_backend_end_frame;
		out_renderer_backend->resized = vulkan_renderer_backend_on_resized;

		return true;
	}

	return false;
}

void renderer_backend_destroy(renderer_backend* renderer_backend) {
	renderer_backend->init = 0;
	renderer_backend->shut = 0;
	renderer_backend->begin_frame = 0;
	renderer_backend->end_frame = 0;
	renderer_backend->resized = 0;
}