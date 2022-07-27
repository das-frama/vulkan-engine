#pragma once

#include "defines.h"

API u64 string_len(const char* str);

API char* string_dup(const char* str);

API b8 strings_equal(const char* str0, const char* str1);