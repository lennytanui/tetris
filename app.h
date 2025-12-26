#pragma once
#include "HandmadeMath.h"
#include "math.h"
#include "renderer.h"
#include <cstdlib>
#include <ctime>
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

struct RenderBatch{
    unsigned int count;
    Render_Square *render_squares[10000];
};

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

    RenderBatch batches[5];

    HMM_Vec2 leaderBoardScreenPos_BottomLeft;
    HMM_Vec2 leaderBoardScreenPos_TopRight;
    std::string playerName;

    unsigned int render_squares_count;
};

#if GLFW_PLATFORM_EMSCRIPTEN
void RunJS(const char* function);
#endif

float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

std::string NumToString(float val){
    std::string result = "";
    char strBuffer[32] = {0};
    for(int i = 0; i < 32; i++){
        strBuffer[i] = '\0';
    }

    if(val != std::floor(val)){
        std::snprintf(strBuffer, sizeof(strBuffer), "%.04f", val);
    }else{
        std::snprintf(strBuffer, sizeof(strBuffer), "%i", (int)val);
    }

    strBuffer[31] = '\0';
    if(val != std::floor(val)){
        for(int i = 30; i > 0; i--){
            if(strBuffer[i] == '.'){
                break;
            }

            if(strBuffer[i] == '\0'){
                continue;
            }

            if(strBuffer[i] != '0'){
                break;
            }

            if(strBuffer[i] == '0'){
                strBuffer[i] = '\0';
            }          
        }
    }

    result = std::string(strBuffer);
    return result;
}

void SetCursorPosition(float xpos, float ypos);

// void GetLeaderBoard(void *leaderboard);
// extern "C" EMSCRIPTEN_KEEPALIVE void GetLeaderBoard_(void *leaderboard);

void ReceiveCharacter(unsigned int codePoint);
void NotesKeyPress(int key, int action);