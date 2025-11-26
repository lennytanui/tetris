#pragma once
#include "HandmadeMath.h"
#include "math.h"
#include "renderer.h"

static int global_window_width = 800;
static int global_window_height = 800;

static int global_frame_buffer_width = 800;
static int global_frame_buffer_height = 800;

float global_ortho_width = 1000.0f;
float global_ortho_height = 1000.0f;
TextureManager *global_textureManager;
UIRenderer *global_UIRenderer;


GLFWwindow *window = 0; // this is bad?

#define CAMERA_NORMAL_POSITION {0.0f, 0.0f, 1.0f}
#define MAX_TEX 16

void move_tetromino(int key);

struct AppState{
    bool initialized;
    int basic_sp;
    int tex_count;
    HMM_Mat4 view;
    HMM_Mat4 proj;

    unsigned int basic_vao;
    unsigned int basic_ebo;
    HMM_Vec3 cam_pos;
    HMM_Vec3 cam_targ;
    HMM_Vec3 cam_dir;

    int window_width;
    int window_height;

    Render_Square* render_squares[10000];
    unsigned int render_squares_count;
};

void RunJS(const char* function);

float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

void SetCursorPosition(float xpos, float ypos);

// void GetLeaderBoard(void *leaderboard);
// extern "C" EMSCRIPTEN_KEEPALIVE void GetLeaderBoard_(void *leaderboard);

void ReceiveCharacter(unsigned int codePoint);
void NotesKeyPress(int key, int action);