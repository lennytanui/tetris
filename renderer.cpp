#pragma once
#include "app.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "HandmadeMath.h"

// #define PROJ_RIGHT 1920.0f
// #define PROJ_TOP 1080.0f

struct ParticleData{
    v2 position;
    v2 velocity;
    v2 acceleration;
    v2 size;
    RGBA color;
};

struct Particle{
    // Note (Lenny): This will have to be removed. Replaced by ParticleData struct
    v2 position;
    v2 velocity;
    v2 acceleration;
    v2 size;
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

void EmitParticles(ParticleManager *pm, float dt){
    for(int i = 0; i < pm->count; i++){
        Particle *particle = &pm->particles[i];
        particle->velocity += (particle->acceleration * dt);
        particle->position += particle->velocity;
        particle->acceleration += v2{0.1f, 0.1f} * dt;
        // particle->position.x += 30.0f * dt;
        particle->color.a -= dt  * 125.0f;
    }

    pm->life_time -= dt;
    if(pm->life_time <= 0){
        pm->ready = false;
    }
}

void ReadFile(char *file_name, char *buffer) {
    char *result = 0;

    buffer = result;
}

// position is v4 because of depth on .w
Render_Square *create_render_square(AppState *app_state, v4 position, v2 dimensions, RGBA color, RGBA border_clr){

    Render_Square *render_square = (Render_Square *)malloc(sizeof(Render_Square));
    render_square->color = color;
    render_square->border_clr = border_clr;
    render_square->position = position;
    render_square->dimensions = dimensions;
    
    app_state->render_squares[app_state->render_squares_count++] = render_square;

    return render_square;
}

void draw_render_squares(AppState *app_state){

}

// Renders the rectangles from the "render_squares"
void RenderRectangles(AppState *app_state, float dt) {
    
    HMM_Vec3 cam_pos = {app_state->cam_pos.x, app_state->cam_pos.y, app_state->cam_pos.z}; 

    if(!app_state->initialized){

        glGenVertexArrays(1, &app_state->basic_vao);
        glBindVertexArray(app_state->basic_vao);
        
        unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        glGenBuffers(1, &app_state->basic_ebo);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 
            app_state->basic_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), 
            indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        const char *basic_vs_src = "#version 330 core \n"\
                        "layout (location = 0) in vec4 aPos; \n"\
                        "layout (location = 1) in vec4 aColor; \n"\
                        "layout (location = 2) in vec2 aTexCoord; \n"\
                        "layout (location = 3) in vec4 a_border_clr; \n"\

                        "uniform mat4 u_view; \n" \
                        "uniform mat4 u_projection; \n"

                        "varying vec2 pos_; \n"\
                        "out vec2 texCoord; \n"\
                        "out vec4 color; \n"\
                        "out vec4 border_clr; \n"\
                        "void main() { \n"\
                            "pos_ = aTexCoord; \n;"\
                            "gl_Position = u_projection * u_view * vec4(aPos.x, aPos.y, aPos.z, aPos.w); \n"\
                            "texCoord = aTexCoord; \n"\
                            "color = aColor;\n" \
                            "border_clr = a_border_clr; \n"\
                        "} \n";

        const char *basic_fs_src = "#version 330 core \n"\
                            "in vec2 texCoord; \n"\
                            "in vec4 color; \n"\
                            "in vec4 border_clr; \n"\
                            "in vec2 pos_; \n"\

                            "uniform sampler2D testTexture; \n"\

                            "out vec4 FragColor; \n"\
                            "void main(){ \n"\
                                "vec4 clr = color; \n"\

                                "// if(gl_FragCoord.x > 205.0f){ \n" \
                                "if (pos_.x <= 0.1f || pos_.x >= 0.9f){ \n"\
                                    "clr = border_clr; \n"\
                                "} else if (pos_.y <= 0.1f || pos_.y >= 0.9f) { \n"\
                                    "clr = border_clr; \n"\
                                "} \n"\

                                "FragColor = clr * " \
                                "texture(testTexture, texCoord); \n"\
                            "} \n";
        
        
        unsigned int basic_vs = 0;
        basic_vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(basic_vs, 1, &basic_vs_src, NULL);
        glCompileShader(basic_vs);

        int success = 0;
        char info_log[512];
        glGetShaderiv(basic_vs, GL_COMPILE_STATUS, &success);

        if(!success){
            glGetShaderInfoLog(basic_vs, 512, NULL, info_log);
            printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s \n", info_log);
        }

        unsigned int basic_fs;
        basic_fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(basic_fs, 1, &basic_fs_src, NULL);
        glCompileShader(basic_fs);

        glGetShaderiv(basic_fs, GL_COMPILE_STATUS, &success);

        if(!success){
            glGetShaderInfoLog(basic_fs, 512, NULL, info_log);
            printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n %s \n", info_log);
        }
        
        app_state->basic_sp = glCreateProgram();
        glAttachShader(app_state->basic_sp, basic_vs);
        glAttachShader(app_state->basic_sp, basic_fs);
        glLinkProgram(app_state->basic_sp);

        glDeleteShader(basic_vs);
        glDeleteShader(basic_fs);

        int width, height, nr_channels = 0;
        unsigned char *data = stbi_load("assets/white_texture.jpg",
            &width, &height, &nr_channels, 0);

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, 
            &app_state->textures[app_state->tex_count++]);
        glBindTexture(GL_TEXTURE_2D, 
            app_state->textures[(app_state->tex_count - 1)]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
            GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (data){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
                width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            printf("ERROR::LOADING TEXTURE");
        }

        stbi_image_free(data);

        app_state->proj = HMM_Orthographic_LH_NO(0.0f, global_window_width, 0.0f, global_window_height, 0.0f, 10.0f);

        app_state->initialized = true;
    }

