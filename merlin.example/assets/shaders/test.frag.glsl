#version 450 core

layout (location = 0) out vec4 o_Color;

in vec3 normal;
uniform vec4 color;

void main(){

	vec3 lightDir = normalize(vec3(1,1,-2));
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

	vec3 n = normalize(normal);

	vec3 ambient = lightColor * 0.2f;
	vec3 diffuse = lightColor * max(dot(n, lightDir), 0.0);
	//float specular = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	o_Color = vec4(ambient + diffuse, 1.0f) * color;
}