#include "memory.c"

#include "core/logger.h"
#include "platform/platform.h"

#include <string.h>
#include <stdio.h>

struct memory_stats {
	u64 total_allocated;
	u64 tagged_allocations[MEMORY_TAG_MAX_TAGS];
}

static const char* memory_tag_strings[MEMORY_TAG_MAX_TAGS] = {
	"UNKNOWN	",
	"ARRAY		",
	"DARRAY		",
	"DICT		",
	"RING_QUEUE ",
	"BST		",
	"STRING		",
	"APPLICATION",
	"JOB		",
	"TEXTURE	",
	"MAT_INST	",
	"RENDERER	",
	"GAME		",
	"TRANSFORM	",
	"ENTITY		",
	"ENTITY_NODE",
	"SCENE      "};

static struct memory_stats stats;

void memory_init() {
	platform_zero_mem(&stats, sizeof(stats));
}

void memory_shut() {
}

void* memory_allocate(u64 size, memory_tag tag) {
	if (tag == MEMORY_TAG_UNKNOWN) {
		WARN("allocate() called using MEMORY_TAG_UNKNOWN. Re-class this allocation.");
	}

	stats.total_allocated += size;
	stats.tagged_allocations[tag] += size;

	// TODO: Memory alignment.
	void* block = platform_alloc(size, false);
	platform_zero_mem(block, size);

	return block;
}

void memory_free(void* block, u64 size, memory_tag tag) {
	if (tag == MEMORY_TAG_UNKNOWN) {
		WARN("free() called using MEMORY_TAG_UNKNOWN. Re-class this allocation.");
	}

	stats.total_allocated -= size;
	stats.tagged_allocations[tag] -= size;

	// TODO: Memory alignment.
	platform_free(block, false);
}

void* memory_zero(void* block, u64 size) {
	return platform_zero_mem(block, size);
}

void* memory_copy(void* dest, const void* src, u64 size) {
	return platform_copy_mem(dest, src, size);
}

void* memory_set(void* dest, i32 value, u64 size) {
	return platform_set_mem(dest, value, size);
}

char* get_memory_usage_str() {
	const u64 gib = 1024 * 1024 * 1024;
	const u64 mib = 1024 * 1024;
	const u64 kib = 1024;

	char buffer[8000] = "System memory use (tagged):\n";
	u64 offset = strlen(buffer);
	for (u32 i = 0; i < MEMORY_TAG_MAX_TAGS; ++i) {
		char unit[4] = "XiB";
		float amount = 1.0f;
		if (stats.tagged_allocations[i] >= gib) {
			unit[0] = 'G';
			amount = stats.tagged_allocations[i] / (float)gib;
		} else if (stats.tagged_allocations[i] >= mib) {
			unit[0] = 'M';
			amount = stats.tagged_allocations[i] / (float)mib;
		} else if (stats.tagged_allocations[i] >= gib) {
			unit[0] = 'K';
			amount = stats.tagged_allocations[i] / (float)kib;
		} else {
			unit[0] = 'B';
			unit[1] = 0;
			amount = (float)stats.tagged_allocations[i];
		}

		i32 len = snprintf(buffer + offset, 8000, "  %s: %.2f%s\n", memory_tag_strings[i], amount, unit);
		offset += len;
	}

	char* out_str = _strdup(buffer);
	return out_str;
}