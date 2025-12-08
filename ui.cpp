#ifndef UI_H
#include "text.cpp"
#include "shader.cpp"
#include <GLFW/glfw3.h>
#include "input.h"
#include "renderer.h"

/** Note (Lenny) : IMGUI by Casey Muratori Notes

Traditionally, callbacks and mesages procs are get messy, because data 
 is too intermingled and wrangled.

In IMGUI, there is no state, and functions like immidiate mode graphics
 it gets rid of redundant state
  unified flow control, meaning no multiple paths to accomplish tasks. 
    no callbacks!
 in a ui element, store the minimum amount of data required to interact
  with said element
 do not have to enable, disable buttons, or make any true effort to hide
  the UI. simply just don't call the button 
 

if(do...){
    // action code
}

if / for / while{
    do(...){
    }
}

-- UI_Context struct --

struct ui_id{ // some method of identifying item
    // could just be function pointer
    int owner; int item; int index;
}

ui_id hot; // about to be interacting with item (ex: mouse hover)
ui_id active; // interacting with item (ex: mouse click)

// example implementation code
bool DoButton(UI_id, text, pos, ...) {
    if (active) {
        if (MouseWEntUp){
            if(hot) result == true;
            SetNotActive;
        } else if (hot)
            if (MouseWentDown) SetActive;

        if (inside) SetHot
    }

    // draw button
}

// to cater to introspection, use the idea of render pass and 
 layout pass or just accept one frame of lag


*/
// #define BUTTON_NORMAL_COLOR {20.0f / 255.0f, 12.0f / 255.0f, 12.0f / 255.0f, 1.0f} // #0c0c0c
#define BUTTON_NORMAL_COLOR {20.0f / 255.0f, 122.0f / 255.0f, 72.0f / 255.0f, 1.0f} // #0c0c0c
// #define BUTTON_NORMAL_COLOR {12.0f / 255.0f, 12.0f / 255.0f, 12.0f / 255.0f, 1.0f} // #0c0c0c
#define BUTTON_HOT_COLOR {47.0f / 255.0f, 36.0f / 255.0f, 36.0f / 255.0f, 1.0f} // #252424
#define BUTTON_ACTIVE_COLOR {82.0f / 255.0f, 60.0f / 255.0f, 60.0f / 255.0f, 1.0f} // #3e3c3c


Shader2 basic_2d_shader = {};
unsigned int vao2d = 0;
unsigned int vbo2d = 0;

void SetActive(InputManager *im, void *active_ui){
    im->cursorClickPos = im->cursorPos;
    im->active_ui = active_ui;
}

void SetNotActive(InputManager *im){
    im->active_ui = 0;
}

void SetHot(InputManager *im, void *hot_ui){
    im->hot_ui = hot_ui;
}

void SetNotHot(InputManager *im){
    im->hot_ui = 0;
}

UIRenderer::UIRenderer(TextureManager *textureManager): m_textureManager{textureManager}{

    basic_2d_shader.program = LoadShaders("assets/basic_ui_shader_vs_web.glsl", "assets/basic_ui_shader_fs_web.glsl");
    glUseProgram(basic_2d_shader.program);
    
    unsigned int u_projection_matrix = GetUniformLocation(&basic_2d_shader, "uProjectionMatrix");
    SetUniformValue(u_projection_matrix, m_projection_ortho);
    
    unsigned int u_resolution = GetUniformLocation(&basic_2d_shader, "u_resolution");
    SetUniformValue(u_resolution, HMM_Vec2{global_ortho_width, global_ortho_height});
    
    glGenVertexArrays(1, &vao2d);
    glGenBuffers(1, &vbo2d);
    glBindVertexArray(vao2d);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2d);

    const unsigned int stride = 9;
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * stride * 6, NULL,  GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), 0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride * sizeof(float), (GLvoid*)(2 * sizeof(float)));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (GLvoid*)(6 * sizeof(float)));
    
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride * sizeof(float), (GLvoid*)(8 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(0);
}

