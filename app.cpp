#pragma once
#include <soloud/soloud.h>
#include <soloud/soloud_wav.h>

#include "app.h"
#include "renderer.cpp"
#include "shapes.cpp"
#include "ui.cpp"
#include "save.cpp"
#include "str.c"

#include <random>
#include <fstream>

/**
    TODO: [x] make it so the tiles do not move into other tiles when
        moving right and left
    [X] check for new rows completed after a row everything shifts
        down
    [] fix first pieces bug when it reaches down
    [x] create scoring system
    [] add options for 4 different resolutions including
        full screen
    [X] game over screen
        shows score, and options to restart or quit
    [x] randomize pieces
    [x] holding system
    [] music volume slider
    [] mute setting
    [] leaderboard that stores the time and date of each play
    [] Nicer Looking Buttons
    [] smooth text rendering
    [] write the setup documentation
    [] optimizations
*/

#define TILE_COUNT_X 10
#define TILE_COUNT_Y 18

#define TILE_SIZE 30.0f
#define BORDER_CLR {72.0f, 79.0f, 72.0f, 255.0f}
#define TILE_CLR {16.0f, 31.0f, 17.0f, 255.0f}
#define BACKGROUND_COLOR {50.0f, 50.0f, 50.0f, 255.0f}

struct Tile{
    RGBA color;
    RGBA border_clr;

    bool taken;
};

struct Tetris_Board{
    Tile tiles[TILE_COUNT_X][TILE_COUNT_Y];
};

static const Block_Info blocks_table[SHAPES_COUNT] = {
    SHAPE_straight, SHAPE_l1, SHAPE_l2, SHAPE_square, 
    SHAPE_zigzag1, SHAPE_zigzag2, SHAPE_t
};

#pragma pack(push, 1)
struct Score{
    int id;
    float score;
    float time;
    uint8_t age;
};

#pragma pack(pop)

struct Score_1{
    int id;
    float score;
    float time;
    uint8_t age;
};

struct ScoreDataManager{
    int scores_count;
    Score *scores;
};

AppState *global_app_state = 0;

Tetris_Board global_tetris_board = {};

Block_Info global_parent = SHAPE_zigzag2;
Child_Block *current_blk = &global_parent.rotations[0];
int parent_blk_index = 0;

float move_amount = TILE_SIZE;
v2 start_pos = {TILE_SIZE * 7, TILE_SIZE * 1};
v2 curr_pos = {start_pos.x + TILE_SIZE * 3, TILE_SIZE * TILE_COUNT_Y - 4};
bool global_pause = false;
bool global_game_over = false;
bool global_show_leaderboard = false;
bool global_show_menuboard = false;
ScoreDataManager global_sdm = {};
int reached_down = 0;
int global_score = 0;
int score_per_line = 100;
int global_rotation_index = 0;

Block_Info held_blck_parent = {};
int can_hold = true;
v2 held_blck_pos = {TILE_SIZE * 1, start_pos.y + TILE_SIZE * TILE_COUNT_Y - TILE_SIZE * 8};

SoLoud::Soloud gSoloud; // SoLoud engine
SoLoud::Wav gWave;      // One wave file
SoLoud::Wav global_wav_reached_down;
SoLoud::Wav global_wav_move;
SoLoud::Wav global_wav_phase;

std::random_device rd;
std::uniform_int_distribution<int> dist(0, SHAPES_COUNT - 1);

bool global_phase_down = false;

float time_btw_moves = 0.5f;

#define PHASE_TIME 0.01f;
float time_to_next_move = time_btw_moves;

static float global_shake_sin = 0.0f;
#define CAMERA_SHAKE_SPEED 30
#define CAMERA_SHAKE_DURATION 0.05f
static bool camera_can_shake = false;
static float camera_shake_time_left = 0.0f;

TextRendererManager trm = {};
InputManager im = {};

