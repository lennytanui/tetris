#ifndef TEXT_H
#include "str.c"
#include "HandmadeMath.h"
#include "shader.cpp"
#include <string>
#include "text.h"

inline int GetRowWidth(Text *text, int row){
    int result = 0;

    // Left 20 bits
    result = text->rowWidthCount[row] >> 12;

    return result;
}

inline int GetRowCharCount(Text *text, int row){
    int result = 0;

    // Right 12 bits
    result = (text->rowWidthCount[row] & (~(0xFFFFF << 12)));

    return result;
}

inline int IncreaseRowWidthCharCount(Text *text, int row, int amount, int charCount){
    int rowWidth = GetRowWidth(text, row);
    rowWidth += amount;
    rowWidth = rowWidth << 12;

    int rowCharCount = GetRowCharCount(text, row);
    rowCharCount += charCount;

    text->rowWidthCount[row] = (rowWidth | rowCharCount);
    // text->rowWidthCount[row] = 1233456;
    
    // returns new row width;
    return rowWidth;
}

inline int DecreaseRowWidthCharCount(Text *text, int row, int amount, int charCount){
    int rowWidth = GetRowWidth(text, row);
    rowWidth -= amount;
    rowWidth = rowWidth << 12;

    int rowCharCount = GetRowCharCount(text, row);
    rowCharCount -= charCount;
    
    text->rowWidthCount[row] = (rowWidth | rowCharCount);

    // returns new row width;
    return rowWidth;
}

#define TEXT_H
#endif