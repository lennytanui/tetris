@echo off
call cd vendor

if not exist %~dp0%\build (
    echo "build directory does no exist. run ./build.bat first"
)

call ..\build\platform.exe 
cd ..