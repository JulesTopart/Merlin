#version 430
#include "common/uniforms.comp"
#include "common/constants.comp"
#include "common/buffers.comp"
#include "common/colors.comp"

layout(location = 0) in vec3 _position;
layout(location = 1) in vec3 _normal;

out vec4 position;
out vec4 screen_position;
out vec4 normal;
out vec4 color;
out vec2 texCoord;
out mat4 mv;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform int colorCycle = 4;
uniform vec2 WindowSize;
uniform float zoomLevel = 20;

void main() {
	uint i = gl_InstanceID;
	vec3 offset = xi;
	position = model * (vec4(_position + vec3(offset),1));

	color = vec4(1);

	if(colorCycle == 0){ 
		color = vec4(vec3(0.8), 1.0);
	}else if(colorCycle == 4){ 
		color = colorMap(map(length(B_V(gl_InstanceID)),0,1000.0), parula);
	}else if(colorCycle == 5){ 
		color = vec4(randomColor(i), 1);
	}
	


	screen_position = projection * view * position;
	mv = projection * view;
		
	gl_Position = screen_position;
	gl_PointSize = 4.0*particleRadius*400.0/(gl_Position.w);
		
	
}