#pragma once

#include "renderer/renderer_backend.h"

b8 vulkan_renderer_backend_init(renderer_backend* backend, const char* app_name, struct platform_state* plat_state);
void vulkan_renderer_backend_shut(renderer_backend* backend);

void vulkan_renderer_backend_on_resized(renderer_backend* backend, u16 width, u16 height);

b8 vulkan_renderer_backend_begin_frame(renderer_backend* backend, f32 dt);
b8 vulkan_renderer_backend_end_frame(renderer_backend* backend, f32 dt);