    { // TODO: camera movement

    }
    
    glUseProgram(app_state->basic_sp);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    // glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, 
            app_state->textures[(app_state->tex_count - 1)]);

    HMM_Vec3 cam_front = {0.0f, 0.0f, -1.0f};
    HMM_Vec3 cam_up = {0.0f, 1.0f, 0.0f};
    app_state->view = HMM_LookAt_LH(cam_front + HMM_Vec3{cam_pos.X, cam_pos.Y, 0.0f},{cam_pos.X, cam_pos.Y, 20.0f}, cam_up);

    unsigned int projection_loc = glGetUniformLocation(app_state->basic_sp, "u_projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, &app_state->proj[0][0]);
    
    unsigned int view_loc = glGetUniformLocation(app_state->basic_sp, "u_view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, &app_state->view[0][0]);

    for(int i = 0; i < app_state->render_squares_count; i++){
        Render_Square *render_square = app_state->render_squares[i];
        v4 pos = render_square->position;
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
        
        v2 size = render_square->dimensions;

        float vertices[] = {
             // positions                                 // color                    // texture coords     // border color
             pos.x + size.x,  pos.y + size.y,   pos.z, pos.w,     clr.r, clr.g, clr.b, clr.a,     1.0f, 1.0f,       b_clr.r, b_clr.g, b_clr.b, b_clr.a,  // top right
             pos.x + size.x,  pos.y,            pos.z, pos.w,     clr.r, clr.g, clr.b, clr.a,     1.0f, 0.0f,       b_clr.r, b_clr.g, b_clr.b, b_clr.a,  // bottom right
             pos.x,           pos.y,            pos.z, pos.w,     clr.r, clr.g, clr.b, clr.a,     0.0f, 0.0f,       b_clr.r, b_clr.g, b_clr.b, b_clr.a,  // bottom left
             pos.x,           pos.y + size.y,   pos.z, pos.w,     clr.r, clr.g, clr.b, clr.a,     0.0f, 1.0f,       b_clr.r, b_clr.g, b_clr.b, b_clr.a  // top left
        };
    
        glBindVertexArray(app_state->basic_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 
            app_state->basic_ebo);
        
        unsigned int b = 0;
        glGenBuffers(1, &b);
        glBindBuffer(GL_ARRAY_BUFFER, b);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), 
            vertices, GL_STATIC_DRAW);        

        // describe the data in the buffer
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 14 * sizeof(float), 
            (void*)0); // aPos
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 14 * sizeof(float), 
            (void*)(4 * sizeof(float))); // aColor
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), 
            (void*)(8 * sizeof(float))); // aTexCoord
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 14 * sizeof(float), 
            (void*)(10 * sizeof(float))); // a_border_clr

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glDeleteBuffers(1, &b);
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
    glUseProgram(0);

    for(int i = 0; i < app_state->render_squares_count; i++){
        Render_Square *render_square = app_state->render_squares[i];
        free(render_square);
        render_square = 0;
    }
    app_state->render_squares_count = 0;
}

void AppQuit(AppState *app_state){
    
}