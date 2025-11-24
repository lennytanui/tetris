#version 300 es
precision mediump float;

in vec4 fColor;
in vec2 fTexCoord;
in float fTexSlot;

uniform sampler2D uTexture_0; 
uniform sampler2D uTexture_1; 
uniform sampler2D uTexture_2; 
uniform sampler2D uTexture_3; 

out vec4 FragColor;

void main(){
    FragColor = vec4(fColor.xyz, 1.0);

    if(fTexSlot == 0.0){
        FragColor = fColor * texture(uTexture_0, fTexCoord); 
    }else{
        FragColor = fColor * texture(uTexture_1, fTexCoord); 
    }
}