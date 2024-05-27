//This file was automatically generated 
//DO NOT CHANGE !
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;

void main()
{
    //vec3 pos = aPos;
    //pos.y *= -1;
    gl_Position = model * vec4(aPos, 1.0);
}  