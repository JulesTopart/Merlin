#version 420 core
in vec3 position;
in vec3 normal;
in vec3 color;
in vec2 texCoord;
out vec4 FragColor;

// material parameters
uniform vec3  albedo = vec3(1, 0.1 ,0.1);
uniform float metallic = 0.1;
uniform float roughness = 0.2;
uniform float ao = 0.1;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec4 lightColor;

uniform samplerCube skybox;
uniform int hasSkybox = 0;

layout(binding=0) uniform sampler2D albedoTex;
layout(binding=1) uniform sampler2D normalTex;
layout(binding=2) uniform sampler2D metalnessTex;
layout(binding=3) uniform sampler2D roughnessTex;
layout(binding=4) uniform samplerCube specularTex;
layout(binding=5) uniform samplerCube irradianceTex;

const float PI = 3.14159265359;
const vec3 Fdielectric = vec3(0.04);

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 getNormalFromNormalMap();

void main()
{		
    vec3 N = normalize(normal);
    vec3 V = normalize(viewPos - position);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
	           
    // reflectance equation
    vec3 Lo = vec3(0.0);

    // calculate per-light radiance
    vec3 L = normalize(lightPos - position);
    vec3 H = normalize(V + L);
    float distance    = length(lightPos - position);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance     = vec3(lightColor) * attenuation;        
        
    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, roughness);        
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
        
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;  
            
    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);                
    Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
     	
	vec3 R = reflect(-V, N); 
	vec3 prefilteredColor = texture(skybox, R,  roughness).rgb;
  
    vec3 ambient = vec3(0.2) +  vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;


    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
   

   	
	// Reflection (using skybox)
	vec3 I = normalize(position - viewPos);
	vec3 Re = reflect(I, N);
	Re = vec3(Re.x, -Re.z, -Re.y);
	vec3 skyColor = texture(skybox, Re).rgb;
   	color += skyColor / metallic; // Adjust the reflection amount with the last parameter
	

    FragColor = vec4(color, 1.0);
}  

/*
vec3 getNormalFromNormalMap(){
    vec3 n = texture(normalMap, texCoord).rgb;
    // transform normal vector to range [-1,1]
    n = normalize(normal * 2.0 - 1.0);   
    return n;
}
*/

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float GeometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}
  
float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}

float DistributionGGX(vec3 N, vec3 H, float a)
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}