void camera_shake(v3 *position, float dt){
    if(camera_can_shake){
        global_shake_sin += CAMERA_SHAKE_SPEED * dt;

        position->x += cos(global_shake_sin);
        position->y += sin(global_shake_sin);
    }

    if(camera_shake_time_left <= 0.0f){
        camera_can_shake = false;
        global_shake_sin = 0.0f;
        *position = CAMERA_NORMAL_POSITION;
    }else{
        camera_shake_time_left -= dt;
    }
}

void SetCursorPosition(float x, float y){
    
    if(global_app_state){
        HMM_Vec4 cursor_pos = {x, y, 0.0f, 1.0f};
        cursor_pos.X /= window_width * 0.5f;
        cursor_pos.Y /= window_height * 0.5f;
        
        cursor_pos.X -= 1.0f;
        cursor_pos.Y -= 1.0f;
        cursor_pos.Y *= -1;

        cursor_pos = HMM_InvGeneralM4(global_app_state->proj) * cursor_pos;
        
        im.cursorX = cursor_pos.X;
        im.cursorY = cursor_pos.Y;
    }
}

void FindFullLines(){
    int full_lines[4] = {};
    int index = 0;

    for(int i = 0; i < TILE_COUNT_Y; i++){
        int full_line = true;
        for(int j = 0; j < TILE_COUNT_X; j++){
            Tile *tile = &global_tetris_board.tiles[j][i];

            if(!tile->taken){
                full_line = false;
                break;
            }
        }   

        if(full_line){
            full_lines[index++] = i + 1;
        }
    }

    int cleared_lines = 0;
    for(int i = 0; i < 4; i++){
        if(full_lines[i] != 0){
            cleared_lines++;
            printf("full line -- %i\n", full_lines[i]);
            int line = full_lines[i];

            for(int j = 0; j < TILE_COUNT_X; j++){
                Tile *tile = &global_tetris_board.tiles[j][line - 1];
                tile->taken = false;
                tile->color = TILE_CLR;
            }

            for(int j0 = line; j0 < TILE_COUNT_Y; j0++){
                for(int j1 = 0; j1 < TILE_COUNT_X; j1++){
                    Tile *tile_top = &global_tetris_board.tiles[j1][j0];
                    Tile *tile_bottom = &global_tetris_board.tiles[j1][j0 - 1];

                    if(tile_top->taken){
                        tile_bottom->taken = true;
                        tile_bottom->color = tile_top->color;
                    }
                    tile_top->taken = false;
                    tile_top->color = TILE_CLR;
                }
            }
        }
    }

    if(cleared_lines == 4){
        global_score += 800.0f;
    } else if(cleared_lines >= 0 && cleared_lines < 4){
        global_score += 100 * cleared_lines + cleared_lines * 15.0f; 
    } else {
        printf("--- STRANGE --- > strange number of cleared lines --- > %i\n", cleared_lines);
    }

    time_btw_moves -= 0.005f * cleared_lines;

    full_lines[0] = 0;
    full_lines[1] = 0;
    full_lines[2] = 0;
    full_lines[3] = 0;

    index = 0;
    for(int i = 0; i < TILE_COUNT_Y; i++){
        int full_line = true;
        for(int j = 0; j < TILE_COUNT_X; j++){
            Tile *tile = &global_tetris_board.tiles[j][i];

            if(!tile->taken){
                full_line = false;
                break;
            }
        }   

        if(full_line){
            full_lines[index++] = i + 1;
        }
    }

    if(index > 0){
        printf("--found a new line after new lines were solved, should run this function again\n");
        FindFullLines();
    }
}

v2 GetBoardCoord(v2 position){
    v2 result = {};

    result.x = (float)(int)(position.x - start_pos.x) / TILE_SIZE;
    result.y = (float)(int)(position.y - start_pos.y) / TILE_SIZE;

    return result;
}

