#!/bin/bash

# Build script for engine
set echo on

mkdir -p ../bin

# Get a list of all the .c files.
c_files=$(find . -type f \( -name "*.c" -o -name "*.m" \))

# echo "files:" $c_files

assembly="engine"
compiler_flags="-g -shared -fdeclspec -fPIC -std=c17 -ObjC"
include_flags="-Isrc -I/Users/frama/VulkanSDK/1.3.216.0/macOS/include"
linker_flags="-lvulkan -L/Users/frama/VulkanSDK/1.3.216.0/macOS/lib -lobjc -framework AppKit -framework QuartzCore"
defines="-D_DEBUG -DEXPORT"

echo "Building $assembly..."
clang $c_files $compiler_flags -o ../bin/lib$assembly.so $defines $include_flags $linker_flags
