#pragma once
#include <soloud/soloud.h>
#include <soloud/soloud_wav.h>

#include "app.h"
#include "renderer.cpp"
#include "shapes.cpp"
#include "ui.cpp"
#include "save.cpp"
#include "str.c"
#include "utils.h"

#include <random>
#include <fstream>

/**
    TODO: [x] make it so the tiles do not move into other tiles when
        moving right and left
    [X] check for new rows completed after a row everything shifts
        down
    [X] fix first pieces bug when it reaches down
    [X] create scoring system
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

#define TILE_SIZE 37.0f
#define BORDER_CLR {72.0f, 79.0f, 72.0f, 0.0f}
#define TILE_CLR {16.0f, 31.0f, 17.0f, 255.0f}
#define BACKGROUND_COLOR {50.0f, 50.0f, 50.0f, 255.0f}
#define TIME_TO_CLEAR_ROW 1.0f

struct Tile{
    Block_Info type;
    RGBA color;
    RGBA border_clr;
    float age;
    bool taken;
    HMM_Vec2 size;
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

AppState global_app_state = {0};

Tetris_Board global_tetris_board = {};

Block_Info global_parent = SHAPE_straight; // Starting block
Child_Block *current_blk = &global_parent.rotations[0];
int parent_blk_index = 0;

float move_amount = TILE_SIZE;
HMM_Vec2 start_pos = {0};
HMM_Vec2 curr_pos = {0};
bool global_pause = false;
bool global_game_over = false;
bool global_show_leaderboard = false;
bool global_show_menuboard = false;
ScoreDataManager global_sdm = {};
int global_reached_down = 0;
int global_score = 0;
int global_last_game_score = 0;
int score_per_line = 100;
int global_rotation_index = 0;

Block_Info held_blck_parent = {};
int can_hold = true;
HMM_Vec2 held_blck_pos = {0};

SoLoud::Soloud gSoloud; // SoLoud engine
SoLoud::Wav gWave;      // One wave file
SoLoud::Wav global_wav_reached_down;
SoLoud::Wav global_wav_move;
SoLoud::Wav global_wav_phase;

std::random_device rd;
std::uniform_int_distribution<int> dist(0, SHAPES_COUNT - 1);

bool global_phase_down = false;
float global_lines_count_to_clear = 0; // # of line to clear
float global_time_to_clear_lines = 0; // time to clear the filled lines
int global_lines_to_clear[4] = {-1, -1, -1, -1};

float global_time_btw_moves = 0.5f;

#define PHASE_TIME 0.01f;
float global_time_to_next_move = global_time_btw_moves;

ParticleManager global_pms[TILE_COUNT_X][4] = {};

static float global_shake_sin = 0.0f;
#define CAMERA_SHAKE_SPEED 30
#define CAMERA_SHAKE_DURATION 0.05f
static bool camera_can_shake = false;
static float camera_shake_time_left = 0.0f;

InputManager im = {};
HMM_Vec2 testSquarePos = {50.0f, 50.0f};
static bool mobileSliding = false;
static HMM_Vec2 initialPiecePos = {0}; // for mobile sliding
static float slideTime = 0.0f;
static float verticalSlideTime = 0.5f;

void SetCursorPosition(float x, float y){
    
    if(global_app_state.initialized){
        HMM_Vec4 cursor_pos = {x, y, 0.0f, 1.0f};
        cursor_pos.X /= global_frame_buffer_width * 0.5f;
        cursor_pos.Y /= global_frame_buffer_height * 0.5f;
 

        cursor_pos.X -= 1.0f;
        cursor_pos.Y -= 1.0f;
        cursor_pos.Y *= -1;
 
        cursor_pos = HMM_InvGeneralM4(global_app_state.proj) * cursor_pos;
        
        im.cursorPos = HMM_Vec2{cursor_pos.X, cursor_pos.Y};
    }
    
}

namespace Tetris{
void camera_shake(HMM_Vec3 *position, float dt){
    if(camera_can_shake){
        global_shake_sin += CAMERA_SHAKE_SPEED * dt;

        position->X += cos(global_shake_sin);
        position->Y += sin(global_shake_sin);
    }

    if(camera_shake_time_left <= 0.0f){
        camera_can_shake = false;
        global_shake_sin = 0.0f;
        *position = CAMERA_NORMAL_POSITION;
    }else{
        camera_shake_time_left -= dt;
    }
}

void FindFullLines(){
    int cleared_lines = 0;
    int full_lines[4] = {-1, -1, -1, -1};
    int full_lines_count = 0;

    for(int i = 0; i < TILE_COUNT_Y; i++){
        int full_line = true;
        for(int j = 0; j < TILE_COUNT_X; j++){
            Tile *tile = &global_tetris_board.tiles[j][i];

            if(!tile->taken){
                full_line = false;
                j = TILE_COUNT_X;
            }
        }   

        if(full_line){
            full_lines[full_lines_count++] = i;
            cleared_lines++;
        }
    }

    // clear the lines
    if(cleared_lines > 0){
        global_lines_count_to_clear = (float)cleared_lines;
        global_time_to_clear_lines = (float)cleared_lines;

        global_lines_to_clear[0] = full_lines[0];
        global_lines_to_clear[1] = full_lines[1];
        global_lines_to_clear[2] = full_lines[2];
        global_lines_to_clear[3] = full_lines[3];

        // Update scor based on cleared lines
        if(cleared_lines == 4){
           global_score += 800.0f;
        } else if(cleared_lines >= 0 && cleared_lines < 4){
            global_score += 100 * cleared_lines + cleared_lines * 15.0f; 
        } else {
            printf("--- STRANGE --- > strange number of cleared lines --- > %i\n", cleared_lines);
        }

    }

    // global_time_btw_moves -= 0.005f * cleared_lines;
}

HMM_Vec2 GetBoardCoord(HMM_Vec2 position){
    HMM_Vec2 result = {};

    result.X = (float)(int)(position.X - start_pos.X) / TILE_SIZE;
    result.Y = (float)(int)(position.Y - start_pos.Y) / TILE_SIZE;

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
        HMM_Vec2 tile_coord = result.rotations[0].structure[i];
        if((curr_pos.X + tile_coord.X * TILE_SIZE) >= 
            (start_pos.X + TILE_SIZE * TILE_COUNT_X)){

            // adjust right to be in bounds
            curr_pos.X -= TILE_SIZE * 
                ((curr_pos.X + tile_coord.X * TILE_SIZE) / 
                    (start_pos.X + TILE_SIZE * TILE_COUNT_X));

        }
    }

    return result;
}

int ReachedObstacle(HMM_Vec2 position){
    int result = 0;

    for(int i = 0; i < 4; i++){
        HMM_Vec2 structure = current_blk->structure[i];
        HMM_Vec2 coord = GetBoardCoord({structure.X * TILE_SIZE + position.X, structure.Y * TILE_SIZE + position.Y - 1});

        if(coord.Y <= 0){
            result = true;
            break;
        }

        Tile *tile = &global_tetris_board.tiles[(int)coord.X][(int)coord.Y];
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
        HMM_Vec2 tile_cell_pos = curr_pos;
        tile_cell_pos.X += current_blk->structure[i].Elements[0] * TILE_SIZE;
        tile_cell_pos.Y += current_blk->structure[i].Elements[1] * TILE_SIZE;

        HMM_Vec2 coord = GetBoardCoord(tile_cell_pos); 

        if((coord.X - 1) < 0){
            out_of_bounds_left = true;
        }

        if((coord.X + 1) > (TILE_COUNT_X - 1)){
            out_of_bounds_right = true;
        }
    }

    if(global_phase_down){
        return;
    }

    if(key != GLFW_KEY_ESCAPE && global_pause){
        return;
    }

    if(global_game_over || global_show_menuboard){
        return;
    }

    switch(key){
        // zoom down
        case GLFW_KEY_SPACE:{
            global_time_to_next_move = 0.0f;
            global_phase_down = true;
            
            gSoloud.play(global_wav_phase);
            break;
        }

        case GLFW_KEY_DOWN:
        case GLFW_KEY_S:{
            curr_pos.Y -= move_amount;
            gSoloud.play(global_wav_move);
            break;
        }

        
        case GLFW_KEY_RIGHT:
        case GLFW_KEY_D:{
            if(!out_of_bounds_right){
                curr_pos.X += move_amount;
            }

            if(ReachedObstacle(curr_pos)){
                curr_pos.X -= move_amount;
            }else{
                if(!out_of_bounds_right){
                    gSoloud.play(global_wav_move);
                }
            }

            break;
        }
        
        case GLFW_KEY_LEFT:
        case GLFW_KEY_A:{

            if(!out_of_bounds_left){
                curr_pos.X -= move_amount;
            }

            if(ReachedObstacle(curr_pos)){
                curr_pos.X += move_amount;
            }else{
                if(!out_of_bounds_left){
                    gSoloud.play(global_wav_move);
                }
            }
            
            break;
        }

        
        case GLFW_KEY_UP:
        case GLFW_KEY_W:{
            if((global_rotation_index + 1) >= global_parent.rotations_count){
                global_rotation_index = -1;
            }
            current_blk = &global_parent.rotations[++global_rotation_index];
            gSoloud.play(global_wav_move);

            // check if out of bounds on the right
            for(int i = 0; i < 4; i++){
                HMM_Vec2 tile_coord = current_blk->structure[i];
                if((curr_pos.X + tile_coord.X * TILE_SIZE) >= 
                    (start_pos.X + TILE_SIZE * TILE_COUNT_X)){

                    // adjust right to be in bounds
                    curr_pos.X -= TILE_SIZE * 
                        ((curr_pos.X + tile_coord.X * TILE_SIZE) / 
                            (start_pos.X + TILE_SIZE * TILE_COUNT_X));

                }
            }
            break;
        }

        // hold piece key
        case GLFW_KEY_C:{

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
    
    AddToDataFile("data.dat", de);

    // sort the leader board
    ReadDataResult rdr = ReadDataFile("data.dat");

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

    WriteDataFile("data.dat", rdr.data, rdr.data_len, 1);

    free(rdr.data);
}

void ResetParticleManager(ParticleManager *pm, HMM_Vec2 position){
    if(!pm->ready){
        *pm = {0};
        pm->count = 100;
        pm->ready = true;
        pm->life_time = 1.0f;

        for(int i = 0; i < pm->count; i++){
            Particle *particle = &pm->particles[i];
            // particle->acceleration = {0.5f,0.5f};
            float randx = RandomFloat(0.1, 0.5) * 12.7;
            float randy = RandomFloat(-0.5, 0.5) * 1.1;
            particle->acceleration.X = randx; // rand
            particle->acceleration.Y = randy;
            particle->position = position;
            particle->size = {10, 10};
            particle->color = {RandomFloat(30, 255.0f), RandomFloat(30, 255.0f), RandomFloat(30, 255.0f), 255.0f};
        }
    }
}

void Resize_UpdatePositions(){
    HMM_Vec2 old_start_pos = {start_pos.X, start_pos.Y};
    
    start_pos.X = global_ortho_width / 2.0f;
    start_pos.X -= (TILE_SIZE * TILE_COUNT_X) / 2.0f;
    start_pos.Y = global_ortho_height / 2.0f;
    start_pos.Y -= (TILE_SIZE * TILE_COUNT_Y) / 2.0f;

    HMM_Vec2 pos_diff = start_pos - old_start_pos;
    curr_pos += pos_diff;

    held_blck_pos = {global_ortho_width / 2.0f, start_pos.Y + TILE_SIZE * TILE_COUNT_Y - TILE_SIZE * 8};
    held_blck_pos.X -= (TILE_SIZE * TILE_COUNT_X) / 2.0f;
    held_blck_pos.X -= TILE_SIZE * 5.0f;
}

void UpdateDimensions(){
    Resize_UpdatePositions();
    
    global_UIRenderer->UpdateTextRendererDimensions(global_ortho_width, global_ortho_height);
    // global_app_state.proj = HMM_Orthographic_LH_NO(0.0f, global_frame_buffer_width, 0.0f, global_frame_buffer_height, 0.0f, 10.0f);

    global_app_state.proj = HMM_Orthographic_LH_NO(0.0f, 1000, 0.0f, global_ortho_height, 0.0f, 10.0f);
}

void draw(AppState *app_state, float dt){
    unsigned whiteTextureSlot = global_textureManager->GetTextureSlot("assets/white_texture.jpg");
    // draw background
    create_render_square(app_state,
        HMM_Vec4{0.0f, 0.0f, 6.0f, 1.0f}, {(float)global_ortho_width * 1.5f, (float)global_ortho_height * 1.5f}, 
        BACKGROUND_COLOR, BACKGROUND_COLOR, whiteTextureSlot);
#if 1
    // draw the tiles
    HMM_Vec2 tile_pos = start_pos;

    for(int x = 0; x < TILE_COUNT_X; x++){
        for(int y = 0; y < TILE_COUNT_Y; y++){
            Tile *tile = &global_tetris_board.tiles[x][y];   
            
            float t = 0.25f;

            HMM_Vec2 tile_pos_offset = {0.0f, 0.0f};
            if(tile->taken){
                // block lock animation if age < 5;
                float grow_size = 100.0f;
                if(tile->age <= t){
                    if(tile->age < (t / 2.0f)){
                        tile->size.X += grow_size * dt;
                        tile->size.Y += grow_size * dt;

                        tile_pos_offset.X = -1 * grow_size * tile->age;
                        tile_pos_offset.Y = -1 * grow_size * tile->age;
                    } else {
                        tile->size.X -= grow_size * dt;
                        tile->size.Y -= grow_size * dt;
                        
                        tile_pos_offset.X = -1 * grow_size * (t - tile->age);
                        tile_pos_offset.Y = -1 * grow_size * (t - tile->age);
                    }
                }else{
                    tile->size.X = TILE_SIZE;
                    tile->size.Y = TILE_SIZE;
                }
            }

            float z_index = 5.0f;
            if(tile->age < t & tile->age > 0){
                z_index = 5.5f;
            }
            Render_Square *render_square = create_render_square(app_state,
                HMM_Vec4{tile_pos.X + tile_pos_offset.X, tile_pos.Y + tile_pos_offset.Y, z_index, 1.0f}, {tile->size.X, tile->size.Y}, 
                    tile->color, tile->border_clr, whiteTextureSlot);
            
            tile_pos.Y += TILE_SIZE;
        }
        
        tile_pos.X += TILE_SIZE;
        tile_pos.Y = start_pos.Y;
    }    

    // draw active block
    if(!global_show_menuboard || !global_show_leaderboard){
        for(int i = 0; i < 4; i++){
            HMM_Vec2 tile_pos = {
                curr_pos.X + current_blk->structure[i].X * TILE_SIZE,
                curr_pos.Y + current_blk->structure[i].Y * TILE_SIZE
            };

            Render_Square *background = create_render_square(app_state,
                    HMM_Vec4{tile_pos.X, tile_pos.Y, 4.0f, 1.0f}, {TILE_SIZE, TILE_SIZE}, 
                        global_parent.color, BORDER_CLR, whiteTextureSlot);
        }

        // calculate active block "shadow" position
        int current_blk_height = 0;
        float shadow_y = 0;
        for(int i = 0; i < 4; i++){
            if(current_blk->structure[i].Y > current_blk_height){
                current_blk_height = current_blk->structure[i].Y;
            }

            // Scan through the lower tiles to find closest block
            int reached_down = 0;
            float j = curr_pos.Y;
        
            while(reached_down == 0){
                HMM_Vec2 tile_pos = {
                    curr_pos.X + (current_blk->structure[i].X * TILE_SIZE),
                    j + (current_blk->structure[i].Y * TILE_SIZE)
                };

                reached_down = ReachedObstacle(HMM_Vec2{curr_pos.X, j});
                j--;
            }
            if(j > shadow_y){
                shadow_y = j;
            }
        }

        // draw current block "shadow"
        for(int i = 0; i < 4; i++){
            HMM_Vec2 tile_pos = {
                curr_pos.X + (current_blk->structure[i].X * TILE_SIZE),
                shadow_y + (current_blk->structure[i].Y * TILE_SIZE)
            };

            Render_Square *background = create_render_square(app_state,
                    {tile_pos.X, tile_pos.Y, 4.0f, 1.0f}, {TILE_SIZE, TILE_SIZE}, 
                        RGBA{1.0f, 0.0f, 1.0f, 1.0f}, global_parent.color, whiteTextureSlot);
        }

    }

    // draw held block background
    global_UIRenderer->DrawText(Create_String("HOLD"), 0.5f, 
        {held_blck_pos.X + TILE_SIZE * 1, held_blck_pos.Y + TILE_SIZE * 5.5f}, 
        {125.0f, 125.0f, 125.0f});
    Render_Square *render_square = create_render_square(app_state,
        {held_blck_pos.X, held_blck_pos.Y, 1.0f, 1.0f}, {TILE_SIZE * 4, TILE_SIZE * 4}, 
            {70.0f, 70.0f, 70.0f, 255.0f}, {70.0f, 70.0f, 70.0f, 255.0f}, whiteTextureSlot);

    // draw held block 
    if(held_blck_parent.rotations_count > 0){
        for(int i = 0; i < 4; i++){
            HMM_Vec2 tile_pos = {
                held_blck_pos.X + TILE_SIZE + held_blck_parent.rotations[0].structure[i].X * TILE_SIZE,
                held_blck_pos.Y + TILE_SIZE + held_blck_parent.rotations[0].structure[i].Y * TILE_SIZE
            };

            Render_Square *background = create_render_square(app_state,
                    {tile_pos.X, tile_pos.Y, 0.0f, 1.0f}, {TILE_SIZE, TILE_SIZE}, 
                        held_blck_parent.color, BORDER_CLR, whiteTextureSlot);
        }
    }

    create_render_square(app_state, {testSquarePos.X, testSquarePos.Y, 0.0f, 1.0f}, 
        {TILE_SIZE, TILE_SIZE},RGBA{255.0f, 0.0f, 0.0f, 50.0f}, BORDER_CLR, whiteTextureSlot);


    HMM_Vec2 menu_position = {start_pos.X, start_pos.Y + (TILE_SIZE * 2)};
    HMM_Vec2 menu_size = {TILE_SIZE * TILE_COUNT_X, TILE_SIZE * 0.75f * TILE_COUNT_Y};

    // draw pause menu
    if(global_pause){
        // draw menu background
        create_render_square(app_state, HMM_Vec4{menu_position.X, menu_position.Y, 0.0f, 1.0f}, 
            menu_size, {60.0f, 60.0f, 60.0f, 255.0f}, 
            {60.0f, 60.0f, 60.0f, 255.0f}, whiteTextureSlot);

        global_UIRenderer->DrawText(Create_String("PAUSE"), 1.2f, 
        {menu_position.X + TILE_SIZE * 3, 
            menu_position.Y + menu_size.Y - TILE_SIZE * 3.0f}, 
        {125.0f, 125.0f, 125.0f});
        
        if(Button((void *)AppQuit, &im, global_UIRenderer,  Create_String("Quit"), 
            HMM_Vec2{menu_position.X + TILE_SIZE * 3, 
            menu_position.Y + menu_size.Y - TILE_SIZE * 7.0f}, 
                {0.3f, 0.3f, 0.3f, 1.0f})){
            // quit
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        if(Button((void *)draw, &im, global_UIRenderer,  Create_String("Continue"), 
            HMM_Vec2{menu_position.X + TILE_SIZE * 3, 
            menu_position.Y + menu_size.Y - TILE_SIZE * 9.0f}, 
                {0.3f, 0.3f, 0.3f, 1.0f})){
            global_pause = false;
        }
        
        if(Button((void *)draw, &im, global_UIRenderer,  Create_String("Settings"), 
            HMM_Vec2{menu_position.X + TILE_SIZE * 3, 
            menu_position.Y + menu_size.Y - TILE_SIZE * 11.0f}, 
                {0.3f, 0.3f, 0.3f, 1.0f})){
            global_pause = false;
        }
    }

    // draw game over
    if(global_game_over && !global_show_menuboard && !global_show_leaderboard){
        create_render_square(app_state, HMM_Vec4{menu_position.X, menu_position.Y, 0.0f, 1.0f}, 
            menu_size, {60.0f, 60.0f, 60.0f, 255.0f}, 
            {60.0f, 60.0f, 60.0f, 255.0f}, whiteTextureSlot);

        global_UIRenderer->DrawText(Create_String("GAME OVER!"), 0.9f, 
        {menu_position.X + TILE_SIZE * 1, 
            menu_position.Y + menu_size.Y - TILE_SIZE * 3.0f}, 
        {125.0f, 125.0f, 125.0f});
        
        
        String high_score_str = Create_String("High Score : ");
        AddToString(&high_score_str, 555);
        global_UIRenderer->DrawText(high_score_str, 0.8f, 
        {menu_position.X + TILE_SIZE * 1, 
            menu_position.Y + menu_size.Y - TILE_SIZE * 5.0f},
            {200.0f, 200.0f, 200.0f});

        String score_string = Create_String("SCORE - ");
        AddToString(&score_string, global_last_game_score);
        global_UIRenderer->DrawText(score_string, 0.8f, 
        {menu_position.X + TILE_SIZE * 1, 
            menu_position.Y + menu_size.Y - TILE_SIZE * 7.0f}, 
        {125.0f, 125.0f, 125.0f});

        if(Button((void *)AppQuit, &im, global_UIRenderer,  Create_String("Menu"), 
            HMM_Vec2{menu_position.X + TILE_SIZE * 1, 
            menu_position.Y + menu_size.Y - TILE_SIZE * 9.0f}, 
                {0.3f, 0.3f, 0.3f, 1.0f})){
            global_show_menuboard = true;
        }


        if(Button((void *)draw, &im, global_UIRenderer,  Create_String("Restart"), 
            HMM_Vec2{menu_position.X + TILE_SIZE * 1, 
            menu_position.Y + menu_size.Y - TILE_SIZE * 9.0f}, 
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
        create_render_square(app_state, HMM_Vec4{menu_position.X, menu_position.Y, 0.0f, 1.0f}, 
            menu_size, {60.0f, 60.0f, 60.0f, 255.0f}, 
            {60.0f, 60.0f, 60.0f, 255.0f}, whiteTextureSlot);

        global_UIRenderer->DrawText(Create_String("LEADERBOARD"), 0.9f, 
        {menu_position.X + TILE_SIZE * 1, 
            menu_position.Y + menu_size.Y - TILE_SIZE * 3.0f}, 
        {125.0f, 125.0f, 125.0f});

        
        ReadDataResult rdr = ReadDataFile("data.dat");
        float y_pos = 5.0f;

        for(int i = 0; i < rdr.data_len; i++){
            if(i >= 5){
                break;
            }
            float leader_board_score_height = TILE_SIZE + 48;

            if(i % 2 == 0){
                create_render_square(app_state, {menu_position.X,
                    menu_position.Y + menu_size.Y - TILE_SIZE * y_pos, 0.0f, 1.0f}, 
                    {menu_size.X, leader_board_score_height }, 
                {20.0f, 20.0f, 20.0f, 255.0f}, {20.0f, 20.0f, 20.0f, 255.0f}, whiteTextureSlot);
            } else {
                create_render_square(app_state, {menu_position.X,
                    menu_position.Y + menu_size.Y - TILE_SIZE * y_pos, 0.0f, 1.0f}, 
                    {menu_size.X, leader_board_score_height }, 
                {80.0f, 80.0f, 80.0f, 255.0f}, {80.0f, 80.0f, 80.0f, 255.0f}, whiteTextureSlot);
            }


            String score_string = Create_String("Score : ");
            AddToString(&score_string, rdr.data[i].score);
            global_UIRenderer->DrawText(score_string, 0.9f, 
            {menu_position.X + TILE_SIZE * 1, 
                menu_position.Y + menu_size.Y - TILE_SIZE * y_pos + 48 * 0.5f}, 
            {125.0f, 125.0f, 125.0f});

            y_pos += 2;
        }

        if(Button(&global_show_leaderboard, &im, global_UIRenderer,  Create_String("Back"), 
            HMM_Vec2{menu_position.X + TILE_SIZE * 1, 
            menu_position.Y + menu_size.Y - TILE_SIZE * y_pos}, 
                {0.3f, 0.3f, 0.3f, 1.0f})){
            
            global_show_leaderboard = false;
            global_show_menuboard = true;
        }
    }

    // show menu board
    if(global_show_menuboard){
        create_render_square(app_state, HMM_Vec4{menu_position.X, menu_position.Y, 0.0f, 1.0f}, 
            menu_size, 
        {60.0f, 60.0f, 60.0f, 255.0f}, {60.0f, 60.0f, 60.0f, 255.0f}, whiteTextureSlot);

        global_UIRenderer->DrawText(Create_String("MENU"), 0.9f, 
        {menu_position.X + TILE_SIZE * 1, 
            menu_position.Y + menu_size.Y - TILE_SIZE * 3.0f}, 
        {125.0f, 125.0f, 125.0f});

        float y_pos = 5.0f;
        if(Button(&global_pause, &im, global_UIRenderer,  Create_String("Play"), 
            HMM_Vec2{menu_position.X + TILE_SIZE * 1, 
            menu_position.Y + menu_size.Y - TILE_SIZE * y_pos}, 
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

        if(Button(&global_show_leaderboard, &im, global_UIRenderer,  Create_String("Leaderboard"), 
            HMM_Vec2{menu_position.X + TILE_SIZE * 1, 
            menu_position.Y + menu_size.Y - TILE_SIZE * (y_pos + 2)}, 
                {0.3f, 0.3f, 0.3f, 1.0f})){
                global_show_leaderboard = true;
                global_show_menuboard = false;
        }
        
        if(Button((void *)&AppQuit, &im, global_UIRenderer,  Create_String("Quit"), 
            HMM_Vec2{menu_position.X + TILE_SIZE * 1, 
            menu_position.Y + menu_size.Y - TILE_SIZE * (y_pos + 4)}, 
                {0.3f, 0.3f, 0.3f, 1.0f})){

            // quit
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
    
#endif
}

void start(AppState *app_state){

    Resize_UpdatePositions();
    curr_pos = {start_pos.X + TILE_SIZE * 3, start_pos.Y + TILE_SIZE * (TILE_COUNT_Y - 2)};
    global_show_menuboard = true;
    ReadDataFile("data.dat");
    for(int x = 0; x < TILE_COUNT_X; x++){
        for(int y = 0; y < TILE_COUNT_Y; y++){
            global_tetris_board.tiles[x][y].color = TILE_CLR;
            global_tetris_board.tiles[x][y].border_clr = BORDER_CLR;
            global_tetris_board.tiles[x][y].size = {TILE_SIZE, TILE_SIZE};
        }
    }

    app_state->cam_pos = CAMERA_NORMAL_POSITION;
    gSoloud.init(); // Initialize SoLoud
    gWave.load("assets/Future-Technology.Wav"); // Load a wave
    // gSoloud.play(gWave); // Play the wave
    gWave.setLooping(1);

    global_wav_reached_down.load("assets/ImpactIntoSand.Wav");
    global_wav_phase.load("assets/Retro Block Hit.Wav");
    global_wav_move.load("assets/Click.Wav");

    global_UIRenderer->SetupTextRenderer(app_state->window_width, app_state->window_height);
    im.window = window;
}

void update(AppState *app_state, float dt){

#if 1 // Tetris Updates

    // Slide Timing

    if(mobileSliding){
        slideTime += dt;
    }

    // Iterate through all tiles and update ages
    for(int x = 0; x < TILE_COUNT_X; x++){
        for(int y = 0; y < TILE_COUNT_Y; y++){
            Tile *tile = &global_tetris_board.tiles[x][y];

            if(tile->taken){
                // increase age for block lock animation and cap age at 100.0f;
                if(tile->age >= 100.0f){
                    tile->age = 100.0f;
                    continue;
                }
                tile->age += 1 * dt;
            }else{
                tile->size.X = TILE_SIZE;
                tile->size.Y = TILE_SIZE;
            }
        }
    }

    // TODO: work on borders
    if(global_reached_down){
        // change the color of the bottom blocks
        for(int i = 0; i < 4; i++){
            HMM_Vec2 size = {TILE_SIZE, TILE_SIZE};
            HMM_Vec2 tile_cell_pos = curr_pos;
            tile_cell_pos.X += current_blk->structure[i].Elements[0] * size.X;
            tile_cell_pos.Y += current_blk->structure[i].Elements[1] * size.Y;

            HMM_Vec2 coord = GetBoardCoord(tile_cell_pos); 

            Tile *tile = &global_tetris_board.tiles[(int)coord.X][(int)coord.Y];
            tile->taken = true;
            tile->age = 0;
            tile->color = global_parent.color;
            tile->type = global_parent;
        }

        // Note (Lenny) : should the index persist?
        global_parent = GetNewParentBlock();
        current_blk = &global_parent.rotations[0];
        global_rotation_index = 0;


        // move to the top
        curr_pos.Y = start_pos.Y + TILE_SIZE * (TILE_COUNT_Y - 2);
        global_reached_down = 0;

        // check if spawned on top of a piece
        if(ReachedObstacle(curr_pos)){
            global_last_game_score = global_score;
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
        int h = gSoloud.play(global_wav_reached_down, 0.5f, 0.0, 1, 0);
        gSoloud.setPause(h, 0);
    }

    testSquarePos.Y = curr_pos.Y;

    // Tetris main calculations
    if(!global_game_over && !global_pause && !global_show_menuboard && !global_show_leaderboard){

        if(global_time_to_next_move <= 0){ // Move down
            if (global_time_to_clear_lines <= 0) { // all lines are cleared
                curr_pos.Y -= move_amount;

                if(global_phase_down){
                    global_time_to_next_move = PHASE_TIME;
                }else{
                    global_time_to_next_move = global_time_btw_moves;
                }
            } else if(global_time_to_clear_lines > 0) {
                global_time_to_clear_lines -= 4 * dt;
                // clearing with animation
                for(int i = 3; i >= 0; i--){
                    int full_line = global_lines_to_clear[i];

                    if(full_line < 0) // not a full line
                        continue;

                    for(int j = 0; j < TILE_COUNT_X; j++){
                        Tile *tile = &global_tetris_board.tiles[j][full_line];
                        tile->taken = false;
                        float percent_done = (float)j / TILE_COUNT_X;
                        float percent_done_prev = (float)TILE_COUNT_X / (float)(j - 1);
                        
                        float percent_time_left = (float)global_time_to_clear_lines / (float)global_lines_count_to_clear;

                        tile->color.r = tile->type.color.r * (percent_time_left * (4 - i)) * percent_time_left;
                        tile->color.g = tile->type.color.g * (percent_time_left * (4 - i)) * percent_time_left;
                        tile->color.b = tile->type.color.b * (percent_time_left * (4 - i)) * percent_time_left;
                    }

                }
                // global_lines_count_to_clear--;

                // shift down
                if(global_time_to_clear_lines <= 0){
                    for(int i = 3; i >= 0; i--){
                        int full_line = global_lines_to_clear[i];
                        if(full_line >= 0){

                            for(int j0 = full_line; j0 < TILE_COUNT_Y; j0++){
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
                }
            }
        }

        // swiping and such
        HMM_Vec2 cursorPos = im.cursorPos;

        PanelState panelState = Panel(&im.cursorPos, &im, global_UIRenderer, "assets/white_texture.jpg", 
            start_pos, {TILE_SIZE * TILE_COUNT_X, TILE_SIZE * TILE_COUNT_Y});

        if(panelState.hot){
            HMM_Vec2 oldPos = curr_pos;

            if(panelState.active){
                if(mobileSliding == false){
                    initialPiecePos = curr_pos;
                    slideTime = 0.0f;
                    mobileSliding = true;
                }

                HMM_Vec2 tileFixedPos = initialPiecePos;
                tileFixedPos.X += (int)((cursorPos.X - im.cursorClickPos.X) / TILE_SIZE) * TILE_SIZE;        
        
                curr_pos.X = tileFixedPos.X;
                bool out_of_bounds_left = false;
                bool out_of_bounds_right = false;

                for(int i = 0; i < 4; i++){
                    // Note (Lenny) : Unnecessary??
                    HMM_Vec2 tile_cell_pos = curr_pos;
                    tile_cell_pos.X += current_blk->structure[i].Elements[0] * TILE_SIZE;
                    tile_cell_pos.Y += current_blk->structure[i].Elements[1] * TILE_SIZE;

                    HMM_Vec2 coord = GetBoardCoord(tile_cell_pos); 

                    if(coord.X < 0){
                        out_of_bounds_left = true;
                    }

                    if((coord.X + 1) > TILE_COUNT_X){
                        out_of_bounds_right = true;
                    }
                }

                if(out_of_bounds_left || out_of_bounds_right){
                    curr_pos = oldPos;
                }
            }
        }

        if(panelState.clicked == 1){
            mobileSliding = false;
            int tilesMovedVertical = (cursorPos.Y - im.cursorClickPos.Y) / TILE_SIZE;
            int tilesMovedHorizontal = (cursorPos.X - im.cursorClickPos.X) / TILE_SIZE;

            if(tilesMovedVertical < 0 && slideTime < verticalSlideTime){
                move_tetromino(GLFW_KEY_SPACE); // Phase Down
            }

            if(slideTime < 0.5f && tilesMovedVertical > 0){
                move_tetromino(GLFW_KEY_C); // Store Tetromino
            }

            if(slideTime < 1.0f && tilesMovedVertical == 0 && tilesMovedHorizontal == 0){
                move_tetromino(GLFW_KEY_UP); // Rotate Tetromino
            }
        }
        

        global_time_to_next_move -= dt;
    }

    // compute if reached down
    if(global_game_over || global_show_menuboard){
        global_reached_down = 0;
    }else{
        global_reached_down = ReachedObstacle(curr_pos);
    }

    camera_shake(&app_state->cam_pos, dt);

    draw(app_state, dt);
    String score_str = Create_String("SCORE : ");
    AddToString(&score_str, global_score);
    
    String framet_str = Create_String("DT : ");
    String fps_str = Create_String("FPS : ");
    
    static float framet_val_avg_updated = dt; 
    static float framet_val_avg = dt; 
    static int framet_val_count = 1;
    static float time_to_update_fps = 0;

    static int fps_count_avg = 0;
    static int fps_count = 0;

    if(time_to_update_fps >= 1){
        framet_val_avg_updated = framet_val_avg;
        fps_count_avg = fps_count;

        time_to_update_fps = 0;
        framet_val_count = 0;
        framet_val_avg = 0;
        
        fps_count = 0;
    }else{
        framet_val_avg *= framet_val_count;
        framet_val_count++;
        framet_val_avg += dt;
        framet_val_avg /= framet_val_count;

        time_to_update_fps += dt;
        fps_count += 1;
    }
    AddToString(&framet_str, framet_val_avg_updated * 10 * 10 * 10);
    AddToString(&fps_str, fps_count_avg);
    
    HMM_Vec2 framet_pos = {start_pos.X + TILE_SIZE * (TILE_COUNT_X + 1), held_blck_pos.Y + TILE_SIZE * 2.5f};
    HMM_Vec2 fps_pos = {start_pos.X + TILE_SIZE * (TILE_COUNT_X + 1), held_blck_pos.Y + TILE_SIZE * 0.5f};

    global_UIRenderer->DrawText(framet_str, 0.5f, {framet_pos.X, framet_pos.Y}, 
        {200.0f, 200.0f, 200.0f});
        
    global_UIRenderer->DrawText(fps_str, 0.5f, {fps_pos.X, fps_pos.Y}, 
        {200.0f, 200.0f, 200.0f});

    HMM_Vec2 score_pos = {start_pos.X + TILE_SIZE * (TILE_COUNT_X + 1), held_blck_pos.Y + TILE_SIZE * 5.5f};
    // score_pos.X += (TILE_SIZE.)
    // start_pos.X + (TILE_SIZE / 2.0f) * TILE_COUNT_X - TILE_SIZE * 2

    global_UIRenderer->DrawText(score_str, 0.5f, {score_pos.X, score_pos.Y}, 
        {200.0f, 200.0f, 200.0f});
#endif

    glFinish();
}

};