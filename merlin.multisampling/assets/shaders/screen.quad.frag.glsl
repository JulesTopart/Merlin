#version 450 core
layout (location = 0) out vec4 o_Color;

in vec2 texCoord;
uniform sampler2D screen;

void main(){
	o_Color = texture(screen, texCoord) + vec4(0.2f, 0,0,1.0f); 
	//FragColor = vec4(texCoord.x, texCoord.y, 1.0f, 1.0f);//
}
