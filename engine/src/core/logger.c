#include "logger.h"
#include "asserts.h"
#include "platform/platform.h"

// TODO: temporary
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

b8 init_logging() {
    // TODO: create log file.
    
    return true;
}

void shut_logging() {
    // TODO: cleanup logging entries.
}

void log_output(log_level level, const char* message, ...) {
    const char* level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]:  ", "[INFO]:  ", "[DEBUG]: ", "[TRACE]: "};
    
    b8 is_error = level < LOG_LEVEL_WARN;
    
    const i32 msg_len = 32000;
    char out_message[msg_len];
    memset(out_message, 0, sizeof(out_message));
    
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, msg_len, message, arg_ptr);
    va_end(arg_ptr);
    
    char out_message2[msg_len];
    sprintf(out_message2, "%s%s\n", level_strings[level], out_message);
    
    if (is_error) {
        platform_console_write_err(out_message2, level);
    } else {
        platform_console_write(out_message2, level);
    }
}

void report_assert_failure(const char* expr, const char* msg, const char* file, i32 line) {
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expr, msg, file, line);
}
