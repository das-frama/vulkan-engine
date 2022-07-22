#include <core/logger.h>
#include <core/asserts.h>

#include <platform/platform.h>

int main(void) {
    FATAL("A test fatal message: %f", 3.14f);
    ERROR("A test error message: %f", 3.14);
    WARN("A test warn message: %f", 3.14);
    INFO("A test info message: %f", 3.14);
    DEBUG("A test debug message: %f", 3.14);
    TRACE("A test trace message: %f", 3.14);

    platform_state state;
    if (platform_start(&state, "Vulkan Game Engine", 100, 100, 1280, 720)) {
        while (platform_pump_messages(&state)) {

        }
    }

    platform_shut(&state);
    return 0;
}