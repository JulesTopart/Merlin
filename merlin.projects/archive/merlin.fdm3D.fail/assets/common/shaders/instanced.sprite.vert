#version 430

layout(location = 0) in vec3 _position;

layout(std430) buffer position_buffer {
	vec4 ssbo_position[];
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
	color = vec4(1);
	mv = projection * view;
		
	gl_Position = screen_position;
	gl_PointSize = 50.0/(gl_Position.w);		
}
