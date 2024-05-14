//This file was automatically generated 
//DO NOT CHANGE !
#version 450 core

//In data
layout(location=0) in Vertex
{
	vec3 position;
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

const vec3 Fdielectric = vec3(0.04);

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
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
	// Sample input textures to get shading model params.
	vec3 albedo = material.use_albedo_tex ? texture(material.albedo_tex, vin.texcoord).rgb : material.albedo;
	vec3 normalRGB = material.use_normal_tex ? texture(material.normal_tex, vin.texcoord).rgb : vec3(0.0);
	float metalness = material.use_metallic_tex ? texture(material.metalness_tex, vin.texcoord).r : material.metalness;
	float roughness = material.use_roughness_tex ? texture(material.roughness_tex, vin.texcoord).r : material.roughness;
	float ao = material.use_ao_tex ? texture(material.ao_tex, vin.texcoord).r : material.ao;

	// Outgoing light direction (vector from world-space fragment position to the "eye").
	vec3 Lo = normalize(cameraPosition - vin.position);

	// Get current fragment's normal and transform to world space.
	vec3 N = normalize(2.0 * normalRGB - 1.0);
	N = normalize(vin.tangentBasis * N);
	
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
	}/**/

	// Final fragment color.
	fragColor = vec4(directLighting + ambientLighting, 1.0);
}