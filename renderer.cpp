#pragma once
#include "app.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "HandmadeMath.h"
#include "shader.cpp"
#include "renderer.h"

struct ParticleData{
    HMM_Vec2 position;
    HMM_Vec2 velocity;
    HMM_Vec2 acceleration;
    HMM_Vec2 size;
    RGBA color;
};

struct Particle{
    // Note (Lenny): This will have to be removed. Replaced by ParticleData struct
    HMM_Vec2 position;
    HMM_Vec2 velocity;
    HMM_Vec2 acceleration;
    HMM_Vec2 size;
    RGBA color;

    ParticleData current;
    ParticleData original;
};

struct ParticleManager{
    float life_time;
    float current_time;
    bool ready;
    int index;
    int count;
    Particle particles[1000];
};

TextureManager::TextureManager(){

}

unsigned int TextureManager::GetTexture(const char *filepath){
    unsigned int result = 0;
    result = m_textures[filepath];
    return result;
}

// TODO: Heavily Optimize this
unsigned int TextureManager::GetTextureSlot(const char *filepath){
    unsigned int result = 0;
    result = m_texture_slots[m_textures[filepath]];
    return result;
}
    
unsigned int TextureManager::AddTexture(const char *filepath){
    unsigned int result = 0;
    unsigned int textureSlotsCount = m_texture_slots.size();
    int width, height, nr_channels = 0;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load(filepath, &width, &height, 
        &nr_channels, 0);
    
    printf("Requested Texture Slot : %i\n", textureSlotsCount);
    glActiveTexture(GL_TEXTURE0 + textureSlotsCount);
    glGenTextures(1, &result);
    glBindTexture(GL_TEXTURE_2D, result);
    
    m_textures.insert({filepath, result});
    m_texture_slots.insert({result, textureSlotsCount});
       
#if GLFW_PLATFORM_EMSCRIPTEN
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
        GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
        GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif

    if (data){
#if GLFW_PLATFORM_EMSCRIPTEN
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
            width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
#else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
            width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
#endif
        glGenerateMipmap(GL_TEXTURE_2D);
 
} else {
        printf("ERROR::LOADING TEXTURE");
    }

    stbi_image_free(data);

    return result;
}

void UIRenderer::Render(){

}

