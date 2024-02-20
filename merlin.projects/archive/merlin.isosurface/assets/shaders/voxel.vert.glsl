#version 450

struct Node {
    vec3 U;		//Position
	vec3 V;		//Velocity

	int enable;	//particle is deactivated
	uint index;
};

layout (std430, binding = 1) buffer NodeBuffer {
  Node nodes[];
};

layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 _normal;
layout (location = 2) in vec3 _color;
layout (location = 3) in vec2 _texCoord;

out vec3 position;
out vec3 offset;
out vec3 normal;
out vec3 color;
out vec2 texCoord;
out float opacity;

uniform mat4 view;
uniform mat4 model;
uniform float scale;
uniform int mode;



void main() {
	position = vec3(model * vec4((_position + nodes[gl_InstanceID].U)*scale, 1.0f));
	if(nodes[gl_InstanceID].enable == 1) color = normalize(position);
	//if(nodes[gl_InstanceID].enable == 1) color = vec3(0.6);
	if(nodes[gl_InstanceID].enable == 0) color = vec3(0.1);

	opacity = nodes[gl_InstanceID].enable == 1 ? nodes[gl_InstanceID].V.x/2.0 : 0.0;
	normal = _normal;


	gl_Position = view * vec4(position, 1.0f);
}
