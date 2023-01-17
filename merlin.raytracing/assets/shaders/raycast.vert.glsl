#version 460 core

layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 _normal;
layout (location = 2) in vec3 _color;
layout (location = 3) in vec2 _texCoord;

uniform mat4 view;
uniform mat4 model;

out vec3 position;
out vec3 offset;
out vec3 normal;
out vec3 color;
out vec2 texCoord;

uniform vec3 start;
uniform vec3 end;

void main() {
	if(length(_position) == 0.0){
		position = start;
	}else{
		position = end;
	}
	color = vec3(1);
	gl_Position = view * vec4(position, 1.0f);
} 
