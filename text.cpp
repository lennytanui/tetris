#ifndef TEXT_H

#include "str.c"
#include "HandmadeMath.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "shader.cpp"
#include <string>

//#define DEFAULT_TEXT_PIXEL_HEIGHT 48

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

// TODO: shader program should be in here
struct TextRendererManager{
    FT_Library library;
    FT_Face *faces; // Can hold multiple typefaces and styles..."Times New Roman Italic for ex"
    unsigned int facesIndex;
    unsigned int facesCount;
    unsigned int vao;
    unsigned int vbo;

    CharacterTable *cts; // [""] ... character tables
    unsigned int *ctsCount;

    Shader2 shader; 
    unsigned int u_projection_matrix;
    unsigned int u_text_color;
    HMM_Mat4 projection_ortho;
    int window_width;
    int window_height;
};

struct Text{
    // Max buffer size for each string is 1024
    String string;

    int cursorRow;
    int cursorColumn;
    float left;
    float top;
    bool oneLine;

    int rowsCount; 
    unsigned int rowWidthCount[512];
    unsigned int columnWidthCount[512];
};

#if 0
struct TextEditorString{
    // Max Characters per string is (8192 bytes) .. about 8kb
    char *val;
    int gapLeft;
    int gapRight;

    TextEditorString *prevString;
    TextEditorString *nextString;
};

struct TextEditorStringManager{
    TextEditorString tes;
}
#endif

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

void AddFontFace(TextRendererManager *trm, const char* ttfFilePath, int fontHeight){
    FT_Face *tempFace = trm->faces;
    FT_Face *newFace = 0;

    if(trm->facesIndex < trm->facesCount){
        int error = FT_New_Face(trm->library, ttfFilePath, 0, &trm->faces[0]);

        if(error == FT_Err_Unknown_File_Format){
            printf("ERROR: failed to initialize freetype -- %s -- Font Face. FORMAT NOT SUPPORTED.\n", ttfFilePath);
        } else if (error){
            // Could mean file could not be opened or read, or it is broken.
            printf("ERROR: failed to initialize freetype -- %s -- Font Face.\n", ttfFilePath);
        }else{
            newFace = &trm->faces[0];
        }
    }else{
        // Add more space to the Font Faces Array to make room for more Font Faces
        int x = 0;
        trm->facesCount += 2;
        trm->faces = (FT_Face *)calloc(trm->facesCount,  sizeof(FT_Face));
        
        for(int i = 0; i < trm->facesCount - 2; i++){
            trm->faces[i] = tempFace[i];
        }
        
        int error = FT_New_Face(trm->library, ttfFilePath, 0, &trm->faces[trm->facesIndex++]);

        if(error){
            printf("ERROR: failed to initialize freetype -- %s -- Font Face\n", ttfFilePath);
        }

        newFace = &trm->faces[trm->facesIndex - 1];
    }
    // FT_Set_Pixel_Sizes(*newFace, 0, fontHeight);
    // 16 pt for 72x72dpi device
    int error = FT_Set_Char_Size( 
        *newFace,    /* handle to face object         */
        0,       /* char_width in 1/64 of points  */
        16*64,   /* char_height in 1/64 of points */
        300,     /* horizontal device resolution  */
        300);   /* vertical device resolution    */

    if(error){
        printf("Error setting pixel size.\n");
    }
}

// Note: called at app_start and during resizing. MUST BE CALLED BY 'SetupTextRenderer'
void UpdateTextRendererDimensions(TextRendererManager *trm, int window_width, int window_height){
    trm->projection_ortho = HMM_Orthographic_RH_ZO(0.0f, window_width, 0.0f, window_height, 0.0f, 1.0f);
    trm->window_width = window_width;
    trm->window_height = window_height;

    // update projection matrix uniform
    glUseProgram(trm->shader.program);
    trm->u_projection_matrix = GetUniformLocation(&trm->shader, "projection_matrix");

    SetUniformValue(trm->u_projection_matrix, trm->projection_ortho);
    
    glUseProgram(0);
}

