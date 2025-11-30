#include <stdio.h>

#if WIN32
#include <glad/glad.h>
#endif

#ifdef _WIN32
#include <GLFW/glfw3.h>
#else
#include <emscripten/html5.h>
#include <GLFW/emscripten_glfw3.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#endif

#include <fstream>
#include "renderer.cpp"
#include "app.h"
#include "app.cpp"

int global_running = 1;
float global_delta_time = 1.0f;

static void window_size_callback(GLFWwindow* window, int width, int height)
{
    global_window_width = width;
    global_window_height = height;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    global_frame_buffer_width = width;
    global_frame_buffer_height = height;
    glViewport(0, 0, global_frame_buffer_width, global_frame_buffer_height);
    
    global_ortho_width = 1000.0f;
    global_ortho_height = global_ortho_width;
    global_ortho_height *= ((float)global_frame_buffer_height / (float)global_frame_buffer_width);
    Tetris::UpdateDimensions();
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
#if GLFW_PLATFORM_EMSCRIPTEN
    // set callback for exit (CTRL+Q) and fullscreen (CTRL+F)
    if(action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
        switch(key) {
        case GLFW_KEY_Q: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
        case GLFW_KEY_F: emscripten_glfw_request_fullscreen(window, false, true); break; // ok from a keyboard event
        default: break;
        }
    }
#endif

    switch(currentApp){
        case TETRIS: {

            if ((key == GLFW_KEY_W || key == GLFW_KEY_A || 
                key == GLFW_KEY_S || key == GLFW_KEY_D || key == GLFW_KEY_SPACE ||
                key == GLFW_KEY_Z || key == GLFW_KEY_C || 
                key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS){           
                    Tetris::move_tetromino(key);
            }else if ((key == GLFW_KEY_UP || key == GLFW_KEY_LEFT || 
                key == GLFW_KEY_DOWN || key == GLFW_KEY_RIGHT || key == GLFW_KEY_SPACE ||
                key == GLFW_KEY_Z || key == GLFW_KEY_C || 
                key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS){           
                    Tetris::move_tetromino(key);
            }

            break;
        };

        default: break;
    }
}

void character_callback(GLFWwindow* window, unsigned int codePoint)
{
    
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    double fbX = xpos * ((double)global_frame_buffer_width / global_window_width);
    double fbY = ypos * ((double)global_frame_buffer_height / global_window_height);
    
    SetCursorPosition((float)fbX, (float)fbY);
}

// Web setup with Emscripten
//! Display error message in the Console
void consoleErrorHandler(int iErrorCode, char const *iErrorMessage)
{
    printf("GLFW Error: %i | %s\n", iErrorCode, iErrorMessage);
}

static float global_last_frame_end_time = 0.0f;
// Main loop (called by emscripten for each frame)
void main_loop(){
    glfwMakeContextCurrent(window);
    // Loop until the user closes the window
    // Poll for and process events
    glfwPollEvents();
    

#if GLFW_PLATFORM_EMSCRIPTEN
    int w,h; 
    glfwGetWindowSize(window, &w, &h);
    int fw,fh; 
    glfwGetFramebufferSize(window, &fw, &fh);
#endif

    // Render Section
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 0.0f, 0.0f, 255.0f);
    app_update(&global_app_state, global_delta_time);
    glfwSwapBuffers(window);

    if(glfwWindowShouldClose(window)){
        // terminate program
        global_running = false;
        
        glfwTerminate();
        
        #if GLFW_PLATFORM_EMSCRIPTEN
        emscripten_cancel_main_loop();
        #endif
    }

    global_delta_time = glfwGetTime() - global_last_frame_end_time;
    global_last_frame_end_time = glfwGetTime();
}

int main(void) {
    printf("App Setup Start..\n");

#if GLFW_PLATFORM_EMSCRIPTEN
    // set a callback for errors otherwise if there is a problem, we won't know
    glfwSetErrorCallback(consoleErrorHandler);
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_EMSCRIPTEN);
#endif

    printf("GLFW VERSION %s\n", glfwGetVersionString());

    if(!glfwInit()){
        printf("Failed to initialize GLFW \n");
        return -1;
    }
    
#if GLFW_PLATFORM_EMSCRIPTEN
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
    // make it not Hi DPI Aware (simplify rendering code a bit)
    glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_TRUE);

    global_window_width = 600;
    global_window_height = 600;

    printf("Frame Size  [%i, %i]\n", global_window_width, global_window_height);

    // print the Emscripten version on the console
    printf("emscripten: v%d.%d.%d\n", __EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__);
    
    // setting the association window <-> canvas
    emscripten_glfw_set_next_window_canvas_selector("#canvas");

    window = glfwCreateWindow(global_window_width, global_window_height, "Lennys Canvas", nullptr, nullptr);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(global_window_width, global_window_height, "Lennys Canvas", NULL, NULL);

#endif
    
    if (!window) {
        printf("Failed to Create Window \n");
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

#if GLFW_PLATFORM_EMSCRIPTEN
    printf("GL ES VERSION %s\n", glGetString(GL_VERSION));
#endif

#if _WIN32
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize OpenGL context \n");
        return -1;
    }
#endif

    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetCharCallback(window, character_callback);
    glfwWindowHint(GLFW_SAMPLES, 0); // Disable multisampling if not needed
    
    app_start(&global_app_state);
    Tetris::UpdateDimensions();

#if GLFW_PLATFORM_EMSCRIPTEN
    // makes the canvas resizable and match the full window size
    emscripten_glfw_make_canvas_resizable(window, "#canvas-container", nullptr);
    int w,h; 
    glfwGetWindowSize(window, &w, &h);
    int fw,fh; 
    glfwGetFramebufferSize(window, &fw, &fh);

    global_frame_buffer_width = fw;
    global_frame_buffer_height = fh;

    // tell emscripten to use "main_loop" as the main loop (window is user data)
    emscripten_set_main_loop(main_loop, 0, false);
#endif

#if _WIN32
    while (global_running) {
        main_loop();
    }
    AppQuit(&global_app_state);
#endif

    return 0;
}