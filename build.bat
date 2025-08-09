@REM @echo off

@REM if not exist .\build mkdir .\build

@REM pushd .\build
@REM cl ..\platform.cpp ..\glad.c -Z7 -I../vendor/include -link -LIBPATH:..\vendor\libs^
@REM  soloud_static_x64_d.lib glfw3dll.lib Gdi32.lib User32.lib Shell32.lib Opengl32.lib
@REM popd


@echo off


@REM ENV (environment) can either be GNU, MSVC, or WEB
set ENV= MSVC

if %ENV% == MSVC (
    where cl >nul 2>nul

    if %errorlevel% neq 0 (
        echo "MSVC 'cl' command does not exist"
        echo "  Cannot continue. Exiting"
        exit /b 1
    ) 
)


set makecall= mingw32-make

set GLFW_DIR= ".\vendor\libs_src\glfw"
set FreeType_DIR= ".\vendor\libs_src\freetype-2.10.0"
set EMSDK_DIR= ".\vendor\libs_src\emdsk"

if not exist ".\build" (
    echo "Creating `.\build` directory in %cd"
    mkdir ".\build"
)

if not exist ".\build\vendor" (
    echo "Creating `.\build\vendor` directory"
    mkdir ".\build\vendor"
)

if not exist ".\vendor\libs" (
    echo "Creating `.\vendor\libs` directory"
    mkdir ".\vendor\libs"
)

@REM check if EMSDK folder is empty
dir /b /s /a %EMSDK_DIR% | findstr . > nul || (
    echo EMSDK Folder is empty
    
    pushd "../emsdk"
    call git submodule init
    call git submodule update
    popd
)


@REM check if GLFW folder is empty and if so Initialize and Update
dir /b /s /a %GLFW_DIR% | findstr . > nul || (
    @REM The program should never really run this nest of commands
    echo GLFW Folder is empty

    pushd "../glfw"
    call git submodule init
    call git submodule update
    popd
)

@REM check if FreeType folder is empty and if so Initialize and Update
dir /b /s /a %FreeType_DIR% | findstr . > nul || (
    @REM The program should never really run this nest of commands
    echo FreeType Folder is empty

    pushd "../freetype"
    call git submodule init
    call git submodule update
    popd
)

if %ENV% == WEB (
    @REM start  em++
    WHERE em++ /Q
    if ERRORLEVEL 1 (
        @REM how do I if this fails?
        echo Starting EM++
        echo NOTE : First time run may take some time
        call ..\emsdk\emsdk install latest
        call ..\emsdk\emsdk activate latest
        call "..\emsdk\emsdk_env.bat"
    )
)

set Skip= false

if exist ".\vendor\glfw3.dll" (
    set Skip= true
)

if %ENV% == WEB (
    set Skip= true
)

if %Skip% == true (
    GOTO skip_GLFW_build
)

@REM --------- Start of GLFW Build ------------

echo --------------------- building GLFW
@REM copy ".\glfw_config.h" "..\glfw\src" >nul
pushd ".\vendor\libs_src\glfw\src"

if not exist "..\..\..\include\GLFW" (
    robocopy .\include\GLFW\ ..\..\..\include\GLFW  /E /NJH /NJS /NFL /NDL
)

if not exist ".\build" (
    echo "Creating `.\build` directory"
    mkdir ".\build\"
)

if not exist ".\temp" (
    echo "Creating `.\build` directory"
    mkdir ".\temp\"
)  

@REM move files not related to windows to temporary directory
set source_folder=.\
set search_words=x11 wl linux cocoa glx xkb posix
set destination_folder=.\temp

for %%W in (%search_words%) do (
    for /R %source_folder% %%F in (*%%W*) do (
        move "%%F" "%destination_folder%" >nul
    )
)

