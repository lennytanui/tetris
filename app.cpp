#include "tetris.cpp"
#include <string>
#include "renderer.h"

enum APPS {
    TETRIS
};

enum APPS currentApp = TETRIS;

#if GLFW_PLATFORM_EMSCRIPTEN    
void RunJS(const char *function){
    emscripten_run_script(function);
}
#endif

void app_start(AppState *app_state){
    srand(time(0));

    std::string val = NumToString(55.14);
    printf("Int to String %s\n", val.c_str());
    app_state->playerName = std::string("Player_");
    int randomVal = rand() % 10001;
    app_state->playerName += NumToString(randomVal);
    printf("App  Started..\n");

    app_state->window_width = global_window_width;
    app_state->window_height = global_window_height;
    
    app_state->ortho_width = 1000.0f;
    app_state->ortho_height = app_state->ortho_width;
    app_state->ortho_height *= ((float)global_frame_buffer_height / (float)global_frame_buffer_width);
  
    global_textureManager = new TextureManager();
    global_textureManager->AddTexture("assets/white_texture.jpg");
    global_textureManager->AddTexture("assets/w_button.jpg");
    global_textureManager->AddTexture("assets/a_button.jpg");
    global_textureManager->AddTexture("assets/s_button.jpg");
    global_textureManager->AddTexture("assets/d_button.jpg");

    global_UIRenderer = new UIRenderer(global_textureManager);

    if(currentApp == TETRIS){
        Tetris::start(app_state);    
    }
    
}

void app_update(AppState *app_state, float dt){
    app_state->window_width = global_window_width;
    app_state->window_height = global_window_height;
    
    RenderRectangles(app_state, global_textureManager, dt, 0);
    
    HMM_Vec2 leaderBoardDim = {0};
    leaderBoardDim.X = app_state->leaderBoardScreenPos_TopRight.X - app_state->leaderBoardScreenPos_BottomLeft.X; 
    leaderBoardDim.Y = app_state->leaderBoardScreenPos_TopRight.Y - app_state->leaderBoardScreenPos_BottomLeft.Y; 
    
    glScissor(app_state->leaderBoardScreenPos_BottomLeft.X, app_state->leaderBoardScreenPos_BottomLeft.Y, 
        leaderBoardDim.X, leaderBoardDim.Y);
    glEnable(GL_SCISSOR_TEST);
    RenderRectangles(app_state, global_textureManager, dt, 1);
    glDisable(GL_SCISSOR_TEST);    

    unsigned int whiteTextureSlot = global_textureManager->GetTextureSlot("assets/white_texture.jpg");
    if(currentApp == TETRIS){
        Tetris::update(app_state, dt);
        Tetris::draw(app_state, dt);
    }
}