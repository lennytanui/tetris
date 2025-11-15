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

v2 test_pos = v2{400.0f, 400.0f};
float dir = 1.0f;
void app_update(AppState *app_state, float dt){
    
    app_state->window_width = global_window_width;
    app_state->window_height = global_window_height;

    if(currentApp == TETRIS){
        Tetris::update(app_state, dt);
        Tetris::draw(app_state, dt);
        create_render_square(app_state,
        v4{0.0f, 0.0f, 6.0f, 1.0f}, {(float)global_frame_buffer_width * 1.5f, (float)global_frame_buffer_height * 1.5f}, 
        BACKGROUND_COLOR, BACKGROUND_COLOR);

        create_render_square(app_state,
        v4{test_pos.x, test_pos.y, 7.0f, 1.0f}, {50.0f, 50.0f}, 
        RGBA{175.0f, 175.0f, 0.0f, 255.0f}, RGBA{175.0f, 175.0f, 0.0f, 255.0f});
        
        if(test_pos.x >= global_frame_buffer_width - 50.0f){
            dir = -1;
        }else if(test_pos.x <= 0.0f){
            dir = 1;
        }
        
        test_pos.x += dir * 120.0f * dt;
    } else if(currentApp == NOTES){
        Notes::update(app_state, dt);
        Notes::draw(app_state);
    }
}