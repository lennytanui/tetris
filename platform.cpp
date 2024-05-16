#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include "renderer.cpp"
#include "app.h"
#include "app.cpp"

static void window_size_callback(GLFWwindow* window, int width, int height)
{
    printf("Alert : Window Resized \n");
    window_width = width;
    window_height = height;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if ((key == GLFW_KEY_W || key == GLFW_KEY_A || 
        key == GLFW_KEY_S || key == GLFW_KEY_D || key == GLFW_KEY_SPACE ||
        key == GLFW_KEY_Z || key == GLFW_KEY_C || 
        key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS){           
            move_tetromino(key);
    }else if ((key == GLFW_KEY_UP || key == GLFW_KEY_LEFT || 
        key == GLFW_KEY_DOWN || key == GLFW_KEY_RIGHT || key == GLFW_KEY_SPACE ||
        key == GLFW_KEY_Z || key == GLFW_KEY_C || 
        key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS){           
            move_tetromino(key);
    }
}


static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    SetCursorPosition((float)xpos, (float)ypos);
}

int main(void) {

    if(!glfwInit()){
        printf("Failed to initialize GLFW \n");
        return -1;
    }

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(window_width, window_height, "Tetris", NULL, NULL);
    if (!window) {
        printf("Failed to Create Window \n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize OpenGL context \n");
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    AppState app_state = {};

    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    float delta_time = 1.0f;
    float start_time = 0.0f;

    app_start(&app_state);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        float start_time = glfwGetTime();
        /* Render here */
        glViewport(0, 0, window_width, window_height);

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0f, 0.0f, 0.0f, 255.0f);

        // glEnable(GL_BLEND);
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // glBlendEquation(GL_FUNC_ADD);

        RenderRectangles(&app_state, delta_time);
        app_update(&app_state, delta_time);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        delta_time = glfwGetTime() - start_time;
    }

    AppQuit(&app_state);

    glfwTerminate();
    return 0;
}