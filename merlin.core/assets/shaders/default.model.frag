//This file was automatically generated 
//DO NOT CHANGE !
#version 420 core

in vec3 position;
in vec3 normal;
in vec3 color;
in vec2 texCoord;
out vec4 FragColor;

uniform vec3 viewPos;

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

#define MAX_LIGHTS 10

struct Light {
    vec3 position;    // For point and spot lights
    vec3 direction;   // For directional and spot lights
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cutOff;     // For spot lights
    int type;         // 0: Directional, 1: Point, 2: Spot
};

uniform Light lights[MAX_LIGHTS];
uniform int numLights;


vec3 calculateDirectionalLight(Light light, vec3 normal, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor);
vec3 calculatePointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor);
vec3 calculateSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor);

vec3 calculateLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    if (light.type == 0) {
        return calculateDirectionalLight(light, normal, viewDir, ambientColor, diffuseColor, specularColor);
    } else if (light.type == 1) {
        return calculatePointLight(light, normal, fragPos, viewDir, ambientColor, diffuseColor, specularColor);
    } else if (light.type == 2) {
        return calculateSpotLight(light, normal, fragPos, viewDir, ambientColor, diffuseColor, specularColor);
    }
    return vec3(0.0);
}

vec3 calculateDirectionalLight(Light light, vec3 normal, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    vec3 lightDir = normalize(light.direction);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess * 128.0);

    vec3 ambient = light.ambient * ambientColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;

    return ambient + diffuse + specular;
}

vec3 calculatePointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (0.01 + 0.001 * distance + 0.0001 * distance * distance);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess * 128.0);

    vec3 ambient = light.ambient * ambientColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;

    return attenuation * (ambient + diffuse + specular);
}

vec3 calculateSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (0.01 + 0.001 * distance + 0.0001 * distance * distance);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - 0.95;
    float intensity = clamp((theta - 0.95) / epsilon, 0.0, 1.0);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess * 128.0);

    vec3 ambient = light.ambient * ambientColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;

    return intensity * attenuation * (ambient + diffuse + specular);
}

void main() {

	vec3 norm = normalize(normal);
	vec2 uv = texCoord;
	//vec3 lightDir = normalize(lightPos - position );
	//float diff = max(dot(norm, lightDir), 0.0);


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

    vec3 viewDir = normalize(viewPos - position);

    vec3 finalColor = vec3(0.0);

    for (int i = 0; i < numLights; ++i) {
        finalColor += calculateLight(lights[i], N, position, viewDir, ambientColor, diffuseColor, specularColor);
    }

    FragColor = vec4(finalColor, 1.0);

}