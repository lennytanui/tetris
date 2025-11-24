#include "tetris.cpp"
#include <string>
#include "renderer.h"

enum APPS {
    TETRIS
};

enum APPS currentApp = TETRIS;

void app_start(AppState *app_state){
    printf("App  Started..\n");

    app_state->window_width = global_frame_buffer_width;
    app_state->window_height = global_frame_buffer_height;
  
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
    
    RenderRectangles(app_state, global_textureManager, dt);

    unsigned int whiteTextureSlot = global_textureManager->GetTextureSlot("assets/white_texture.jpg");
    if(currentApp == TETRIS){
        Tetris::update(app_state, dt);
        Tetris::draw(app_state, dt);
    }
}