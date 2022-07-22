REM Build script for testbed
@ECHO OFF
SetLocal EnableDelayedExpansion

REM Get a list of all the .c files.
SET c_filenames=
FOR /R %%f in (*.c) do (
    SET c_filenames=!c_filenames! %%f
)

REM echo "Files:" %c_filenames%

SET assembly=testbed
SET compiler_flags=-g 
REM -Wall -Werror
SET include_flags=-Isrc -I../engine/src/
SET linker_flags=-L../bin/ -lengine.lib
SET defines=-D_DEBUG -DKIMPORT

ECHO "Building %assembly%%..."
clang %c_filenames% %compiler_flags% -o ../bin/%assembly%.exe %defines% %include_flags% %linker_flags%
