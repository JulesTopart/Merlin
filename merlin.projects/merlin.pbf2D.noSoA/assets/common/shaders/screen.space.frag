//This file was automatically generated 
//DO NOT CHANGE !
#version 330 core
out vec4 FragColor;

#define COLOR 0
#define DEPTH 1

in vec2 texCoord;
uniform sampler2D screen;
uniform int mode = COLOR;

void main() {
	if(mode == COLOR){
		FragColor = vec4(texture(screen, texCoord));
	}else if(mode == DEPTH){
		float depthValue = texture(screen, texCoord).r;
		FragColor = vec4(vec3(depthValue), 1.0);
	}
}
