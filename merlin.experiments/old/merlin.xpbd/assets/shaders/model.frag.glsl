#version 330 core

in vec3 position;
//flat in vec3 normal;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec4 lightColor;

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

uniform samplerCube skybox;

uniform sampler2D color0;
uniform int hasColorTex;
uniform sampler2D normal0;
uniform int hasNormalTex;

out vec4 FragColor;

vec4 defaultLight() {
	vec3 ambientColor = ambient * color;
	vec3 norm = normalize(normal);
	vec2 uv = texCoord;
	if (abs(uv.x) > 0.0) uv.x *= 2.0;
	if (abs(uv.y) > 0.0) {
		uv.y *= 1.2;
		uv.y -= uv.y;
	}

	if (hasNormalTex == 1) {
		norm = normalize(texture(normal0, uv).rgb) * norm;
	}

	vec3 lightDir = normalize(lightPos - position);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuseColor;
	if (hasColorTex == 1) {
		diffuseColor = diff * texture(color0, uv).rgb * color;
	}
	else {
		diffuseColor = diff * diffuse * color;
	}

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

	
	// Reflection (using skybox)
	vec3 I = normalize(position - viewPos);
	vec3 R = reflect(I, norm);
	R = vec3(R.x, -R.z, -R.y);
	vec4 skyColor = texture(skybox, R);
	finalColor = mix(finalColor, skyColor.rgb, 0.0); // Adjust the reflection amount with the last parameter
	

	return vec4(finalColor, 1.0);
	//return vec4(texCoord, 1.0, 1.0);
}

void main() {
	//FragColor = vec4(1);
	FragColor = defaultLight();
	//FragColor = texture(color0, texCoord);
}