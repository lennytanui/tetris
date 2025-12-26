#version 300 es
precision mediump float;

in vec4 color; 
in vec4 borderClr; 
in vec2 pos;

out vec4 FragColor; 

void main(){ 
    vec4 clr = color; 

    FragColor = clr; 
}