void UIRenderer::AddFontFace(const char *ttfFilePath, int fontHeight){
    FT_Face *tempFace = m_faces;
    FT_Face *newFace = 0;

    if(m_facesIndex < m_facesCount){
        int error = FT_New_Face(m_library, ttfFilePath, 0, &m_faces[0]);

        if(error == FT_Err_Unknown_File_Format){
            printf("ERROR: failed to initialize freetype -- %s -- Font Face. FORMAT NOT SUPPORTED.\n", ttfFilePath);
        } else if (error){
            // Could mean file could not be opened or read, or it is broken.
            printf("ERROR: failed to initialize freetype -- %s -- Font Face. with Error %i\n", ttfFilePath, error);
        }else{
            newFace = &m_faces[0];
        }
    }else{
        // Add more space to the Font Faces Array to make room for more Font Faces
        int x = 0;
        m_facesCount += 2;
        m_faces = (FT_Face *)calloc(m_facesCount,  sizeof(FT_Face));
        
        for(int i = 0; i < m_facesCount - 2; i++){
            m_faces[i] = tempFace[i];
        }
        
        int error = FT_New_Face(m_library, ttfFilePath, 0, &m_faces[m_facesIndex++]);

        if(error){
            printf("ERROR: failed to initialize freetype -- %s -- Font Face\n", ttfFilePath);
        }

        newFace = &m_faces[m_facesIndex - 1];
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
void UIRenderer::UpdateTextRendererDimensions(int window_width, int window_height){
    m_projection_ortho = HMM_Orthographic_RH_ZO(0.0f, window_width, 0.0f, window_height, 0.0f, 1.0f);
    m_window_width = window_width;
    m_window_height = window_height;

    // update projection matrix uniform
    glUseProgram(m_shader.program);
    m_u_projection_matrix = GetUniformLocation(&m_shader, "projection_matrix");

    SetUniformValue(m_u_projection_matrix, m_projection_ortho);
    
    glUseProgram(0);
}


void UIRenderer::SetupTextRenderer(int window_width, int window_height){
    int error = FT_Init_FreeType( &m_library );
    if(error != FT_Err_Ok){
        printf("ERROR: failed to initialize freetype\n");
    }

    m_facesIndex = 0;
    m_facesCount = 4;
    m_faces = (FT_Face *)calloc(1, sizeof(FT_Face));
    // default font
    AddFontFace("assets/Passion_One/PassionOne-Regular.ttf", 48);
   
    m_ctsCount = (unsigned int *)(calloc(1, sizeof(unsigned int)));
    m_ctsCount[0] = 1;

    m_cts = (CharacterTable *)calloc(m_ctsCount[0], sizeof(CharacterTable));
    m_cts[0].glyphsCount = m_faces[0]->num_glyphs;
    m_cts[0].characters = (Character *)calloc(m_cts[0].glyphsCount, sizeof(Character));

    // opengl stuff
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for(int c = 0; c < m_cts[0].glyphsCount; c++){
        // load character glyph 
        if (FT_Load_Char(m_faces[0], c, FT_LOAD_RENDER)) {
            std::cout << "ERROR: Failed to load Glyph" << std::endl;
            continue;
        }

        // generate texture
        unsigned int texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

#if GLFW_PLATFORM_EMSCRIPTEN
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_R8,
            m_faces[0]->glyph->bitmap.width,
            m_faces[0]->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            m_faces[0]->glyph->bitmap.buffer
        );
        
#else
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            m_faces[0]->glyph->bitmap.width,
            m_faces[0]->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            m_faces[0]->glyph->bitmap.buffer
        );

#endif
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      
        Character *character = &m_cts[0].characters[c];
        m_cts[0].index++;
        character->textureID = texture;
        character->size = {(float)m_faces[0]->glyph->bitmap.width, (float)m_faces[0]->glyph->bitmap.rows};
        character->bearing = {(float)m_faces[0]->glyph->bitmap_left, (float)m_faces[0]->glyph->bitmap_top};
        character->advance = (float)m_faces[0]->glyph->advance.x;
        int x = 0;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    // load text shader
    m_shader.program = LoadShaders("assets/text_basic_vs_web.glsl", "assets/text_basic_fs_web.glsl");
    
    UpdateTextRendererDimensions(window_width, window_height);

    glUseProgram(m_shader.program);

    BindLocation(&m_shader, 0, "position");
    m_u_text_color = GetUniformLocation(&m_shader, "text_color");

    SetUniformValue(m_u_text_color, HMM_Vec3{255.0f, 0.0f, 0.0f});
    
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);      
    glUseProgram(0);

}