void UIRenderer::DrawRect(HMM_Vec2 pos, float width, float height, RGBA color, const char *texturePath){
    // draw rect
    glUseProgram(basic_2d_shader.program);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glDisable(GL_DEPTH_TEST);
    
    unsigned int u_projection_matrix = GetUniformLocation(&basic_2d_shader, "uProjectionMatrix");
    SetUniformValue(u_projection_matrix, m_projection_ortho);
    
    glBindVertexArray(vao2d);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureManager->GetTexture(texturePath));
    
    float outline_width = 1.0f;
    HMM_Vec2 center_pos = {pos.X + width / 2.0f, pos.Y + height / 2.0f};
    const unsigned int stride = 9;
    float texSlot = 1.0f;
    float vertices[6][stride] = {
        {pos.X, pos.Y, color.r, color.g, color.b, color.a, 
            0.0f, 0.0f, texSlot},
        {pos.X + width, pos.Y, color.r, color.g, color.b, color.a, 
            1.0f, 0.0f, texSlot},
        {pos.X + width, pos.Y + height, color.r, color.g, color.b, color.a, 
            1.0f, 1.0f, texSlot},

        {pos.X + width, pos.Y + height, color.r, color.g, color.b, color.a, 
            1.0f, 1.0f, texSlot},
        {pos.X, pos.Y + height, color.r, color.g, color.b, color.a, 
            0.0f, 1.0f, texSlot},
        {pos.X, pos.Y, color.r, color.g, color.b, color.a, 
            0.0f, 0.0f, texSlot},
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo2d);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

// NOTE (Lenny) : ui will be draw relative to the bottom left of this panel
unsigned int UI_Begin(InputManager *im, HMM_Vec2 pos){

    im->parent_pos = pos;

    return true;
}

unsigned int UI_End(InputManager *im){

    im->parent_pos = {};
    return true;
}

#define BUTTON_PADDING 5.0f;
unsigned int Button(void *id, InputManager *im, UIRenderer *renderer, std::string label, HMM_Vec2 pos, RGBA color){

    int result = 0;
    float scale = 1.0f; // should be passed in 
    
    Character tallest = {};
    char c = '0';
    float textWidth = 0.0f;

    for(auto it = label.begin(); it != label.end(); ++it){
        c = *it;
        // Note (Lenny) : should have variable for active character table
        Character ch = renderer->GetCharacterTable()[0].characters[c];
        if(ch.size.Y > tallest.size.Y){
            tallest = ch;
        }

        textWidth += (ch.advance >> 6) * scale;
    }

    pos += im->parent_pos;
    // float textXPos = xpos;
    HMM_Vec2 textPos = pos;
    float width = textWidth;
    float height = 48;

    int state = glfwGetMouseButton(im->window, GLFW_MOUSE_BUTTON_LEFT);
    
    if(im->cursorPos.X >= pos.X && im->cursorPos.X <= pos.X + width 
        && (im->cursorPos.Y) >= pos.Y && (im->cursorPos.Y) <= pos.Y + height){
        if(!im->active_ui){
            SetHot(im, id);
    
            if(state == GLFW_PRESS){
                SetActive(im, id);
            }

        }else{
            
            if(im->active_ui == id){
                if (state == GLFW_RELEASE){
                    SetNotActive(im);
                    result = true;
                }
            }
        }
        
    } else {
        if(state == GLFW_RELEASE){
            if(im->active_ui == id){
                SetNotActive(im);
            }

            SetNotHot(im);
        }
    }

    color = BUTTON_NORMAL_COLOR;
    if(im->hot_ui == id){
        color = BUTTON_HOT_COLOR;
    }

    if(im->active_ui == id){
        color = BUTTON_ACTIVE_COLOR;
    }

    textPos.Y = pos.Y + (height / 2.0f) - (tallest.size.Y / 2.0f);

    renderer->DrawRect(pos, width, height, color, "assets/white_texture.jpg");    

    unsigned int error = glGetError();
    Text text = {};
    text.string = std::string(label.c_str());
    
    text.oneLine = true;

    renderer->RenderText(text, scale, HMM_Vec3{255.0f / 255.0f, 231.0f / 255.0f, 147.0f / 255.0f}, HMM_Vec2{textPos.X, textPos.Y});
    error = glGetError();
    return result;
}

