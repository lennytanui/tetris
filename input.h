#pragma once
struct InputManager{
    float dt;
    HMM_Vec2 cursorPos;
    GLFWwindow *window;

    HMM_Vec2 cursorClickPos;
    void *active_ui;
    void *hot_ui;

    /** layouting
        does not allow for panels inside panels yet
    */ 
    
    HMM_Vec2 parent_pos;
};