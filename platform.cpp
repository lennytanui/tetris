#include <stdio.h>
#include <glad/glad.h>

#ifdef _WIN32
#include <GLFW/glfw3.h>
#else
#include <GLFW/emscripten_glfw3.h>
#include <emscripten/html5.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
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
    
    Tetris::UpdateDimensions();
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
}

void character_callback(GLFWwindow* window, unsigned int codePoint)
{
    if(currentApp == NOTES){
        Notes::ReceiveCharacter(codePoint);
    }
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
    /* Poll for and process events */
    glfwPollEvents();
    
    static int frameCount = 0;

#if GLFW_PLATFORM_EMSCRIPTEN
    int w,h; 
    glfwGetWindowSize(window, &w, &h);
    int fw,fh; 
    glfwGetFramebufferSize(window, &fw, &fh);

    // printf("Window Size [%i, %i]\n", w, h);
    // printf("Frame Size  [%i, %i]\n", global_window_width, global_window_height);
    
    // double mx,my; 
    // glfwGetCursorPos(window, &mx, &my);
    
    // double fbX = mx * ((double)fw / w);
    // double fbY = my * ((double)fh / h);
    // SetCursorPosition((float)mx, (float)my);
    // printf("Cursor Pos  [%f, %f]\n", fbX, fbY);
#endif

    /* Render here */
    // glViewport(0, 0, global_frame_buffer_width, global_frame_buffer_height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 0.0f, 0.0f, 255.0f);
    RenderRectangles(&global_app_state, global_delta_time);
    app_update(&global_app_state, global_delta_time);

#if _WIN32
    /* Swap front and back buffers */
    glfwSwapBuffers(window);
#endif
    // printf("updating!\n");
    
    if(glfwWindowShouldClose(window)){
        // done => terminating
        global_running = false;
        
        glfwTerminate();
        
        #if GLFW_PLATFORM_EMSCRIPTEN
        emscripten_cancel_main_loop();
        #endif
    }
    printf("DT: %f\n", global_delta_time * 1000);
    global_delta_time = glfwGetTime() - global_start_time;
}

int main(void) {
    printf("App Setup Start..\n");

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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    global_window_width = 800;
    global_window_height = 800;

    printf("Frame Size  [%i, %i]\n", global_window_width, global_window_height);

    // print the Emscripten version on the console
    printf("emscripten: v%d.%d.%d\n", __EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__);
    
    // make it not Hi DPI Aware (simplify rendering code a bit)
    glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_TRUE);
    // setting the association window <-> canvas
    emscripten::glfw3::SetNextWindowCanvasSelector("#canvas");
    
    // setting the association window <-> canvas
    emscripten_glfw_set_next_window_canvas_selector("#canvas");

    window = glfwCreateWindow(global_window_width, global_window_height, "Tetris", nullptr, nullptr);
#else
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
    

#if GLFW_PLATFORM_EMSCRIPTEN

    if (!gladLoadGLES2Loader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize OpenGL context \n");
        return -1;
    }

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

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    app_start(&global_app_state);
    
#if GLFW_PLATFORM_EMSCRIPTEN
    // makes the canvas resizable and match the full window size
    // makes the canvas resizable to the size of its div container
    emscripten_glfw_make_canvas_resizable(window, "#canvas-container", nullptr);
    int w,h; 
    glfwGetWindowSize(window, &w, &h);
    int fw,fh; 
    glfwGetFramebufferSize(window, &fw, &fh);

    global_frame_buffer_width = fw;
    global_frame_buffer_height = fh;
    Tetris::UpdateDimensions();

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