pushd "./build"
    @REM (is building glfw with MSVC even neccessary?) 
    @REM cl /D_USRDLL /D_WINDLL -I../ -I../../deps ../*.c -D_GLFW_USE_CONFIG_H -D_GLFW_BUILD_DLL Gdi32.lib User32.lib Shell32.lib /link /DLL /OUT:glfw3.dll

    gcc -c ../*.c -I../../deps -D_GLFW_BUILD_DLL -D_GLFW_USE_CONFIG_H -lOpengl32 -lShell32 -lGdi32 -lUser32
    gcc -shared -o glfw3.dll *.o -Wl,--out-implib,libglfw3.a -lOpengl32 -lShell32 -lGdi32 -lUser32
    copy "glfw3.dll" "../../../../" 
    copy "libglfw3.a" "../../../../libs"
popd

@REM restore files from temporary directory
for %%W in (%search_words%) do (
    for /R %destination_folder% %%F in (*%%W*) do (
        move "%%F" "%source_folder%" >nul
    )
)

@REM --------- End of GLFW Build ------------

set source_folder= 
set destination_folder= 
set search_words= 

popd

:skip_GLFW_build

@REM ---------Start of FreeType Build ----------------

@REM will require make sadly
echo --------------------- building FreeType
pushd %FreeType_DIR%
set FreeType_LIB_DIR= "..\..\libs\libfreetype.a"

if %ENV% == GNU (
    
    if exist %FreeType_LIB_DIR% (
        GOTO skip_FREETYPE_build
    )
    
    call %makecall% setup gcc
)

if %ENV% == WEB (
    
    if exist %FreeType_LIB_DIR% (
        GOTO skip_FREETYPE_build
    )

    call %makecall% setup gcc
)

if %ENV% == MSVC (
    if exist "..\..\libs\freetype.lib" (
        GOTO skip_FREETYPE_build
    )

    @REM copy "..\demo\vendor\visualc.mk" ".\builds\compiler\"
    call dir
    call %makecall% setup visualc
)
call %makecall%

@REM xcopy ".\include\" "..\..\include" /E /Y

if %ENV% == GNU (
    copy ".\objs\freetype.a" "..\libs\libfreetype.a"
)

if %ENV% == WEB (
    copy ".\objs\freetype.a" "..\libs\libfreetype.a"
)

if %ENV% == MSVC (
    copy ".\objs\freetype.lib" "..\..\libs\freetype.lib"
)

:skip_FREETYPE_build
popd

@REM ---------End of FreeType Build ----------------


@REM ---------Start of Soloud Build ----------------
echo --------------------- building Soloud


if not exist "./vendor/libs_src/soloud20200207/src/build" (
    echo "Creating `.\build` directory"
    mkdir "./vendor/libs_src/soloud20200207/src/build"
)

pushd "./vendor/libs_src/soloud20200207/src/build"

if %ENV% == MSVC (
    if exist "..\..\..\..\libs\soloud.lib" (
        GOTO skip_SOLOUD_build
    )
    call dir
    cl /c /DWITH_MINIAUDIO -I../../include/ ../audiosource/wav/*.cpp ../audiosource/wav/*.c ../backend/miniaudio/*.cpp ../core/*.cpp

    lib ./*.obj /out:soloud.lib
    
    copy ".\soloud.lib" "..\..\..\..\libs\soloud.lib"
)

if %ENV% == GNU (
    @REM g++ 
)

:skip_SOLOUD_build
popd
@REM ---------End of Soloud Build ----------------


pushd "./build"

if %ENV% == GNU (
    @REM Compiling with g++
    g++ -g -std=c++14 ..\main.cpp ..\app.cpp -o app -I..\..\include -L..\..\libs -l:libglfw3.a -l:libfreetype.a -lOpengl32
)

if %ENV% == MSVC (
    @REM Compiling with MSVC
    cl ..\platform.cpp ..\glad.c -Z7  -EHsc -I../vendor/include -link -LIBPATH:..\vendor\libs^
    soloud.lib freetype.lib libglfw3.a Gdi32.lib User32.lib Shell32.lib Opengl32.lib
)

if %ENV% == WEB (
    @REM Compiling with EM++
    robocopy ../vendor ./vendor  /E /NJH /NJS /NFL /NDL

    em++ ..\main.cpp ..\app.cpp -I..\..\include -sFULL_ES3 -sUSE_GLFW=3 -lglfw -lGLESv2 -o eq.html^
     --preload-file ./vendor/web_v_shader.glsl --preload-file ./vendor/web_f_shader.glsl^
     --preload-file ./vendor/checker_board.png --preload-file ./vendor/thin/stall.obj^
     --preload-file ./vendor/thin/stallTexture.png --preload-file ./vendor/thin/dragon.obj^
     --preload-file ./vendor/white.png --preload-file ./vendor/cube.obj
     
    popd
)

popd

