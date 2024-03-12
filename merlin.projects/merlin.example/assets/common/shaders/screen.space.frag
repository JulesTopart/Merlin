//This file was automatically generated 
//DO NOT CHANGE !
#version 330 core
out vec4 FragColor;

in vec2 texCoord;
uniform sampler2D screen;


void main() {
	FragColor = vec4(texture(screen, texCoord));
}
