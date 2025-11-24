#version 300 es 
precision mediump float;

in vec2 fTexCoord; 
in vec4 fColor; 
in float fTexSlot;

uniform sampler2D texture_0;
uniform sampler2D texture_1;

out vec4 FragColor; 
void main(){ 
    FragColor = aColor * texture(texture_0, fTexCoord); 
}