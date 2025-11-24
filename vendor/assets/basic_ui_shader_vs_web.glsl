#version 300 es
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float aTexSlot;

out vec4 fColor;
out vec2 fTexCoord;
out float fTexSlot;

uniform mat4 uProjectionMatrix;

void main(){
    gl_Position = uProjectionMatrix * vec4(aPosition, 1.0, 1.0);
    
    fColor = aColor;
    fTexCoord = aTexCoord;
    fTexSlot = aTexSlot;
}