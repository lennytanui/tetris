#pragma once
#include "str.c"

struct Character{
    unsigned int textureID;
    unsigned int advance;
    HMM_Vec2 size; // size of glyph
    HMM_Vec2 bearing; // offset from origin to top left of glyph
};

struct CharacterTable{  
    Character *characters;
    unsigned int index;
    unsigned int glyphsCount; 
};

struct Text{
    // Max buffer size for each string is 1024
    std::string string;

    int cursorRow;
    int cursorColumn;
    float left;
    float top;
    bool oneLine;

    int rowsCount; 
    unsigned int rowWidthCount[512];
    unsigned int columnWidthCount[512];
};