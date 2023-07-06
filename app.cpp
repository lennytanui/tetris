#pragma once
#include <soloud/soloud.h>
#include <soloud/soloud_wav.h>

#include "app.h"
#include "renderer.cpp"
#include "shapes.cpp"
#include "ui.cpp"

/**
    TODO: make it so the tiles do not move into other tiles when
        moving right and left
    check for new rows completed after a row everything shifts
        down
    create scoring system
    add options for 4 different resolutions including
        full screen


    write the setup documentation
*/

#define TILE_COUNT_X 10
#define TILE_COUNT_Y 20

#define TILE_SIZE 30.0f
#define BORDER_CLR {72.0f, 79.0f, 72.0f, 255.0f}
#define TILE_CLR {16.0f, 31.0f, 17.0f, 255.0f}

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

Tetris_Board global_tetris_board = {};

Block_Info global_parent = SHAPE_zigzag2;
Child_Block *current_blk = &global_parent.rotations[0];
int parent_blk_index = 0;

v2 curr_pos = {TILE_SIZE * 5, TILE_SIZE * 7};
float move_amount = TILE_SIZE;
v2 start_pos = {TILE_SIZE * 2, TILE_SIZE * 1};
int reached_down = 0;
float score = 0;
float score_per_line = 5;
int global_rotation_index = 0;

SoLoud::Soloud gSoloud; // SoLoud engine
SoLoud::Wav gWave;      // One wave file
SoLoud::Wav global_wav_reached_down;
SoLoud::Wav global_wav_move;
SoLoud::Wav global_wav_phase;

bool global_phase_down = false;

#define TIME_BTW_MOVES 0.5f;

#define PHASE_TIME 0.01f;
float time_to_next_move = TIME_BTW_MOVES;

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
    im.cursorX = x;
    im.cursorY = y;
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


    for(int i = 0; i < 4; i++){
        if(full_lines[i] != 0){
            score += score_per_line;
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
}

v2 GetBoardCoord(v2 position){
    v2 result = {};

    result.x = (float)(int)(position.x - start_pos.x) / TILE_SIZE;
    result.y = (float)(int)(position.y - start_pos.y) / TILE_SIZE;

    return result;
}

Block_Info GetNewParentBlock(){
    int index = 0;
    Block_Info result = {};

    if(parent_blk_index > 6){
        parent_blk_index = 0;
    }
    result = blocks_table[parent_blk_index++];
    
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
                gSoloud.play(global_wav_move);
            }
            break;
        }
        
        case GLFW_KEY_A:{
            if(!out_of_bounds_left){
                curr_pos.x -= move_amount;
                gSoloud.play(global_wav_move);
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
    }
}

void draw(AppState *app_state){
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
    for(int i = 0; i < 4; i++){
        v2 tile_pos = {
            curr_pos.x + current_blk->structure[i].x * TILE_SIZE,
            curr_pos.y + current_blk->structure[i].y * TILE_SIZE
        };

        Render_Square *background = create_render_square(app_state,
                {tile_pos.x, tile_pos.y}, {TILE_SIZE, TILE_SIZE}, 
                    global_parent.color, BORDER_CLR);
    }

    Render_Square *render_square = create_render_square(app_state,
            {500.0f, 100.0f}, {100.0f, 100.0f}, {0.0f, 255.0f, 0.0f, 255.0f}, {255.0f, 0.0f, 0.0f, 255.0f});
}

void app_start(AppState *app_state){
    for(int x = 0; x < TILE_COUNT_X; x++){
        for(int y = 0; y < TILE_COUNT_Y; y++){
            global_tetris_board.tiles[x][y].color = TILE_CLR;
            global_tetris_board.tiles[x][y].border_clr = BORDER_CLR;
        }
    }

    app_state->cam_pos = CAMERA_NORMAL_POSITION;
    gSoloud.init(); // Initialize SoLoud
    gWave.load("assets/Future-Technology.wav"); // Load a wave
    gSoloud.play(gWave); // Play the wave
    gWave.setLooping(1);

    global_wav_reached_down.load("assets/ImpactIntoSand.wav");
    global_wav_phase.load("assets/Retro Block Hit.wav");
    global_wav_move.load("assets/Click.wav");

    SetupTextRenderer(&trm);
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
        curr_pos.y = start_pos.y + TILE_SIZE * (TILE_COUNT_Y - 4);
        reached_down = 0;

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
    if(time_to_next_move <= 0){
        curr_pos.y -= move_amount;

        if(!global_phase_down){
            time_to_next_move = TIME_BTW_MOVES;
        }else{
            gSoloud.play(global_wav_phase);
            time_to_next_move = PHASE_TIME;
        }

    }

    time_to_next_move -= dt;

    // compute if reached down
    for(int i = 0; i < 4; i++){
        v2 structure = current_blk->structure[i];
        v2 coord = GetBoardCoord({structure.x * TILE_SIZE + curr_pos.x, structure.y * TILE_SIZE + curr_pos.y - 1});

        if(coord.y < 0){
            reached_down = true;
            break;
        }

        Tile *tile = &global_tetris_board.tiles[(int)coord.x][(int)coord.y];
        if(tile->taken){
            reached_down = true;
            break;
        }
    }

    camera_shake(&app_state->cam_pos, dt);


    draw(app_state);
    DrawText(&trm, Create_String("SCORE : "), 1.0f, 
        {TILE_SIZE * TILE_COUNT_X + 50.0f, window_height - DEFAULT_TEXT_PIXEL_HEIGHT - 20.0f}, 
        {0.0f, 0.0f, 255.0f});
    
    Button(app_update, &im, &trm,  Create_String("Quit"), 
        HMM_Vec2{400.0f, 200.0f}, {0.3f, 0.3f, 0.3f, 1.0f});
}