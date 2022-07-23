REM Build script for engine
@ECHO OFF
setlocal EnableDelayedExpansion

REM Get a list of all line .c files
SET c_files= 
FOR /R %%f in (*.c) do (
	SET c_files=!c_files! %%f
)

REM echo "Files:" %c_files%

SET assembly=engine
SET compiler_flags=-g -shared -Wvarargs -Wall -Werror -std=c17
REM -Wall -Werror
SET include_flags=-Isrc -I%VULKAN_SDK%/Include
SET linker_flags=-luser32 -lvulkan-1 -L%VULKAN_SDK%/Lib
SET defines=-D_DEBUG -DEXPORT -D_CRT_SECURE_NO_WARNINGS

ECHO "Building %assembly%..."
clang %c_files% %compiler_flags% -o ../bin/%assembly%.dll %defines% %include_flags% %linker_flags%