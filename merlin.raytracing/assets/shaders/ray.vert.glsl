#version 460 core

struct Ray {
	vec3 o; //Origin
	vec3 d; //Direction
	int hitID;
	int bounce;
};

layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 _normal;
layout (location = 2) in vec3 _color;
layout (location = 3) in vec2 _texCoord;

layout (std430, binding = 1) buffer RayBuffer {
  Ray rays[];
};

uniform mat4 view;
uniform mat4 model;

out vec3 position;
out vec3 offset;
out vec3 normal;
out vec3 color;
out vec2 texCoord;


void main() {
	offset = vec3(rays[gl_InstanceID].o);
	position = vec3(model * vec4((_position) + offset, 1.0f) );
	color = vec3(rays[gl_InstanceID].d);
	gl_Position = view * vec4(position, 1.0f);
}
