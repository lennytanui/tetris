#include "tetris.cpp"
#include "notes.cpp"

enum APPS {
    TETRIS,
    NOTES
};

enum APPS currentApp = TETRIS;

void app_start(AppState *app_state){
    
    app_state->window_width = global_window_width;
    app_state->window_height = global_window_height;

    if(currentApp == TETRIS){
        Tetris::start(app_state);
    } else if(currentApp == NOTES){    
        Notes::start(app_state);
    }
}

void app_update(AppState *app_state, float dt){
    
    app_state->window_width = global_window_width;
    app_state->window_height = global_window_height;

    if(currentApp == TETRIS){
        Tetris::update(app_state, dt);
        Tetris::draw(app_state, dt);
    } else if(currentApp == NOTES){
        Notes::update(app_state, dt);
        Notes::draw(app_state);
    }

}