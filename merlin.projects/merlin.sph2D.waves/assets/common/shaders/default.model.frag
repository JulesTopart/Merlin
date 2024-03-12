//This file was automatically generated 
//DO NOT CHANGE !
#version 420 core

in vec3 position;
in vec3 normal;
in vec3 color;
in vec2 texCoord;
out vec4 FragColor;

uniform vec3 viewPos;
uniform vec3 lightPos = vec3(50,50,1000);
uniform vec4 lightColor = vec4(1.0);

uniform vec3 ambient = vec3(0.2);
uniform vec3 diffuse = vec3(0.4);
uniform vec3 specular = vec3(0.05);
uniform float shininess = 0.01;

layout(binding=0) uniform samplerCube skybox;
uniform int hasSkybox = 0;
layout(binding=1) uniform sampler2D color0;
uniform int hasColorTex = 0;
layout(binding=2) uniform sampler2D normal0;
uniform int hasNormalTex = 0;


vec4 pointLight(){	

	vec3 norm = normalize(normal);
	vec2 uv = texCoord;
	vec3 lightDir = normalize(lightPos - position );
	float diff = max(dot(norm, lightDir), 0.0);


	//Load material textures
	vec3 N;
	if (hasNormalTex == 1) N = normalize(texture(normal0, uv).rgb);
	else N = normalize(normal);

	// Reflection (using skybox)
	
	vec3 I = normalize(position - viewPos);
	vec3 R = reflect(I, norm);
	R = vec3(R.x, -R.z, -R.y);
	vec3 skyColor;
	if(hasSkybox == 1) skyColor = mix(vec3(1),textureLod(skybox, R, 6.0).rbg, shininess);
	else skyColor = vec3(-R.y*0.5+0.8);


	vec3 ambientColor;
	if (hasColorTex == 1) ambientColor = ambient * (texture(color0, uv).rgb + 5.0*normalize(skyColor));
	else ambientColor = ambient;

	vec3 diffuseColor;
	if (hasColorTex == 1) diffuseColor = diffuse * texture(color0, uv).rgb;
	else diffuseColor = diffuse;
	
	vec3 specularColor;
	if (hasColorTex == 1) specularColor = specular * texture(color0, uv).rgb;
	else specularColor = specular;



	
   
	// intensity of light with respect to distance
	float dist = length(lightDir);
	float a = 0.01;
	float b = 0.001;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	vec3 L = normalize(vec3(lightDir));
	float diffuseAmount = max(dot(N, L), 0.0f);

	// specular lighting
	vec3 viewDirection = normalize(viewPos - position);
	vec3 halfwayVec = normalize(viewDirection + L);
	float specularAmount = pow(max(dot(N, halfwayVec), 0.0f), shininess*128);
	
	vec4 finalColor = vec4(ambientColor + (diffuseColor * (diffuseAmount * inten) + specularAmount * specularColor * inten) * (vec3(lightColor) * skyColor ), 1.0);
	
	

	return finalColor;
}

vec4 direcLight()
{
	vec3 ambientColor = ambient * color;
	vec3 norm = normalize(normal);
	vec2 uv = texCoord;
	vec3 lightDir = normalize(lightPos - position );
	float diff = max(dot(norm, lightDir), 0.0);


	//Load material textures
	vec3 N;
	if (hasNormalTex == 1) N = normalize(texture(normal0, uv).rgb);
	else N = normalize(normal);

	vec3 diffuseColor;
	if (hasColorTex == 1) diffuseColor = texture(color0, uv).rgb;
	else diffuseColor = color;
	
	vec3 specularColor;
	if (hasColorTex == 1) specularColor = specular * texture(color0, uv).rgb;
	else specularColor = specular;



	// ambient lighting
	float ambient = 0.20f;
	// diffuse lighting
	float diffuse = max(dot(N, lightDir), 0.0f);

	// specular lighting
	float specular = 0.0f;
	if (diffuse != 0.0f)
	{
		float specularLight = 0.50f;
		vec3 viewDirection = normalize(viewPos - position);
		vec3 halfwayVec = normalize(viewDirection + lightDir);
		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * specularLight;
	};
	
	return vec4((diffuseColor * (diffuse + ambient) + specularColor * specular) * vec3(lightColor), 1.0);
}

vec4 spotLight()
{

	vec3 ambientColor = ambient * color;
	vec3 norm = normalize(normal);
	vec2 uv = texCoord;
	vec3 lightDir = normalize(lightPos - position );
	float diff = max(dot(norm, lightDir), 0.0);


	//Load material textures
	vec3 N;
	if (hasNormalTex == 1) N = normalize(texture(normal0, uv).rgb);
	else N = normalize(normal);

	vec3 diffuseColor;
	if (hasColorTex == 1) diffuseColor = texture(color0, uv).rgb;
	else diffuseColor = color;
	
	vec3 specularColor;
	if (hasColorTex == 1) specularColor = specular * texture(color0, uv).rgb;
	else specularColor = specular;


	// controls how big the area that is lit up is
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// ambient lighting
	float ambient = 0.20f;
	// diffuse lighting
	float diffuse = max(dot(N, lightDir), 0.0f);

	// specular lighting
	float specular = 0.0f;
	if (diffuse != 0.0f)
	{
		float specularLight = 0.50f;
		vec3 viewDirection = normalize(viewPos - position);
		vec3 halfwayVec = normalize(viewDirection + lightDir);
		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * specularLight;
	};

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDir);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return vec4((diffuseColor * (diffuse * inten + ambient) + specularColor * specular * inten) * vec3(lightColor), 1.0);

}



void main() {
	FragColor = pointLight();
}