int used_blocks[SHAPES_COUNT] = {};
Block_Info GetNewParentBlock(){
    int index = 0;
    Block_Info result = {};

    if(parent_blk_index > 6){
        parent_blk_index = 0;
    }
    // result = blocks_table[parent_blk_index++];
    int rand_index = dist(rd);

    // check if used_blcks array is empty
    int used_block_array_has_empty = false;
    for(int i = 0; i < SHAPES_COUNT; i++){
        if(used_blocks[i] == 0){
            used_block_array_has_empty = true;
            break;
        }
    }

    if(!used_block_array_has_empty){
        for(int i = 0; i < SHAPES_COUNT; i++){
            used_blocks[i] = 0;
        }
    }

    while(used_blocks[rand_index] != 0){
        rand_index = dist(rd);
    }

    used_blocks[rand_index]++;
    
    result = blocks_table[rand_index];

    // check if out of bounds on the right
    for(int i = 0; i < 4; i++){
        v2 tile_coord = result.rotations[0].structure[i];
        if((curr_pos.x + tile_coord.x * TILE_SIZE) >= 
            (start_pos.x + TILE_SIZE * TILE_COUNT_X)){

            // adjust right to be in bounds
            curr_pos.x -= TILE_SIZE * 
                ((curr_pos.x + tile_coord.x * TILE_SIZE) / 
                    (start_pos.x + TILE_SIZE * TILE_COUNT_X));

        }
    }

    return result;
}

int ReachedObstacle(){
    int result = 0;

    for(int i = 0; i < 4; i++){
        v2 structure = current_blk->structure[i];
        v2 coord = GetBoardCoord({structure.x * TILE_SIZE + curr_pos.x, structure.y * TILE_SIZE + curr_pos.y - 1});

        if(coord.y < 0){
            result = true;
            break;
        }

        Tile *tile = &global_tetris_board.tiles[(int)coord.x][(int)coord.y];
        if(tile->taken){
            result = true;
            break;
        }
    }

    return result;
}

void move_tetromino(int key){

    bool out_of_bounds_left = false;
    bool out_of_bounds_right = false;
  
    for(int i = 0; i < 4; i++){
        // Note (Lenny) : Unnecessary??
        v2 tile_cell_pos = curr_pos;
        tile_cell_pos.x += current_blk->structure[i].e[0] * TILE_SIZE;
        tile_cell_pos.y += current_blk->structure[i].e[1] * TILE_SIZE;

        v2 coord = GetBoardCoord(tile_cell_pos); 

        if((coord.x - 1) < 0){
            out_of_bounds_left = true;
        }

        if((coord.x + 1) > (TILE_COUNT_X - 1)){
            out_of_bounds_right = true;
        }
    }

    if(global_phase_down){
        return;
    }

    if(key != GLFW_KEY_ESCAPE && global_pause){
        return;
    }

    if(global_game_over){
        return;
    }

    switch(key){

        // zoom down
        case GLFW_KEY_SPACE:{
            time_to_next_move = 0.0f;
            global_phase_down = true;
        }

        case GLFW_KEY_S:{
            curr_pos.y -= move_amount;
            gSoloud.play(global_wav_move);
            break;
        }

        case GLFW_KEY_D:{
            if(!out_of_bounds_right){
                curr_pos.x += move_amount;
            }

            if(ReachedObstacle()){
                curr_pos.x -= move_amount;
            }else{
                if(!out_of_bounds_right){
                    gSoloud.play(global_wav_move);
                }
            }

            break;
        }
        
        case GLFW_KEY_A:{

            if(!out_of_bounds_left){
                curr_pos.x -= move_amount;
            }

            if(ReachedObstacle()){
                curr_pos.x += move_amount;
            }else{
                if(!out_of_bounds_left){
                    gSoloud.play(global_wav_move);
                }
            }
            
            break;
        }

        case GLFW_KEY_W:{
            if(global_rotation_index >= global_parent.rotations_count){
                global_rotation_index = 0;
            }
            current_blk = &global_parent.rotations[global_rotation_index++];
            gSoloud.play(global_wav_move);

            // check if out of bounds on the right
            for(int i = 0; i < 4; i++){
                v2 tile_coord = current_blk->structure[i];
                if((curr_pos.x + tile_coord.x * TILE_SIZE) >= 
                    (start_pos.x + TILE_SIZE * TILE_COUNT_X)){

                    // adjust right to be in bounds
                    curr_pos.x -= TILE_SIZE * 
                        ((curr_pos.x + tile_coord.x * TILE_SIZE) / 
                            (start_pos.x + TILE_SIZE * TILE_COUNT_X));

                }
            }
            break;
        }

        // hold piece key
        case GLFW_KEY_C:{
            printf("z has been clicked\n");

            if(can_hold){
                Block_Info temp_held_parent = global_parent;

                if(held_blck_parent.rotations_count > 0){
                    global_parent = held_blck_parent;
                    held_blck_parent = temp_held_parent;
                }else{
                    // generate new block
                    held_blck_parent = global_parent;
                    global_parent = GetNewParentBlock();
                }

                current_blk = &global_parent.rotations[0];
                global_rotation_index = 0;

                can_hold = false;
            }
            break;
        }

        // pause
        case GLFW_KEY_ESCAPE:{
            if(!global_show_menuboard){
                global_pause = !global_pause;
            }
        }
    }
}

