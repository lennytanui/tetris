#pragma once
// #include <glm/glm.hpp>

static int window_width = 780;
static int window_height = 490;
GLFWwindow *window = 0; // this is bad?

#define CAMERA_NORMAL_POSITION {0.0f, 0.0f, 1.0f}
#define MAX_TEX 16

void move_tetromino(int key);

struct RGBA{
    float r;
    float g;
    float b;
    float a;
};

struct v2{
    union {
        struct{
            float x;
            float y;
        };
        
        float e[2];
    };
};

struct v3{
    union {
        struct{
            float x;
            float y;
            float z;
        };
        
        float e[3];
    };
};

v2 operator+(v2 a, v2 b){
    v2 result = {};

    result.x = a.x + b.x;
    result.y = a.y + b.y;

    return result;
}


v2 operator*(v2 a, float b){
    v2 result = {};

    result.x = a.x * b;
    result.y = a.y * b;

    return result;
}


v3 operator+(v3 a, v3 b){
    v3 result = {};

    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    
    return result;
}


v3 operator*(v3 a, float b){
    v3 result = {};

    result.x = a.x * b;
    result.y = a.y * b;
    result.z = a.z * b;

    return result;
}

struct Render_Square{
    v2 dimensions;
    v2 position;
    RGBA color;
    RGBA border_clr;
    unsigned int vao;
};

struct AppState{
    bool initialized;
    int basic_sp;
    int tex_count;
    unsigned int basic_vao;
    // unsigned int basic_vbo;
    unsigned int basic_ebo;
    unsigned int textures[MAX_TEX];
    v3 cam_pos;
    v3 cam_targ;
    v3 cam_dir;

    int window_width;
    int window_height;

    Render_Square* render_squares[10000];
    unsigned int render_squares_count;
};

void SetCursorPosition(float xpos, float ypos);