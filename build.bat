@echo off

if not exist .\build mkdir .\build

pushd .\build
cl ..\platform.cpp ..\glad.c -Z7 -I../vendor/include -link -LIBPATH:..\vendor\libs^
 soloud_static_x64_d.lib glfw3dll.lib Gdi32.lib User32.lib Shell32.lib Opengl32.lib
popd