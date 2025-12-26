#pragma once
#include <map>
#include "utils.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "text.h"
#include "shader.h"
#include "input.h"

struct Render_Square{
    HMM_Vec2 dimensions;
    HMM_Vec4 position;
    RGBA color;
    RGBA border_clr;
    unsigned int texture_id;
    unsigned int textureSlot;
    unsigned int vao;
};

class TextureManager{
    std::map<const char *, unsigned int> m_textures = {};
    std::map<unsigned int, unsigned int> m_texture_slots = {};

public:
    TextureManager();
    unsigned int GetTexture(const char *filepath);
    unsigned int GetTextureSlot(const char *filepath);
    unsigned int AddTexture(const char *filepath);

};


// TODO: shader program should be in here
class UIRenderer{
    TextureManager *m_textureManager;
    HMM_Mat4 m_projection_ortho;
    HMM_Mat4 m_viewMatrix;

    FT_Library m_library;
    FT_Face *m_faces; // Can hold multiple typefaces and styles..."Times New Roman Italic for ex"
    unsigned int m_facesIndex;
    unsigned int m_facesCount;
    unsigned int m_textVao;
    unsigned int m_textVbo;
    unsigned int m_basic_2d_vao;
    unsigned int m_basic_2d_vbo;
    unsigned int m_basic_2d_vao_sdf;
    unsigned int m_basic_2d_vbo_sdf;

    CharacterTable *m_cts; // [""] ... character tables
    unsigned int *m_ctsCount;

    Shader m_textShader; 
    Shader m_basic_2d_shader = {};
    Shader m_basic_2d_shader_sdf = {};
    unsigned int m_u_projection_matrix;
    unsigned int m_u_text_color;
    int m_window_width;
    int m_window_height;
public:
    UIRenderer(TextureManager *textureManager);

    void Render();
    
    CharacterTable * GetCharacterTable(){
        return m_cts;
    }

    HMM_Vec2 WorldToScreen2D(HMM_Vec2 worldPos, HMM_Mat4 viewMat, HMM_Mat4 projMat, HMM_Vec2 screenDim);

    void AddFontFace(const char* ttfFilePath, int fontHeight);
    void UpdateTextRendererDimensions(int window_width, int window_height);
    void SetupTextRenderer(int window_width, int window_height);
    void RenderText(Text text, float scale, HMM_Vec3 color, HMM_Vec2 position);
    void DrawText(std::string string, float scale, HMM_Vec2 position, HMM_Vec3 color);
    void DrawText(Text text, float scale, HMM_Vec2 position, HMM_Vec3 color);
    void CleanTextRenderer();

    // UI
    void DrawRect(HMM_Vec2 pos, float width, float height, RGBA color, const char *texturePath);
    void DrawRectSDF(HMM_Vec3 pos, float width, float height, RGBA clr, RGBA borderClr, const char *texturePath);

};