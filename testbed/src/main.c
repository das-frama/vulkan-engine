#include <core/logger.h>
#include <core/asserts.h>

int main(void) {
	FATAL("A test fatal message: %f", 3.14f);
	ERROR("A test error message: %f", 3.14);
	WARN("A test warn message: %f", 3.14);
	INFO("A test info message: %f", 3.14);
	DEBUG("A test debug message: %f", 3.14);
	TRACE("A test trace message: %f", 3.14);

	ASSERT(false);

  	return 0;
}