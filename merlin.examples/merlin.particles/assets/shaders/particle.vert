#version 430
#include "../common/shaders/colors.comp"
#include "./buffers.comp"
layout(location = 0) in vec3 _position;

out vec4 position;
out vec4 screen_position;
out vec4 color;
out mat4 mv;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main() {
	uint i = gl_InstanceID;
	vec3 offset = ssbo_position[i].xyz;
	position = model * (vec4(_position + vec3(offset),1));
	screen_position = projection * view * position;
	color = colorMap(length(ssbo_velocity[i].xyz),parula);
	mv = projection * view;
		
	gl_Position = screen_position;
	gl_PointSize = 50.0/(gl_Position.w);		
}
