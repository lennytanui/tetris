#version 300 es 
layout (location = 0) in vec4 aPos; 
layout (location = 1) in vec4 aColor; 
layout (location = 2) in vec2 aTexCoord; 
layout (location = 3) in vec4 a_border_clr; 

uniform mat4 u_view;  
uniform mat4 u_projection; 

// varying vec2 pos_; 
out vec2 pos_; 
out vec2 texCoord; 
out vec4 color; 
out vec4 border_clr; 
void main() { 
    pos_ = aTexCoord;
    gl_Position = u_projection * u_view * vec4(aPos.x, aPos.y, aPos.z, aPos.w); 
    texCoord = aTexCoord; 
    color = aColor; 
    border_clr = a_border_clr; 
}