// Button With Image Background
unsigned int Button(void *id, InputManager *im, UIRenderer *renderer, const char *background_image, HMM_Vec2 pos, HMM_Vec2 size){

    int result = 0;
    int state = glfwGetMouseButton(im->window, GLFW_MOUSE_BUTTON_LEFT);
    
    if(im->cursorPos.X >= pos.X && im->cursorPos.X <= pos.X + size.X 
        && (im->cursorPos.Y) >= pos.Y && (im->cursorPos.Y) <= pos.Y + size.Y){
        if(!im->active_ui){
            SetHot(im, id);
    
            if(state == GLFW_PRESS){
                SetActive(im, id);
            }

        }else{
            
            if(im->active_ui == id){
                if (state == GLFW_RELEASE){
                    SetNotActive(im);
                    result = true;
                }
            }
        }
        
    } else {
        if(state == GLFW_RELEASE){
            if(im->active_ui == id){
                SetNotActive(im);
            }

            SetNotHot(im);
        }
    }

    RGBA color = BUTTON_NORMAL_COLOR;
    if(im->hot_ui == id){
        color = BUTTON_HOT_COLOR;
    }

    if(im->active_ui == id){
        color = BUTTON_ACTIVE_COLOR;
    }

    renderer->DrawRect(pos, size.X, size.Y, color, background_image);

    unsigned int error = glGetError();
    error = glGetError();
    return result;
}

struct PanelState{
    unsigned int hot;
    unsigned int active;
    unsigned int clicked;
};

// Panel
PanelState Panel(void *id, InputManager *im, UIRenderer *renderer, const char *background_image, HMM_Vec2 pos, HMM_Vec2 size){

    PanelState result = {0};
    int state = glfwGetMouseButton(im->window, GLFW_MOUSE_BUTTON_LEFT);
    
    if(im->cursorPos.X >= pos.X && im->cursorPos.X <= pos.X + size.X 
        && (im->cursorPos.Y) >= pos.Y && (im->cursorPos.Y) <= pos.Y + size.Y){
        if(!im->active_ui){
            SetHot(im, id);
            result.hot = true;
            if(state == GLFW_PRESS){
                SetActive(im, id);
            }

        }else{
            
            if(im->active_ui == id){
                if (state == GLFW_RELEASE){
                    SetNotActive(im);
                    result.clicked = true;
                    result.active = false;
                }
            }
        }
        
    } else {
        if(state == GLFW_RELEASE){
            if(im->active_ui == id){
                SetNotActive(im);
                result.active = false;
            }

            SetNotHot(im);
            result.hot = false;
        }
    }

    if(im->hot_ui == id){
        result.hot = true;
    }

    if(im->active_ui == id){
        result.active = true;
    }

    RGBA color = RGBA{0.1f, 0.0f, 0.0f, 0.0f};
    // if(im->hot_ui == id){
    //     color = BUTTON_HOT_COLOR;
    // }

    // if(im->active_ui == id){
    //     color = BUTTON_ACTIVE_COLOR;
    // }

    // renderer->DrawRect(pos, size.X, size.Y, color, background_image);
    
    return result;
}


// Scrollable Panel Attributes
struct ScrollablePanelState{
    PanelState panelState;
    
    HMM_Vec2 pos;
    HMM_Vec2 size;

    bool scrolling; // true when scrolling
    float scrollTime;
    float initialOffset;
    float offset = 0.0f;
    float scrollHeight;
    float initialScrollHeight;

    int start = 0;
};
void ScrollablePanel(void *id, InputManager *im, UIRenderer *renderer, const char *background_image, ScrollablePanelState *sps){
    sps->panelState = Panel(id, im, renderer, background_image, sps->pos, sps->size);

    if(sps->panelState.hot){
        if(sps->panelState.active){
            if(sps->scrolling == false){
                sps->initialOffset = sps->offset;
                sps->initialScrollHeight = sps->scrollHeight;
                sps->scrolling = true;
                sps->scrollTime = 0.0f;
            }

            sps->scrollTime += im->dt;
            // printf("Cursor Change %f\n", (im->cursorClickPos.Y - im->cursorPos.Y));
            // printf("Cursor Click Pos {%f, %f}\n", im->cursorClickPos.X, im->cursorClickPos.Y);
            
            sps->scrollHeight = sps->initialScrollHeight + (im->cursorClickPos.Y - im->cursorPos.Y);
            if(sps->scrollHeight < 0){
                sps->scrollHeight = 0;
            }
            sps->offset = sps->initialOffset + (im->cursorPos.Y - im->cursorClickPos.Y);
        }
    }

    if(!sps->panelState.active){
        if(sps->scrolling){
            sps->scrolling = false;
        }
    }
}

unsigned int DrawUIText(UIRenderer *renderer, InputManager *im, float scale, std::string label, HMM_Vec2 pos,  HMM_Vec3 color){
    pos += im->parent_pos;
    pos.Y += 48 * scale;

    Text text = {0};
    text.string = label;
    text.oneLine = true;

    renderer->RenderText(text, scale, color / 255.0f, pos);
    return true;
}

#define UI_H
#endif