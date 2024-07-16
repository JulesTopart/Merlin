//This file was automatically generated 
//DO NOT CHANGE !
#version 450 core

//In data
layout(location=0) in Vertex
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
	mat3 tangentBasis;
} vin;

//Out data
layout(location=0) out vec4 fragColor;

//Constants
const float PI = 3.14159265359;
const float EPSILON = 1e-6;

//Materials data
struct Material{
	vec3  albedo;
	float metalness;
	float roughness;
	float ao;

	bool use_albedo_tex;
	bool use_normal_tex;
	bool use_metallic_tex;
	bool use_roughness_tex;
	bool use_ao_tex;

	sampler2D albedo_tex;
	sampler2D normal_tex;
	sampler2D metalness_tex;
	sampler2D roughness_tex;
	sampler2D ao_tex;
};

struct Environment{
	vec3 irradiance;
	vec3 specular;

	bool use_irradiance_tex;
	bool use_specular_tex;
	bool use_specularBRDF_tex;

	samplerCube specular_tex;
	samplerCube irradiance_tex;
	sampler2D specularBRDF_tex;
};

//Light and camera data
uniform Material material;
uniform Environment environment;

uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform vec3 lightRadiance;


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
uniform int numLights = 0;

const vec3 Fdielectric = vec3(0.04);

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);



void main()
{
	// Sample input textures to get shading model params.
	vec3 albedo = material.use_albedo_tex ? texture(material.albedo_tex, vin.texcoord).rgb : material.albedo;
	vec3 normalRGB = material.use_normal_tex ? normalize(2.0 * texture(material.normal_tex, vin.texcoord).rgb - 1.0) : vin.normal;
	float metalness = material.use_metallic_tex ? texture(material.metalness_tex, vin.texcoord).r : material.metalness;
	float roughness = material.use_roughness_tex ? texture(material.roughness_tex, vin.texcoord).r : material.roughness;
	float ao = material.use_ao_tex ? texture(material.ao_tex, vin.texcoord).r : material.ao;

	// Outgoing light direction (vector from world-space fragment position to the "eye").

	vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metalness);

	// Get current fragment's normal and transform to world space.
	vec3 N = normalRGB;
	//N = normalize(vin.tangentBasis * N);
	vec3 V = normalize(cameraPosition - vin.position);

	vec3 Lo = vec3(0);
	for(int i = 0; i < numLights; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(lights[i].position - vin.position);
        vec3 H = normalize(V + L);
        float dist    = length(lights[i].position - vin.position);
        float attenuation = 1.0;//1.0 / ( lights[i].attenuation.x + lights[i].attenuation.y * dist + lights[i].attenuation.z * dist * dist);
        vec3 radiance     = lights[i].diffuse * attenuation + lights[i].ambient;        
        
        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);        
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metalness;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
    }  
	
	vec3 ambient = vec3(0.03) * albedo * ao;
	vec3 color = ambient + Lo;
	color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
   
    //fragColor = vec4(abs(normalize(N)), 1.0);
    fragColor = vec4(color, 1.0);


	/*
	// Angle between surface normal and outgoing light direction.
	float cosLo = max(0.0, dot(N, Lo));
		
	// Specular reflection vector.
	vec3 Lr = 2.0 * cosLo * N - Lo;

	// Fresnel reflectance at normal incidence (for metals use albedo color).
	vec3 F0 = mix(Fdielectric, albedo, metalness);

	// Direct lighting calculation for analytical lights.
	vec3 directLighting = vec3(0);

	vec3 Li = cameraPosition - lightPosition;
	vec3 Lradiance = lightRadiance;

	// Half-vector between Li and Lo.
	vec3 Lh = normalize(Li + Lo);

	// Calculate angles between surface normal and various light vectors.
	float cosLi = max(0.0, dot(N, Li));
	float cosLh = max(0.0, dot(N, Lh));

	// Calculate Fresnel term for direct lighting. 
	vec3 F  = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
	// Calculate normal distribution for specular BRDF.
	float D = ndfGGX(cosLh, roughness);
	// Calculate geometric attenuation for specular BRDF.
	float G = gaSchlickGGX(cosLi, cosLo, roughness);

	// Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
	// Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
	// To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
	vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);

	// Lambert diffuse BRDF.
	// We don't scale by 1/PI for lighting & material units to be more convenient.
	vec3 diffuseBRDF = kd * albedo;

	// Cook-Torrance specular microfacet BRDF.
	vec3 specularBRDF = (F * D * G) / max(EPSILON, 4.0 * cosLi * cosLo);

	// Total contribution for this light.
	directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
	

	// Ambient lighting (IBL).
	vec3 ambientLighting = vec3(0);
	/*{
		// Sample diffuse irradiance at normal direction.
		vec3 irradiance = environment.use_irradiance_tex ? texture(environment.irradiance_tex, N).rgb : environment.irradiance;

		// Calculate Fresnel term for ambient lighting.
		// Since we use pre-filtered cubemap(s) and irradiance is coming from many directions
		// use cosLo instead of angle with light's half-vector (cosLh above).
		vec3 F = fresnelSchlick(F0, cosLo);

		// Get diffuse contribution factor (as with direct lighting).
		vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);

		// Irradiance map contains exitant radiance assuming Lambertian BRDF, no need to scale by 1/PI here either.
		vec3 diffuseIBL = kd * albedo * irradiance;

		// Sample pre-filtered specular reflection environment at correct mipmap level.
		int specularTextureLevels = environment.use_specular_tex ? textureQueryLevels(environment.specular_tex) : 0;
		vec3 specularIrradiance = environment.use_specular_tex ? textureLod(environment.specular_tex, Lr, roughness * specularTextureLevels).rgb : environment.specular;

		// Split-sum approximation factors for Cook-Torrance specular BRDF.
		vec2 specularBRDF = environment.use_specularBRDF_tex ? texture(environment.specularBRDF_tex, vec2(cosLo, roughness)).rg : vec2(1.0);

		// Total specular IBL contribution.
		vec3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

		// Total ambient lighting contribution.
		ambientLighting = diffuseIBL + specularIBL;
	}

	// Final fragment color.
	fragColor = vec4(directLighting + ambientLighting, 1.0);*/
}





// Uses Disney's reparametrization of alpha = roughness^2.
float ndfGGX(float cosLh, float roughness)
{
	float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}