#version 450 core

layout (location = 0) out vec4 o_Color;

in vec3 position;
in vec3 normal;
in vec3 color;

void main(){
    o_Color = vec4(normal, 1.0f);
}
