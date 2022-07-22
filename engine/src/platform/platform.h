#pragma once

#include "defines.h"

typedef struct platform_state {
    void* internal_state;
} platform_state;

API b8 platform_start(platform_state* state, const char* app_name, i32 x, i32 y, i32 width, i32 height);
API void platform_shut(platform_state* state);
API b8 platform_pump_messages(platform_state* state);

void* platform_alloc(u64 size, b8 aligned);
void platform_free(void* block, b8 aligned);
void* platform_zero_mem(void* block, u64 size);
void* platform_copy_mem(void* dest, const void* src, u64 size);
void* platform_set_mem(void* dest, i32 value, u64 size);

void platform_console_write(const char* msg, u8 color);
void platform_console_write_err(const char* msg, u8 color);

f64 platform_get_abs_time();
void platform_sleep(u64 ms);