void SetupTextRenderer(TextRendererManager *trm, int window_width, int window_height){
    int error = FT_Init_FreeType( &trm->library );
    if(error != FT_Err_Ok){
        printf("ERROR: failed to initialize freetype\n");
    }

    trm->facesIndex = 0;
    trm->facesCount = 4;
    trm->faces = (FT_Face *)calloc(1, sizeof(FT_Face));
    // default font
    AddFontFace(trm, "assets\\Passion_One\\PassionOne-Regular.ttf", 48);
    // AddFontFace(trm, "static\\Antonio-Bold.ttf");
    
    trm->ctsCount = (unsigned int *)(calloc(1, sizeof(unsigned int)));
    trm->ctsCount[0] = 1;

    trm->cts = (CharacterTable *)calloc(trm->ctsCount[0], sizeof(CharacterTable));
    trm->cts[0].glyphsCount = trm->faces[0]->num_glyphs;
    trm->cts[0].characters = (Character *)calloc(trm->cts[0].glyphsCount, sizeof(Character));

    // opengl stuff
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for(int c = 0; c < trm->cts[0].glyphsCount; c++){
        // load character glyph 
        if (FT_Load_Char(trm->faces[0], c, FT_LOAD_RENDER)) {
            std::cout << "ERROR: Failed to load Glyph" << std::endl;
            continue;
        }

        // generate texture
        unsigned int texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            trm->faces[0]->glyph->bitmap.width,
            trm->faces[0]->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            trm->faces[0]->glyph->bitmap.buffer
        );

        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character *character = &trm->cts[0].characters[c];
        trm->cts[0].index++;
        character->textureID = texture;
        character->size = {(float)trm->faces[0]->glyph->bitmap.width, (float)trm->faces[0]->glyph->bitmap.rows};
        character->bearing = {(float)trm->faces[0]->glyph->bitmap_left, (float)trm->faces[0]->glyph->bitmap_top};
        character->advance = (float)trm->faces[0]->glyph->advance.x;
        int x = 0;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    // load text shader
    trm->shader.program = LoadShaders("assets/text_basic_vs.glsl", "assets/text_basic_fs.glsl");
    
    UpdateTextRendererDimensions(trm, window_width, window_height);

    glUseProgram(trm->shader.program);

    BindLocation(&trm->shader, 0, "position");
    trm->u_text_color = GetUniformLocation(&trm->shader, "text_color");

    SetUniformValue(trm->u_text_color, HMM_Vec3{255.0f, 0.0f, 0.0f});
    
    glGenVertexArrays(1, &trm->vao);
    glGenBuffers(1, &trm->vbo);
    glBindVertexArray(trm->vao);
    glBindBuffer(GL_ARRAY_BUFFER, trm->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);      
    glUseProgram(0);

}

void RenderText(TextRendererManager *trm, Text text, float scale, HMM_Vec3 color, HMM_Vec2 position){
    String string = text.string;

    if(!string.val || string.length <= 0)
        return;
    
    HMM_Vec2 originalPosition = position;
    
    glUseProgram(trm->shader.program);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glDisable(GL_DEPTH_TEST);

    SetUniformValue(trm->u_text_color, color);

    glBindVertexArray(trm->vao);
    glActiveTexture(GL_TEXTURE0);

    // iterate through all characters
    std::string::const_iterator c;
    for (int i = 0; i < string.length; i++){
        char c = string.val[i];

        Character ch = trm->cts[0].characters[c];

        float xpos = position.X + ch.bearing.X * scale;
        float ypos = position.Y - (ch.size.Y - ch.bearing.Y) * scale;

        float w = ch.size.X * scale;
        float h = ch.size.Y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, trm->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        position.X += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        
        // new line
        if(c == '\n'){
            position.Y -= 48;
            position.X = originalPosition.X;
        }
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(0);

}


void RenderTextEditor(TextRendererManager *trm, Text text, float scale, HMM_Vec3 color, HMM_Vec2 position){
    String string = text.string;

    if(!string.val || string.length <= 0)
        return;
    
    HMM_Vec2 originalPosition = position;
    
    glUseProgram(trm->shader.program);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glDisable(GL_DEPTH_TEST);

    SetUniformValue(trm->u_text_color, color);

    glBindVertexArray(trm->vao);
    glActiveTexture(GL_TEXTURE0);

    // iterate through all characters
    std::string::const_iterator c;
    for (int i = 0; i < string.length; i++){
        char c = string.val[i];

        Character ch = trm->cts[0].characters[c];

        float xpos = position.X + ch.bearing.X * scale;
        float ypos = position.Y - (ch.size.Y - ch.bearing.Y) * scale;

        float w = ch.size.X * scale;
        float h = ch.size.Y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, trm->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        position.X += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        
        // new line
        if(c == '\n'){
            position.Y -= 48;
            position.X = originalPosition.X;
        }
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(0);

}

void DrawText(TextRendererManager *trm, String string, float scale, HMM_Vec2 position, HMM_Vec3 color){
    Text text = {0};
    text.string = string;
    RenderText(trm, text, scale, color, position);
}

void DrawText(TextRendererManager *trm, Text text, float scale, HMM_Vec2 position, HMM_Vec3 color){
    RenderText(trm, text, scale, color, position);
}

void CleanTextRenderer(TextRendererManager *trm){
    free(trm->faces);
    FT_Done_FreeType(trm->library);
    
    for(int i = 0; i < 1; i++){
        free(trm->cts[i].characters);
    }
    free(trm->cts);
    free(trm->ctsCount);
}

#define TEXT_H
#endif