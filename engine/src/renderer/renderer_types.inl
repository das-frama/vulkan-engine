#pragma once

#include "defines.h"

typedef enum renderer_backend_type {
	RENDERER_BACKEND_TYPE_VULKAN,
	RENDERER_BACKEND_TYPE_OPENGL,
	RENDERER_BACKEND_TYPE_DIRECTX
} renderer_backend_type;

typedef struct renderer_backend {
	struct platform_state* plat_state;
	u64 frame_number;

	b8 (*init)(struct renderer_backend* backend, const char* app_name, struct platform_state* plat_state);
	void (*shut)(struct renderer_backend* backend);

	void (*resized)(struct renderer_backend* backend, u16 width, u16 height);

	b8 (*begin_frame)(struct renderer_backend* backend, f32 dt);
	b8 (*end_frame)(struct renderer_backend* backend, f32 dt);
} renderer_backend;

typedef struct render_packet {
	f32 dt;
} render_packet;