#version 330 core

in vec3 position;
in vec3 normal;
in vec3 color;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec4 lightColor;

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

out vec4 FragColor;

vec4 defaultLight() {
	vec3 ambientColor = ambient * color;
	vec3 norm = normalize(normal);

	vec3 lightDir = normalize(lightPos - position);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuseColor;

	diffuseColor = diff * diffuse * color;
	
	// Specular
	vec3 viewDir = normalize(viewPos - position);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specularColor;

	specularColor = spec * specular;
	

	// Attenuation
	float distance = length(lightPos - position);
	float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
	vec3 finalColor = ambientColor + attenuation * (diffuseColor + specularColor);
	// Apply light color
	finalColor *= vec3(1);// lightColor.rgb;

	return vec4(finalColor, 1.0);
}

void main() {
	FragColor = defaultLight();
}