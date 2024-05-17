//This file was automatically generated 
//DO NOT CHANGE !
#version 420 core

in Vertex{
	vec3 position;
	vec3 normal;
	vec3 color;
	vec2 texcoord;
	vec3 viewPos;
	mat3 tangentBasis;
} vin;

out vec4 FragColor;

layout(binding=0) uniform samplerCube skybox;
uniform int hasSkybox = 0;

//Materials data
struct Material{
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
    float shininess;

	bool use_diffuse_tex;
	bool use_normal_tex;
	bool use_specular_tex;

	sampler2D diffuse_tex;
	sampler2D normal_tex;
	sampler2D specular_tex;
};


struct Environment{
	vec3 ambient;
	vec3 irradiance;
	vec3 specular;

	bool use_skybox_tex;
	bool use_ambient_tex;
	bool use_specular_tex;
	bool use_specularBRDF_tex;

	samplerCube skybox_tex;
	samplerCube specular_tex;
	samplerCube irradiance_tex;
	sampler2D specularBRDF_tex;
};

//Light and camera data
uniform Material material;
uniform Environment environment;

#define MAX_LIGHTS 10

struct Light {
    vec3 position;    // For point and spot lights
    vec3 direction;   // For directional and spot lights
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation; //constant, linear, quadratic
    float cutOff;     // For spot lights
    int type;         // 0: Directional, 1: Point, 2: Spot
};

uniform Light lights[MAX_LIGHTS];
uniform int numLights;

vec3 calculateDirectionalLight(Light light, vec3 normal, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor);
vec3 calculatePointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor);
vec3 calculateSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor);
vec3 calculateAmbientLight(Light light, vec3 ambientColor);

vec3 calculateLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    if (light.type == 0) {
        return calculatePointLight(light, normal, fragPos, viewDir, ambientColor, diffuseColor, specularColor);
    } else if (light.type == 1) {
        return calculateDirectionalLight(light, normal, viewDir, ambientColor, diffuseColor, specularColor);
    } else if (light.type == 2) {
        return calculateSpotLight(light, normal, fragPos, viewDir, ambientColor, diffuseColor, specularColor);
    }else if (light.type == 3) {
        return calculateAmbientLight(light, ambientColor);
    }
    return vec3(0.0);
}

vec3 calculateAmbientLight(Light light, vec3 ambientColor) {
    return light.ambient * ambientColor;
}
vec3 calculateDirectionalLight(Light light, vec3 normal, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    vec3 lightDir = normalize(-light.direction);
    
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = 0;
    if(diff != 0){
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess); //blinn
    }
    vec3 ambient = light.ambient * ambientColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;

    return ambient + diffuse + specular;
}

vec3 calculatePointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    vec3 light_position = vin.tangentBasis * light.position;
    vec3 lightVec = light_position - fragPos;
    
    float dist = length(lightVec);
    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * (dist * dist));
    
    vec3 lightDir = normalize(lightVec);
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = 0;
    //if(diff != 0){
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess); //blinn
    //}

    vec3 ambient = light.ambient * ambientColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;

    return attenuation * (ambient + diffuse + specular);
}

vec3 calculateSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    vec3 light_position = vin.tangentBasis * light.position;
    vec3 lightDir = normalize(light_position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float dist = length(light_position - fragPos);
    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * (dist * dist));
    //float attenuation = 1.0 / (dist);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - 0.95;
    float intensity = clamp((theta - 0.95) / epsilon, 0.0, 1.0);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = 0;
    if(diff > 0) spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess); //blinn

    vec3 ambient = light.ambient * ambientColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;

    return intensity * attenuation * (ambient + diffuse + specular);
}

void main() {

	vec3 norm = normalize(vin.normal);
	vec2 uv = vin.texcoord;
	//vec3 lightDir = normalize(lightPos - position );
	//float diff = max(dot(norm, lightDir), 0.0);


	//Load material textures
	vec3 N = normalize(material.use_normal_tex ? normalize(texture(material.normal_tex, uv).xyz * 2.0f - 1.0) : vin.normal);
	// Reflection (using skybox)
	
	vec3 I = normalize(vin.position - vin.viewPos);
	vec3 R = reflect(I, norm);

	R = vec3(R.x, -R.z, -R.y);
	vec3 skyColor;
	if(environment.use_skybox_tex) skyColor = mix(vec3(1),textureLod(environment.skybox_tex, R, 6.0).rgb, material.shininess);
	else skyColor = vec3(-R.y*0.5+0.8);
    

	vec3 ambientColor = material.ambient_color;
	vec3 diffuseColor;
	if (material.use_diffuse_tex) diffuseColor = material.diffuse_color * texture(material.diffuse_tex, uv).rgb;
	else diffuseColor = material.diffuse_color * vin.color;
	
	vec3 specularColor;
	if (material.use_specular_tex) specularColor = material.specular_color * texture(material.specular_tex, uv).r;
	else specularColor = material.specular_color + skyColor*0.1;

    vec3 viewDir = normalize(vin.viewPos - vin.position);

    vec3 finalColor = vec3(0.0);

    for (int i = 0; i < numLights; ++i) {
        finalColor += calculateLight(lights[i], N, vin.position, viewDir, ambientColor, diffuseColor, specularColor);
    }

    //FragColor = vec4(finalColor, 1.0);
    float gamma = 2.2;
    FragColor.rgb = pow(finalColor.rgb, vec3(1.0/gamma));
    //FragColor.rgb = abs(N);
    FragColor.a = 1.0;

}