#version 420 core

in vec3 position;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gSpecular;

uniform vec3 ambient = vec3(0.2);
uniform vec3 diffuse = vec3(0.4);
uniform vec3 specular = vec3(0.05);
uniform float shininess = 0.01;

layout(binding=1) uniform sampler2D color0;
uniform int hasColorTex = 0;
layout(binding=1) uniform sampler2D specular0;
uniform int hasSpecularTex = 0;
layout(binding=2) uniform sampler2D normal0;
uniform int hasNormalTex = 0;

void main() {
	gPosition = position;
	gNormal = normalize(normal);
	if(hasColorTex == 1) gAlbedo = texture(color0, texCoord).rgb;
	else gAlbedo = color.rgb;
	if(hasSpecularTex == 1) gSpecular = texture(specular0, texCoord).rgb;
	else gSpecular = specular;
}