//This file was automatically generated 
//DO NOT CHANGE !
#version 420 core

#define MAX_LIGHTS 10
#define AMBIENT 0
#define POINT_LIGHT 1
#define DIRECTIONAL_LIGHT 2
#define SPOT_LIGHT 3


in GS_out{
	vec3 position;
	vec3 normal;
	vec3 color;
	vec2 texcoord;
	vec3 viewPos;
	mat3 tangentBasis;
} vin;

out vec4 FragColor;

//Materials data
struct Material{
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
    float shininess;
    float alpha;

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

struct Light {
    vec3 position;    // For point and spot lights
    vec3 direction;   // For directional and spot lights
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation; //constant, linear, quadratic
    float cutOff;     // For spot lights
    int type;
    mat4 lightSpaceMatrix;
    float far_plane;
    int castShadow;
    sampler2D shadowMap;
    samplerCube omniShadowMap;
};

uniform samplerCube skybox;
uniform int hasSkybox = 0;
uniform Material material;
uniform Environment environment;
uniform Light lights[MAX_LIGHTS];
uniform int numLights;

uniform bool useShadows = false;
uniform vec3 viewPos;
uniform mat4 model;
uniform bool use_vertex_color = false;

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float computeShadow(Light li, vec4 fragPosLightSpace)
{   
    if(!useShadows) return 0.0;
    if(li.castShadow == 0) return 0.0;
    float shadow = 0.0;

    if(li.type == DIRECTIONAL_LIGHT || li.type == SPOT_LIGHT ){
        // perform perspective divide
        vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
        // transform to [0,1] range
        projCoords = projCoords * 0.5 + 0.5;
        // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
        float closestDepth = texture(li.shadowMap, projCoords.xy).r; 
        // get depth of current fragment from light's perspective
        float currentDepth = projCoords.z;
        vec3 normal = normalize(vin.normal);
        vec3 lightDir = normalize(li.position - vin.position);
        float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
        vec2 texelSize = 1.0 / textureSize(li.shadowMap, 0);
        for(int x = -1; x <= 1; ++x)
        {
            for(int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(li.shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
                shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
            }    
        }
        shadow /= 9.0;
    
        // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
        if(projCoords.z > 1.0){
            shadow = 0.0;
        }
    }else if(li.type == POINT_LIGHT){
        vec3 fragToLight = vin.position - li.position;
        float currentDepth = length(fragToLight);

        float bias = 0.35;
        int samples = 20;
        float viewDistance = length(viewPos - vin.position);
        float diskRadius = (1.0 + (viewDistance / li.far_plane)) / 25.0;

        // Correctly sample the shadow map using 3D texture coordinates
        for (int i = 0; i < samples; ++i) {
            float closestDepth = texture(li.omniShadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
            closestDepth *= li.far_plane;  // undo mapping [0;1]
            if (currentDepth - bias > closestDepth) {
                shadow += 1.0;
            }
        }
        shadow /= float(samples);
        
    }
        
    return shadow;
}


vec3 calculateDirectionalLight(Light light, vec3 normal, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor);
vec3 calculatePointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor);
vec3 calculateSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor);
vec3 calculateAmbientLight(Light light, vec3 ambientColor);

vec3 calculateLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    if (light.type == POINT_LIGHT) {
        return calculatePointLight(light, normal, fragPos, viewDir, ambientColor, diffuseColor, specularColor);
    } else if (light.type == DIRECTIONAL_LIGHT) {
        return calculateDirectionalLight(light, normal, viewDir, ambientColor, diffuseColor, specularColor);
    } else if (light.type == SPOT_LIGHT) {
        return calculateSpotLight(light, normal, fragPos, viewDir, ambientColor, diffuseColor, specularColor);
    }else if (light.type == AMBIENT) {
        return calculateAmbientLight(light, diffuseColor);
    }
    return vec3(0.0);
}