// could use a c++ data structure
void SaveScore(int score){
    DataElement de = {};
    de.score = score;
    de.time = Create_String("time");
    de.date = Create_String("date");
    
    AddToDataFile(Create_String("data.dat"), de);

    // sort the leader board
    ReadDataResult rdr = ReadDataFile(Create_String("data.dat"));

    int swapped = 1;
    while(swapped){
        swapped = 0;
        for(int i = 0; i < rdr.data_len - 1; i++){
            DataElement de_0 = rdr.data[i];
            DataElement de_1 = rdr.data[i + 1];

            if(de_1.score > de_0.score){
                rdr.data[i] = de_1;
                rdr.data[i + 1] = de_0;
                swapped = 1;
            }
        }
    }

    WriteDataFile(Create_String("data.dat"), rdr.data, rdr.data_len, 1);

    free(rdr.data);
}

void draw(AppState *app_state){
    // draw background
    create_render_square(app_state,
        {0.0f, 0.0f}, {(float)PROJ_RIGHT, (float)PROJ_TOP}, 
        BACKGROUND_COLOR, BACKGROUND_COLOR);

    // draw the tiles
    v2 tile_pos = start_pos;

    for(int x = 0; x < TILE_COUNT_X; x++){
        for(int y = 0; y < TILE_COUNT_Y; y++){
            Tile *tile = &global_tetris_board.tiles[x][y];            
            Render_Square *render_square = create_render_square(app_state,
                {tile_pos.x, tile_pos.y}, {TILE_SIZE, TILE_SIZE}, 
                    tile->color, tile->border_clr);
            
            tile_pos.y += TILE_SIZE;
        }
        
        tile_pos.x += TILE_SIZE;
        tile_pos.y = start_pos.y;
    }    

    // draw current block
    if(!global_show_menuboard || !global_show_leaderboard){
        for(int i = 0; i < 4; i++){
            v2 tile_pos = {
                curr_pos.x + current_blk->structure[i].x * TILE_SIZE,
                curr_pos.y + current_blk->structure[i].y * TILE_SIZE
            };

            Render_Square *background = create_render_square(app_state,
                    {tile_pos.x, tile_pos.y}, {TILE_SIZE, TILE_SIZE}, 
                        global_parent.color, BORDER_CLR);
        }
    }

    // draw held block background
    DrawText(&trm, Create_String("HOLD"), 0.5f, 
        {held_blck_pos.x + TILE_SIZE * 1, held_blck_pos.y + TILE_SIZE * 5.5f}, 
        {125.0f, 125.0f, 125.0f});
    Render_Square *render_square = create_render_square(app_state,
        {held_blck_pos.x, held_blck_pos.y}, {TILE_SIZE * 5, TILE_SIZE * 5}, 
            {70.0f, 70.0f, 70.0f, 255.0f}, {70.0f, 70.0f, 70.0f, 255.0f});

    // draw held block 
    if(held_blck_parent.rotations_count > 0){
        for(int i = 0; i < 4; i++){
            v2 tile_pos = {
                held_blck_pos.x + TILE_SIZE + held_blck_parent.rotations[0].structure[i].x * TILE_SIZE,
                held_blck_pos.y + TILE_SIZE + held_blck_parent.rotations[0].structure[i].y * TILE_SIZE
            };

            Render_Square *background = create_render_square(app_state,
                    {tile_pos.x, tile_pos.y}, {TILE_SIZE, TILE_SIZE}, 
                        held_blck_parent.color, BORDER_CLR);
        }
    }


    v2 menu_position = {start_pos.x, start_pos.y + (TILE_SIZE * 2)};
    v2 menu_size = {TILE_SIZE * TILE_COUNT_X, TILE_SIZE * 0.75f * TILE_COUNT_Y};

    // draw pause menu
    if(global_pause){
        // draw menu background
        create_render_square(app_state, menu_position, 
            menu_size, 
        {60.0f, 60.0f, 60.0f, 255.0f}, {60.0f, 60.0f, 60.0f, 255.0f});

        DrawText(&trm, Create_String("PAUSE"), 1.2f, 
        {menu_position.x + TILE_SIZE * 3, 
            menu_position.y + menu_size.y - TILE_SIZE * 3.0f}, 
        {125.0f, 125.0f, 125.0f});
        
        if(Button(AppQuit, &im, &trm,  Create_String("Quit"), 
            HMM_Vec2{menu_position.x + TILE_SIZE * 3, 
            menu_position.y + menu_size.y - TILE_SIZE * 7.0f}, 
                {0.3f, 0.3f, 0.3f, 1.0f})){
            // quit
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        if(Button(draw, &im, &trm,  Create_String("Continue"), 
            HMM_Vec2{menu_position.x + TILE_SIZE * 3, 
            menu_position.y + menu_size.y - TILE_SIZE * 9.0f}, 
                {0.3f, 0.3f, 0.3f, 1.0f})){
            global_pause = false;
        }
        
        if(Button(draw, &im, &trm,  Create_String("Settings"), 
            HMM_Vec2{menu_position.x + TILE_SIZE * 3, 
            menu_position.y + menu_size.y - TILE_SIZE * 11.0f}, 
                {0.3f, 0.3f, 0.3f, 1.0f})){
            global_pause = false;
        }
    }

    // draw game over
    if(global_game_over && !global_show_menuboard && !global_show_leaderboard){
        create_render_square(app_state, menu_position, 
            menu_size, 
        {60.0f, 60.0f, 60.0f, 255.0f}, {60.0f, 60.0f, 60.0f, 255.0f});

        DrawText(&trm, Create_String("GAME OVER!"), 1.2f, 
        {menu_position.x + TILE_SIZE * 1, 
            menu_position.y + menu_size.y - TILE_SIZE * 3.0f}, 
        {125.0f, 125.0f, 125.0f});
        
        String score_string = Create_String("SCORE - ");
        AddToString(&score_string, global_score);
        DrawText(&trm, score_string, 0.9f, 
        {menu_position.x + TILE_SIZE * 1, 
            menu_position.y + menu_size.y - TILE_SIZE * 5.0f}, 
        {125.0f, 125.0f, 125.0f});

        if(Button(AppQuit, &im, &trm,  Create_String("Menu"), 
            HMM_Vec2{menu_position.x + TILE_SIZE * 1, 
            menu_position.y + menu_size.y - TILE_SIZE * 7.0f}, 
                {0.3f, 0.3f, 0.3f, 1.0f})){
            global_show_menuboard = true;
        }


        if(Button(draw, &im, &trm,  Create_String("Restart"), 
            HMM_Vec2{menu_position.x + TILE_SIZE * 1, 
            menu_position.y + menu_size.y - TILE_SIZE * 9.0f}, 
                {0.3f, 0.3f, 0.3f, 1.0f})){
            global_pause = false;

            // clear the grid
            for(int x = 0; x < TILE_COUNT_X; x++){
                for(int y = 0; y < TILE_COUNT_Y; y++){
                    global_tetris_board.tiles[x][y].color = TILE_CLR;
                    global_tetris_board.tiles[x][y].border_clr = BORDER_CLR;
                    global_tetris_board.tiles[x][y].taken = false;
                }
            }

            held_blck_parent = {};
            global_game_over = false;
        }
    }

    // show leader board    
    if(global_show_leaderboard){
        create_render_square(app_state, menu_position, 
            menu_size, 
        {60.0f, 60.0f, 60.0f, 255.0f}, {60.0f, 60.0f, 60.0f, 255.0f});

        DrawText(&trm, Create_String("LEADERBOARD"), 0.9f, 
        {menu_position.x + TILE_SIZE * 1, 
            menu_position.y + menu_size.y - TILE_SIZE * 3.0f}, 
        {125.0f, 125.0f, 125.0f});

        
        ReadDataResult rdr = ReadDataFile(Create_String("data.dat"));
        float y_pos = 5.0f;
        for(int i = 0; i < rdr.data_len; i++){
            String score_string = Create_String("Score : ");
            AddToString(&score_string, rdr.data[i].score);
            DrawText(&trm, score_string, 0.9f, 
            {menu_position.x + TILE_SIZE * 1, 
                menu_position.y + menu_size.y - TILE_SIZE * y_pos}, 
            {125.0f, 125.0f, 125.0f});

            y_pos += 2;
        }

        if(Button(&global_show_leaderboard, &im, &trm,  Create_String("Back"), 
            HMM_Vec2{menu_position.x + TILE_SIZE * 1, 
            menu_position.y + menu_size.y - TILE_SIZE * y_pos}, 
                {0.3f, 0.3f, 0.3f, 1.0f})){
            
            global_show_leaderboard = false;
            global_show_menuboard = true;
        }
    }

    // show menu board
    if(global_show_menuboard){
        create_render_square(app_state, menu_position, 
            menu_size, 
        {60.0f, 60.0f, 60.0f, 255.0f}, {60.0f, 60.0f, 60.0f, 255.0f});

        DrawText(&trm, Create_String("MENU"), 0.9f, 
        {menu_position.x + TILE_SIZE * 1, 
            menu_position.y + menu_size.y - TILE_SIZE * 3.0f}, 
        {125.0f, 125.0f, 125.0f});

        float y_pos = 5.0f;
        if(Button(&global_pause, &im, &trm,  Create_String("Play"), 
            HMM_Vec2{menu_position.x + TILE_SIZE * 1, 
            menu_position.y + menu_size.y - TILE_SIZE * y_pos}, 
                {0.3f, 0.3f, 0.3f, 1.0f})){
            
            global_pause = false;

            // clear the grid
            for(int x = 0; x < TILE_COUNT_X; x++){
                for(int y = 0; y < TILE_COUNT_Y; y++){
                    global_tetris_board.tiles[x][y].color = TILE_CLR;
                    global_tetris_board.tiles[x][y].border_clr = BORDER_CLR;
                    global_tetris_board.tiles[x][y].taken = false;
                }
            }

            held_blck_parent = {};
            global_game_over = false;

            global_show_menuboard = false;
        }

        if(Button(&global_show_leaderboard, &im, &trm,  Create_String("Leaderboard"), 
            HMM_Vec2{menu_position.x + TILE_SIZE * 1, 
            menu_position.y + menu_size.y - TILE_SIZE * (y_pos + 2)}, 
                {0.3f, 0.3f, 0.3f, 1.0f})){
                global_show_leaderboard = true;
                global_show_menuboard = false;
        }
        
        if(Button(&AppQuit, &im, &trm,  Create_String("Quit"), 
            HMM_Vec2{menu_position.x + TILE_SIZE * 1, 
            menu_position.y + menu_size.y - TILE_SIZE * (y_pos + 4)}, 
                {0.3f, 0.3f, 0.3f, 1.0f})){

            // quit
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
}

void app_start(AppState *app_state){
    global_app_state = app_state;
    global_show_menuboard = true;
    ReadDataFile(Create_String("data.dat"));
    for(int x = 0; x < TILE_COUNT_X; x++){
        for(int y = 0; y < TILE_COUNT_Y; y++){
            global_tetris_board.tiles[x][y].color = TILE_CLR;
            global_tetris_board.tiles[x][y].border_clr = BORDER_CLR;
        }
    }

    app_state->cam_pos = CAMERA_NORMAL_POSITION;
    gSoloud.init(); // Initialize SoLoud
    gWave.load("assets/Future-Technology.wav"); // Load a wave
    // gSoloud.play(gWave); // Play the wave
    gWave.setLooping(1);

    global_wav_reached_down.load("assets/ImpactIntoSand.wav");
    global_wav_phase.load("assets/Retro Block Hit.wav");
    global_wav_move.load("assets/Click.wav");

    SetupTextRenderer(&trm, PROJ_RIGHT, PROJ_TOP);
    Setup2dRendering(&trm);
    im.window = window;
}

void app_update(AppState *app_state, float dt){
    
    // TODO: work on borders
    if(reached_down){
        // change the color of the bottom blocks
        for(int i = 0; i < 4; i++){
            v2 size = {TILE_SIZE, TILE_SIZE};
            v2 tile_cell_pos = curr_pos;
            tile_cell_pos.x += current_blk->structure[i].e[0] * size.x;
            tile_cell_pos.y += current_blk->structure[i].e[1] * size.y;

            v2 coord = GetBoardCoord(tile_cell_pos); 

            Tile *tile = &global_tetris_board.tiles[(int)coord.x][(int)coord.y];
            tile->taken = true;
            tile->color = global_parent.color;
        }

        // Note (Lenny) : should the index persist?
        global_parent = GetNewParentBlock();
        current_blk = &global_parent.rotations[0];
        global_rotation_index = 0;


        // move to the top
        curr_pos.y = start_pos.y + TILE_SIZE * (TILE_COUNT_Y - 2);
        reached_down = 0;

        // check if spawned on top of a piece
        if(ReachedObstacle()){
            if(global_score > 0){
                SaveScore(global_score);
                global_score = 0;
            }
            global_game_over = true;
        }

        if(can_hold == false) {
            can_hold = true;    
        }

        FindFullLines();

        if(global_phase_down){
            camera_can_shake = true;
            camera_shake_time_left = CAMERA_SHAKE_DURATION;
            global_phase_down = false;
        }
        global_wav_reached_down.stop();
        gSoloud.play(global_wav_reached_down);
    }

    // move down
    if(time_to_next_move <= 0 && !global_game_over && !global_pause && !global_show_menuboard && !global_show_leaderboard){
        curr_pos.y -= move_amount;

        if(!global_phase_down){
            time_to_next_move = time_btw_moves;
        }else{
            gSoloud.play(global_wav_phase);
            time_to_next_move = PHASE_TIME;
        }

    }

    if(!global_pause || !global_game_over || !global_show_menuboard ||  !global_show_leaderboard){
        time_to_next_move -= dt;
    }

    // compute if reached down
    if(global_game_over || global_show_menuboard){
        reached_down = 0;
    }else{
        reached_down = ReachedObstacle();
    }

    camera_shake(&app_state->cam_pos, dt);

    draw(app_state);
    String score_str = Create_String("SCORE : ");
    AddToString(&score_str, global_score);
    DrawText(&trm, score_str, 1.0f, 
        {start_pos.x + (TILE_SIZE / 2.0f) * TILE_COUNT_X - TILE_SIZE * 2, PROJ_TOP - TILE_SIZE * 2}, 
        {200.0f, 200.0f, 200.0f});
}