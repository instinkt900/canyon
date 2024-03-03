@echo off
setlocal EnableDelayedExpansion

set OUTPUT_HEADER="vulkan_shaders.h"
set OUTPUT_SOURCE="vulkan_shaders.cpp"

del %OUTPUT_HEADER%
del %OUTPUT_SOURCE%

echo #pragma once >> %OUTPUT_HEADER%
echo. >> %OUTPUT_HEADER%

for %%f in (
	drawing_shader.vert
	drawing_shader.frag
	font.vert
	font.frag
) do call :Generate %%f
goto End

:Generate
set OUTPUT_SPV=%1.spv
echo extern unsigned char %OUTPUT_SPV:.=_%[]; >> %OUTPUT_HEADER%
echo extern unsigned int %OUTPUT_SPV:.=_%_len; >> %OUTPUT_HEADER%
echo /** >> %OUTPUT_SOURCE%
type %1 >> %OUTPUT_SOURCE%
echo */ >> %OUTPUT_SOURCE%
glslc %1 -o %OUTPUT_SPV%%
xxd -i %1.spv >> %OUTPUT_SOURCE%
echo. >> %OUTPUT_SOURCE%
del %1.spv
goto :eof

:End
