#pragma once

#include "renderer_types.inl"

struct static_mesh_data;
struct platform_state;

b8 renderer_init(const char* app_name, struct platform_state* state);
void renderer_shut();

void renderer_on_resized(u16 width, u16 height);

b8 renderer_draw_frame(render_packet* packet);
