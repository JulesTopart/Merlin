#version 430 core
layout (location = 0) out vec4 o_Color;
in vec3 normal;
void main(){
	o_Color = vec4(vec3(0.5f) + max(dot(normalize(normal), normalize(vec3(1,1,-2))), 0.0), 1.0f);
}