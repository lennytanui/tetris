#version 300 es 
precision mediump float;

// in vec2 fTexCoord; 
// in vec4 fColor; 
// in float fTexSlot;

in vec2 texCoord; 
in vec4 color; 
in vec4 border_clr; 
in vec2 pos_;
in float texSlot;

uniform sampler2D texture_0;
uniform sampler2D texture_1;

out vec4 FragColor; 
void main(){ 
    // FragColor = aColor * texture(texture_0, fTexCoord); 
    vec4 clr = color; 

    if (pos_.x <= 0.1 || pos_.x >= 0.9){ 
        clr = border_clr; 
    } else if (pos_.y <= 0.1 || pos_.y >= 0.9) { 
        clr = border_clr; 
    } 

    if(texSlot == 0.0){
        FragColor = clr * texture(texture_0, texCoord); 
    }else{
        FragColor = clr * texture(texture_1, texCoord); 
    }
}