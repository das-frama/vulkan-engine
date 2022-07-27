#include "string.h"
#include "memory.h"

#include <string.h>

u64 string_len(const char* str) {
	return strlen(str);
}

char* string_dup(const char* str) {
	u64 len = string_len(str);
	char* copy = memory_allocate(len + 1, MEMORY_TAG_STRING);
	memory_copy(copy, str, len + 1);
	return copy;
}

b8 strings_equal(const char* str0, const char* str1) {
	return strcmp(str0, str1) == 0;
}

