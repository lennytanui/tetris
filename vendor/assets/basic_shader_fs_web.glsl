#version 300 es 
precision mediump float;

in vec2 texCoord; 
in vec4 color; 
in vec4 border_clr; 
in vec2 pos_; 

uniform sampler2D testTexture; 

out vec4 FragColor; 
void main(){ 
    vec4 clr = color; 

    if (pos_.x <= 0.1f || pos_.x >= 0.9f){ 
        clr = border_clr; 
    } else if (pos_.y <= 0.1f || pos_.y >= 0.9f) { 
        clr = border_clr; 
    } 

    FragColor = clr *  
    texture(testTexture, texCoord); 
}