void UIRenderer::RenderText(Text text, float scale, HMM_Vec3 color, HMM_Vec2 position){
    String string = text.string;

    if(!string.val || string.length <= 0)
        return;
    
    HMM_Vec2 originalPosition = position;
    
    glUseProgram(m_shader.program);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glDisable(GL_DEPTH_TEST);

    SetUniformValue(m_u_text_color, color);

    glBindVertexArray(m_vao);
    glActiveTexture(GL_TEXTURE0);

    // iterate through all characters
    std::string::const_iterator c;
    for (int i = 0; i < string.length; i++){
        char c = string.val[i];

        Character ch = m_cts[0].characters[c];

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
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
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


void UIRenderer::DrawText(String string, float scale, HMM_Vec2 position, HMM_Vec3 color){
    Text text = {0};
    text.string = string;
    RenderText(text, scale, color, position);
}

void UIRenderer::DrawText(Text text, float scale, HMM_Vec2 position, HMM_Vec3 color){
    RenderText(text, scale, color, position);
}

void UIRenderer::CleanTextRenderer(){
    free(m_faces);
    FT_Done_FreeType(m_library);
    
    for(int i = 0; i < 1; i++){
        free(m_cts[i].characters);
    }
    free(m_cts);
    free(m_ctsCount);
}

void ReadFile(char *file_name, char *buffer) {
    char *result = 0;

    buffer = result;
}

// position is HMM_Vec4 because of depth on .W
Render_Square *create_render_square(AppState *app_state, HMM_Vec4 position, HMM_Vec2 dimensions, RGBA color, RGBA border_clr, unsigned texSlot){

    Render_Square *render_square = (Render_Square *)malloc(sizeof(Render_Square));
    render_square->color = color;
    render_square->border_clr = border_clr;
    render_square->position = position;
    render_square->dimensions = dimensions;
    render_square->textureSlot = texSlot;
    
    app_state->render_squares[app_state->render_squares_count++] = render_square;

    return render_square;
}

void draw_render_squares(AppState *app_state){

}

// Renders the rectangles from the "render_squares"
void RenderRectangles(AppState *app_state, TextureManager *textureManager, float dt) {
    
    HMM_Vec3 cam_pos = {app_state->cam_pos.X, app_state->cam_pos.Y, app_state->cam_pos.Z}; 

    if(!app_state->initialized){

        glGenVertexArrays(1, &app_state->basic_vao);
        glBindVertexArray(app_state->basic_vao);
        
        unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3,  // second triangle

            4, 5, 7, // first triangle
            5, 6, 7  // second triangle
        };

        glGenBuffers(1, &app_state->basic_ebo);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 
            app_state->basic_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), 
            indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
     
#if GLFW_PLATFORM_EMSCRIPTEN
        app_state->basic_sp = LoadShaders("assets/basic_shader_vs_web.glsl", "assets/basic_shader_fs_web.glsl");
#else
        app_state->basic_sp = LoadShaders("assets/basic_shader_vs.glsl", "assets/basic_shader_fs.glsl");
#endif
        glUseProgram(app_state->basic_sp);

        unsigned int texture_0_loc = glGetUniformLocation(app_state->basic_sp, "texture_0");
        glUniform1i(texture_0_loc, 0);
        
        unsigned int texture_1_loc = glGetUniformLocation(app_state->basic_sp, "texture_1");
        glUniform1i(texture_1_loc, 1);
        
        glUseProgram(0);
        float ortho_height = 1000;
        ortho_height *= ((float)global_frame_buffer_height / (float)global_frame_buffer_width);

        app_state->proj = HMM_Orthographic_LH_NO(0.0f, 1000.0f, 0.0f, ortho_height, 0.0f, 10.0f);

        app_state->initialized = true;
    }

    { // TODO: camera movement

    }
    
    glUseProgram(app_state->basic_sp);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureManager->GetTexture("assets/white_texture.jpg"));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureManager->GetTexture("assets/w_button.jpg"));

    HMM_Vec3 cam_front = {0.0f, 0.0f, -1.0f};
    HMM_Vec3 cam_up = {0.0f, 1.0f, 0.0f};
    app_state->view = HMM_LookAt_LH(cam_front + HMM_Vec3{cam_pos.X, cam_pos.Y, 0.0f},{cam_pos.X, cam_pos.Y, 20.0f}, cam_up);

    unsigned int projection_loc = glGetUniformLocation(app_state->basic_sp, "u_projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, &app_state->proj[0][0]);
    
    unsigned int view_loc = glGetUniformLocation(app_state->basic_sp, "u_view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, &app_state->view[0][0]);

    const int bufferSize = 60;
    float *batchedVertices = (float *)malloc(bufferSize * app_state->render_squares_count * sizeof(float));
    int batchedIndex = 0;

    unsigned int *batchedIndices = (unsigned int *)malloc(6 * app_state->render_squares_count * sizeof(unsigned int));

    for(int i = 0; i < app_state->render_squares_count; i++){
        Render_Square *render_square = app_state->render_squares[i];
        HMM_Vec4 pos = render_square->position;
        float texSlot = (float)render_square->textureSlot;
        RGBA clr = render_square->color;
        clr.r /= 255.0f;
        clr.g /= 255.0f;
        clr.b /= 255.0f;
        clr.a /= 255.0f;
        
        RGBA b_clr = render_square->border_clr;
        b_clr.r /= 255.0f;
        b_clr.g /= 255.0f;
        b_clr.b /= 255.0f;
        b_clr.a /= 255.0f;
        
        HMM_Vec2 size = render_square->dimensions;

        float vertices[bufferSize] = {
             // positions                                 // color                    // texture coords     // border color
             pos.X + size.X,  pos.Y + size.Y,   pos.Z, pos.W,     clr.r, clr.g, clr.b, clr.a,     1.0f, 1.0f,       b_clr.r, b_clr.g, b_clr.b, b_clr.a, texSlot,  // top right
             pos.X + size.X,  pos.Y,            pos.Z, pos.W,     clr.r, clr.g, clr.b, clr.a,     1.0f, 0.0f,       b_clr.r, b_clr.g, b_clr.b, b_clr.a, texSlot,  // bottom right
             pos.X,           pos.Y,            pos.Z, pos.W,     clr.r, clr.g, clr.b, clr.a,     0.0f, 0.0f,       b_clr.r, b_clr.g, b_clr.b, b_clr.a, texSlot,  // bottom left
             pos.X,           pos.Y + size.Y,   pos.Z, pos.W,     clr.r, clr.g, clr.b, clr.a,     0.0f, 1.0f,       b_clr.r, b_clr.g, b_clr.b, b_clr.a, texSlot // top left
        };

        int indices[6] = {
            0 + batchedIndex * 4, 1 + batchedIndex * 4, 3 + batchedIndex * 4, // first triangle
            1 + batchedIndex * 4, 2 + batchedIndex * 4, 3 + batchedIndex * 4,  // second triangle
        };

        for(int j = 0; j < bufferSize; j++){
            batchedVertices[bufferSize * batchedIndex + j] = vertices[j];
        }

        for(int j = 0; j < 6; j++){
            batchedIndices[6 * batchedIndex + j] = indices[j];
        }

        batchedIndex += 1;   
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 
        app_state->basic_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * app_state->render_squares_count * sizeof(float), 
        batchedIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(app_state->basic_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 
        app_state->basic_ebo);
    
    unsigned int b = 0;
    glGenBuffers(1, &b);
    glBindBuffer(GL_ARRAY_BUFFER, b);
    glBufferData(GL_ARRAY_BUFFER, bufferSize * app_state->render_squares_count * sizeof(float), 
        batchedVertices, GL_STATIC_DRAW);        

    // describe the data in the buffer
    unsigned int stride = 15;
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride * sizeof(float), 
        (void*)0); // aPos
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride * sizeof(float), 
        (void*)(4 * sizeof(float))); // aColor
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), 
        (void*)(8 * sizeof(float))); // aTexCoord
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride * sizeof(float), 
        (void*)(10 * sizeof(float))); // a_border_clr
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, stride * sizeof(float), 
        (void*)(14 * sizeof(float))); // a_border_clr

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    glDrawElements(GL_TRIANGLES, 6 * app_state->render_squares_count, GL_UNSIGNED_INT, 0);
   
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &b);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
    glUseProgram(0);
    free(batchedVertices);

    for(int i = 0; i < app_state->render_squares_count; i++){
        Render_Square *render_square = app_state->render_squares[i];
        free(render_square);
        render_square = 0;
    }
    app_state->render_squares_count = 0;
}

void AppQuit(AppState *app_state){
    
}