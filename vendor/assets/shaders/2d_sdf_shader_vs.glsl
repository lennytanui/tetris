#version 300 es
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec4 aColor; 
layout (location = 2) in vec4 aBorderClr;

uniform mat4 uViewMatrix;  
uniform mat4 uProjectionMatrix; 
  
out vec4 color; 
out vec4 borderClr; 
out float texSlot;
out vec2 pos;

void main() { 
    pos = vec2(100.0, 100.0);

    gl_Position = uProjectionMatrix * uViewMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0); 
    color = aColor; 
    borderClr = aBorderClr; 
}