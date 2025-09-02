#include <stdio.h>
#include <glad/glad.h>

#ifdef _WIN32
#include <GLFW/glfw3.h>
#else
#include <GLFW/emscripten_glfw3.h>
#include <emscripten/html5.h>
#endif

#include <fstream>
#include "renderer.cpp"
#include "app.h"
#include "app.cpp"

int global_running = 1;

float global_delta_time = 1.0f;
float global_start_time = 0.0f;

static void window_size_callback(GLFWwindow* window, int width, int height)
{
    global_window_width = width;
    global_window_height = height;

    Tetris::UpdateTRM();
}

// TODO: should use this viewport
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    global_frame_buffer_width = width;
    global_frame_buffer_height = height;
    glViewport(0, 0, global_frame_buffer_width, global_frame_buffer_height);
    
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

#if _WIN32
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

        case NOTES:{
            if((key == GLFW_KEY_BACKSPACE || key == GLFW_KEY_ENTER || key == GLFW_KEY_LEFT 
                || GLFW_KEY_RIGHT) 
                && (action == GLFW_PRESS || action == GLFW_REPEAT)){
                Notes::NotesKeyPress(key, action);
            }
        };

        default: break;
    }
#endif
}

void character_callback(GLFWwindow* window, unsigned int codePoint)
{
    if(currentApp == NOTES){
        Notes::ReceiveCharacter(codePoint);
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    SetCursorPosition((float)xpos, (float)ypos);
}

// Web setup with Emscripten
//! Display error message in the Console
void consoleErrorHandler(int iErrorCode, char const *iErrorMessage)
{
    printf("glfwError: %d | %s\n", iErrorCode, iErrorMessage);
}

#if GLFW_PLATFORM_EMSCRIPTEN
//! jsRenderFrame: for the sake of this example, uses the canvas2D api to change the color of the screen / display a message
EM_JS(void, jsRenderFrame, (GLFWwindow *glfwWindow, int w, int h, int fw, int fh, double mx, double my, int color, bool isFullscreen), {
    const ctx = Module['glfwGetCanvas'](glfwWindow).getContext('2d');
    ctx.fillStyle = `rgb(${color}, ${color}, ${color})`;
    ctx.fillRect(0, 0, fw, fh); // using framebuffer width/height
    const text = `${w}x${h} | ${mx}x${my} | CTRL+Q to terminate ${isFullscreen ? "" : '| CTRL+F for fullscreen'}`;
    ctx.font = '15px monospace';
    ctx.fillStyle = `rgb(${255 - color}, 0, 0)`;
    ctx.fillText(text, 10 + color, 20 + color);
})
#endif

#if GLFW_PLATFORM_EMSCRIPTEN
//! Called for each frame
void renderFrame(GLFWwindow *iWindow)
{
    static int frameCount = 0;

    // poll events
    glfwPollEvents();
    printf("Frame Count %i\n", frameCount);
    int w,h; glfwGetWindowSize(iWindow, &w, &h);
    int fw,fh; glfwGetFramebufferSize(iWindow, &fw, &fh);
    double mx,my; 
    glfwGetCursorPos(iWindow, &mx, &my);
    auto color = 127.0f + 127.0f * std::sin((float) frameCount++ / 120.f);
    jsRenderFrame(iWindow, w, h, fw, fh, mx, my, (int) color, emscripten_glfw_is_window_fullscreen(iWindow));
}
#endif

//! The main loop (called by emscripten for each frame)
void main_loop(void *)
{
    /* Loop until the user closes the window */
    float global_start_time = glfwGetTime();
    
#if 1
    /* Render here */
    glViewport(0, 0, global_window_width, global_window_height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 0.0f, 0.0f, 255.0f);
    printf("Hello world\n");
    RenderRectangles(&global_app_state, global_delta_time);
    app_update(&global_app_state, global_delta_time);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
#endif
    glfwPollEvents();
    global_delta_time = glfwGetTime() - global_start_time;

    if(glfwWindowShouldClose(window)){
        // done => terminating
        global_running = false;
    
        glfwTerminate();

        #if GLFW_PLATFORM_EMSCRIPTEN
            emscripten_cancel_main_loop();
        #endif
    }
    else
    {
        // not done => renderFrame
        printf("Rendering Frame\n");
        // renderFrame(window);

    }
}

int main(void) {
    printf("App Started..\n");

#if GLFW_PLATFORM_EMSCRIPTEN
    // set a callback for errors otherwise if there is a problem, we won't know
    glfwSetErrorCallback(consoleErrorHandler);

#endif

    printf("GLFW VERSION %s\n", glfwGetVersionString());

    if(!glfwInit()){
        printf("Failed to initialize GLFW \n");
        return -1;
    }

#if GLFW_PLATFORM_EMSCRIPTEN
    // print the Emscripten version on the console
    printf("emscripten: v%d.%d.%d\n", __EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__);

    // no OpenGL (use canvas2D)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // make it not Hi DPI Aware (simplify rendering code a bit)
    glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_FALSE);
    // setting the association window <-> canvas
    emscripten::glfw3::SetNextWindowCanvasSelector("#canvas");
    
    // setting the association window <-> canvas
    emscripten_glfw_set_next_window_canvas_selector("#canvas");

    window = glfwCreateWindow(320, 200, "example_minimal | emscripten-glfw", nullptr, nullptr);
#else

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(global_window_width, global_window_height, "Tetris", NULL, NULL);

#endif

    
    if (!window) {
        printf("Failed to Create Window \n");
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize OpenGL context \n");
        return -1;
    }
    
    #if _WIN32
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetCharCallback(window, character_callback);
    #endif
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    app_start(&global_app_state);
    
#if GLFW_PLATFORM_EMSCRIPTEN
    // makes the canvas resizable and match the full window size
    // makes the canvas resizable to the size of its div container
    emscripten::glfw3::MakeCanvasResizable(window, "#canvas-container");
    // tell emscripten to use "main_loop" as the main loop (window is user data)
    emscripten_set_main_loop_arg(main_loop, window, 0, GLFW_FALSE);
#else

#if _WIN32
    while (global_running) {
        main_loop(nullptr);
    }
    AppQuit(&global_app_state);
#endif

#endif 

    return 0;
}