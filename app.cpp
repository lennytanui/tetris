#include "tetris.cpp"
#include "notes.cpp"

enum APPS {
    TETRIS,
    NOTES
};

enum APPS currentApp = TETRIS;

void app_start(AppState *app_state){
    if(currentApp == TETRIS){
        Tetris::start(app_state);
    } else if(currentApp == NOTES){    
        Notes::start(app_state);
    }
}

void app_update(AppState *app_state, float dt){
 
    if(currentApp == TETRIS){
        Tetris::update(app_state, dt);
        Tetris::draw(app_state);
    } else if(currentApp == NOTES){
        Notes::update(app_state, dt);
        Notes::draw(app_state);
    }

}