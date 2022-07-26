#include "platform.h"

#if PLATFORM_WINDOWS

#include "core/logger.h"
#include "core/input.h"

#include "containers/darray.h"

#include <windows.h>
#include <windowsx.h>

// For surface creation.
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include "renderer/vulkan/vulkan_types.inl"

typedef struct internal_state {
    HINSTANCE h_instance;
    HWND hwnd;
    VkSurfaceKHR surface; // TODO(frama): Temporarly...
} internal_state;

static f64 clock_freq;
static LARGE_INTEGER start_time;

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param);

b8 platform_start(
                  platform_state* state,
                  const char* app_name,
                  i32 x, i32 y,
                  i32 width, i32 height) {
    state->internal_state = malloc(sizeof(internal_state));
    internal_state* i_state = (internal_state*)state->internal_state;
    
    i_state->h_instance = GetModuleHandleA(0);
    
    HICON icon = LoadIcon(i_state->h_instance, IDI_APPLICATION);
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = win32_process_message;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = i_state->h_instance;
    wc.hIcon = icon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = "vulkan_window_class";
    
    if (!RegisterClassA(&wc)) {
        MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }
    
    u32 client_x = x;
    u32 client_y = y;
    u32 client_width = width;
    u32 client_height = height;
    
    u32 window_x = client_x;
    u32 window_y = client_y;
    u32 window_width = client_width;
    u32 window_height = client_height;
    
    u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    u32 window_ex_style = WS_EX_APPWINDOW;
    window_style |= WS_MAXIMIZEBOX;
    window_style |= WS_MINIMIZEBOX;
    window_style |= WS_THICKFRAME;
    
    RECT border_rect = {0, 0, 0, 0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);
    window_x += border_rect.left;
    window_y += border_rect.top;
    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;
    
    HWND handle = CreateWindowExA(
                                  window_ex_style, "vulkan_window_class", app_name, window_style,
                                  window_x, window_y, window_width, window_height,
                                  0, 0, i_state->h_instance, 0);
    
    if (handle == 0) {
        MessageBoxA(NULL, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        FATAL("Window creation failed!");
        return false;
    }
    
    i_state->hwnd = handle;
    
    b32 should_activate = 1;
    i32 show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
    ShowWindow(i_state->hwnd, show_window_command_flags);
    
    // Clock setup.
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    clock_freq = 1.0f / (f64)freq.QuadPart;
    QueryPerformanceCounter(&start_time);
    
    return true;
}

void platform_shut(platform_state* state) {
    internal_state* i_state = (internal_state*)state->internal_state;
    
    if (i_state->hwnd) {
        DestroyWindow(i_state->hwnd);
        i_state->hwnd = 0;
    }
}

b8 platform_pump_messages(platform_state* state) {
    MSG message;
    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
    
    return true;
}

void* platform_alloc(u64 size, b8 aligned) {
    return malloc(size);
}

void platform_free(void* block, b8 aligned) {
    free(block);
}

void* platform_zero_mem(void* block, u64 size) {
    return memset(block, 0, size);
}

void* platform_copy_mem(void* dest, const void* src, u64 size) {
    return memcpy(dest, src, size);
}

void* platform_set_mem(void* dest, i32 value, u64 size) {
    return memset(dest, value, size);
}

void platform_console_write(const char* msg, u8 color) {
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(console_handle, levels[color]);
    
    OutputDebugStringA(msg);
    u64 len = strlen(msg);
    LPDWORD num_written = 0;
    WriteConsoleA(console_handle, msg, (DWORD)len, num_written, 0);
}

void platform_console_write_err(const char* msg, u8 color) {
    HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(console_handle, levels[color]);
    
    OutputDebugStringA(msg);
    u64 len = strlen(msg);
    LPDWORD num_written = 0;
    WriteConsoleA(console_handle, msg, (DWORD)len, num_written, 0);
}

f64 platform_get_abs_time() {
    LARGE_INTEGER now_time;
    QueryPerformanceCounter(&now_time);
    return (f64)now_time.QuadPart * clock_freq;
}

void platform_sleep(u64 ms) {
    Sleep(ms);
}

void platform_get_required_extension_names(const char*** names_darray) {
    darray_push(*names_darray, &"VK_KHR_win32_surface");
}

b8 platform_create_vulkan_surface(platform_state* plat_state, vulkan_context* context) {
    internal_state* state = (internal_state*)plat_state->internal_state;
    
    VkWin32SurfaceCreateInfoKHR create_info = {VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    create_info.hinstance = state->h_instance;
    create_info.hwnd = state->hwnd;
    
    VkResult result = vkCreateWin32SurfaceKHR(context->instance,
                                              &create_info, 
                                              context->allocator,
                                              &state->surface);
    if (result != VK_SUCCESS) {
        FATAL("Vulkan surface creation failed!");
        return false;
    }
    
    context->surface = state->surface;
    return true;
}

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param) {
    switch (msg) {
        case WM_ERASEBKGND:
        return 1;
        case WM_CLOSE:
        // TODO: fire event for app to quit.
        return 0;
        case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
        case WM_SIZE: {
            // RECT r;
            // GetClientRect(hwnd, &r);
            // u32 width = r.right - r.left;
            // u32 height = r.bottom - r.top;
            // TODD: fire an event for window to resize
        } break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            b8 pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            keys key = (u16)w_param;
            
            input_process_key(key, pressed);
        } break;
        case WM_MOUSEMOVE: {
            i32 x_pos = GET_X_LPARAM(l_param);
            i32 y_pos = GET_Y_LPARAM(l_param);
            
            input_process_mouse_move(x_pos, y_pos);
        } break;
        case WM_MOUSEWHEEL: {
            i32 z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
            if (z_delta != 0) {
                z_delta = (z_delta < 0) ? -1 : 1;
                input_process_mouse_wheel(z_delta);
            }
            
        } break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
            b8 pressed = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
            buttons mouse_button = BUTTON_MAX_BUTTONS;
            switch (msg) {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                mouse_button = BUTTON_LEFT;
                break;
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                mouse_button = BUTTON_RIGHT;
                break;
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                mouse_button = BUTTON_MIDDLE;
                break;
            }
            if (mouse_button != BUTTON_MAX_BUTTONS) {
                input_process_button(mouse_button, pressed);
            }
        } break;
    }
    
    return DefWindowProcA(hwnd, msg, w_param, l_param);
}

#endif