#version 330 core

in vec3 position;
//flat in vec3 normal;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

out vec4 FragColor;


void main() {

	vec3 lightDir = normalize(vec3(1, 1, -2));
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

	vec3 n = normalize(normal);

	vec3 ambient = lightColor * 0.5f;
	vec3 diffuse = lightColor * max(dot(n, lightDir), 0.0);
	//float specular = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	FragColor = vec4((ambient + diffuse)* color, 1.0f);
}