vec3 calculateAmbientLight(Light light, vec3 ambientColor) {
    return light.ambient * ambientColor;
}

vec3 calculateDirectionalLight(Light light, vec3 normal, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    vec3 lightDir = -normalize(vin.tangentBasis * light.direction);
    
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = 0;
    if(diff != 0){
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess); //blinn
    }
    vec3 ambient = light.ambient * ambientColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;

    vec4 fragPosLightSpace = light.lightSpaceMatrix * vec4(vin.position, 1.0);
    return ambient + (1.0 - computeShadow(light, fragPosLightSpace)) * (diffuse + specular);
}

vec3 calculatePointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    vec3 light_position = vin.tangentBasis * light.position;
    vec3 lightVec = light_position - fragPos;
    
    float dist = length(lightVec);
    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * (dist * dist));
    
    vec3 lightDir = normalize(lightVec);
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = 0;
    if(diff != 0){
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess); //blinn
    }

    vec3 ambient = light.ambient * ambientColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;
    vec4 fragPosLightSpace = light.lightSpaceMatrix * vec4(vin.position, 1.0);
    float shadow_inv = (1.0 - computeShadow(light, fragPosLightSpace));
   
    return ambient + attenuation * shadow_inv * (diffuse + specular);
}

vec3 calculateSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    vec3 light_position = vin.tangentBasis * light.position;
    vec3 lightDir = normalize(light_position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float dist = length(light_position - fragPos);
    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * (dist * dist));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = radians(light.cutOff) -  radians(0.95);
    float intensity = clamp((theta -  radians(0.95)) / epsilon, 0.0, 1.0);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = 0;
    if(diff > 0) spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess); //blinn

    vec3 ambient = light.ambient * ambientColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;

    vec4 fragPosLightSpace = light.lightSpaceMatrix * vec4(vin.position, 1.0);
    float shadow_inv = (1.0 - computeShadow(light, fragPosLightSpace));

    return ambient + (intensity * attenuation * shadow_inv * (diffuse + specular));
}

void main() {
	vec2 uv = vin.texcoord;

	//Load material textures
	vec3 N = vin.tangentBasis * vin.normal;
	if(material.use_normal_tex){
        N = texture(material.normal_tex, uv).rgb * 2.0f - 1.0;
    }
    N = normalize(N);
	
	vec3 I = normalize(vin.position - vin.viewPos);
	vec3 R = reflect(I, N);

	R = vec3(R.x, -R.z, -R.y);
	vec3 skyColor;
	if(environment.use_skybox_tex) skyColor = mix(vec3(1),textureLod(environment.skybox_tex, R, 6.0).rgb, material.shininess);
	else skyColor = vec3(-R.y*0.5+0.8);
    
    vec3 viewDir = normalize(vin.viewPos - vin.position);

    vec3 finalColor = vec3(0.0);

	vec3 ambientColor = material.ambient_color;
	vec3 diffuseColor;
	if (material.use_diffuse_tex) diffuseColor = material.diffuse_color * texture(material.diffuse_tex, uv).rgb;
	else diffuseColor = material.diffuse_color;
	
	vec3 specularColor;
	if (material.use_specular_tex) specularColor = material.specular_color * texture(material.specular_tex, uv).r;
	else specularColor = material.specular_color + skyColor*0.1;

    for (int i = 0; i < numLights; ++i) {
        finalColor += calculateLight(lights[i], N, vin.position, viewDir, ambientColor, diffuseColor, specularColor);
    }
    float gamma = 0.6;
    FragColor.rgb = pow(finalColor.rgb * (use_vertex_color ? vin.color.rgb : vec3(1.0)), vec3(1.0/gamma));
    //FragColor.rgb = N* 0.9 + FragColor.rgb * 0.1;
    //FragColor.rgb = normalize(vin.tangentBasis * (-lights[0].direction));
    //FragColor.rgb = N;
    FragColor.a = material.alpha;

}