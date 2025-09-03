#include "tetris.cpp"
#include "notes.cpp"
#include <string>

enum APPS {
    TETRIS,
    NOTES
};

enum APPS currentApp = TETRIS;

void app_start(AppState *app_state){
    printf("App  Started..\n");

    // Tetris::UpdateDimensions();

    app_state->window_width = global_frame_buffer_width;
    app_state->window_height = global_frame_buffer_height;

    if(currentApp == TETRIS){
#if 0 // Testing File Reading
        const char* file_path = "assets/Passion_One/PassionOne-Regular.ttf";
        std::ifstream data_file(file_path);
        if(data_file.is_open()){
            int entries_count = 0;

            std::string raw_line;
            while(std::getline(data_file, raw_line)){
                printf("line %s\n", &raw_line[0]);
            }
            
            data_file.close();
        }else{
            printf("Failed opened file %s \n", file_path);
        }
#endif

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