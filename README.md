
- Music and Sounds Attributions

Future Technology by MaxKoMusic | https://maxkomusic.com/
Music promoted by https://www.chosic.com/free-music/all/
Creative Commons Attribution-ShareAlike 3.0 Unported (CC BY-SA 3.0)
https://creativecommons.org/licenses/by-sa/3.0/

DEPENDENCIES
-> GLFW
-> soLoud
-> FreeType

How to build using Command Line (Visual Studio is Required)

1. Download the Precompiled Binaries for  GLFW from `here`.
    -> Copy the content of GLFW include directory, into a folder in the Tetris vendor directory called `GLFW`
    -> Copy `glfw.lib` into the libs directory of Tetris
    -> Copy `glfw.dll` into the vendor directory of Tetris
2. For soloud, click the link `here`, and follow the intructions to build for Visual Studio
3. cd into `Tetris Directory`
4. run the following command `cd .\vendor && ..\build\platform.exe && cd ..`
