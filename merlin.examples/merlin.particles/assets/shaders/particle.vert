#version 430
#include "../common/shaders/colors.comp"
layout(location = 0) in vec3 _position;

layout(std430) buffer position_buffer {
	vec4 ssbo_position[];
};

layout(std430) buffer old_position_buffer {
	vec4 ssbo_old_position[];
};

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
	color = colorMap(length(ssbo_position[i].xyz - ssbo_old_position[i].xyz)*100.0,parula);
	mv = projection * view;
		
	gl_Position = screen_position;
	gl_PointSize = 50.0/(gl_Position.w);		
}
