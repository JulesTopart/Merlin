#version 450 core

layout (location = 0) out vec4 o_Color;

in vec3 position;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    float constant_attenuation;

    vec3 direction;
    float linear_attenuation;
    vec3 ambient;
    float quadratic_attenuation;
    vec3 diffuse;
    float inner_cutoff;
    vec3 specular;
    float outer_cutoff;
    int type; // 0 = directional, 1 = point, 2 = spot
};

/*
layout(std430, binding = 0) buffer MaterialBlock {
    Material materials[];
};

layout(std430, binding = 1) buffer LightBlock {
    Light lights[];
};
*/

uniform uint numlight;
uniform uint materialID;
uniform vec3 viewPos;

uniform Material material;
uniform Light light;


void main(){
    // Compute the material properties
    //Material material = materials[materialID];

    vec3 ambient = material.ambient;// * material.ambientMultiplier;
    vec3 diffuse = material.diffuse;// * material.diffuseMultiplier;
    vec3 specular = material.specular;// * material.specularMultiplier;
    float shininess = material.shininess;

    // Compute the lighting contributions
    vec3 fragColor = vec3(0.0f);
    vec3 viewDir = normalize(viewPos - position);

    //for (int i = 0; i < numlight; i++)
    {
        //Light light = lights[i];

        vec3 lightDir;
        vec3 lightColor;
        float attenuation;

        if (light.type == 0) // Directional light
        {
            lightDir = normalize(-light.direction);
            lightColor = light.diffuse;
            attenuation = 1.0f;
        }
        else if (light.type == 1) // Point light
        {
            vec3 lightVec = light.position - position;
            float distance = length(lightVec);
            lightDir = normalize(lightVec);
            lightColor = light.diffuse;
            attenuation = 1.0f / (light.constant_attenuation +
                                  light.linear_attenuation * distance +
                                  light.quadratic_attenuation * distance * distance);
        }
        else if (light.type == 2) // Spot light
        {
            vec3 lightVec = light.position - position;
            float distance = length(lightVec);
            lightDir = normalize(lightVec);
            float angle = dot(lightDir, normalize(-light.direction));
            if (angle < light.outer_cutoff)
            {
                lightColor = vec3(0.0f);
            }
            else if (angle < light.inner_cutoff)
            {
                lightColor = light.diffuse * (angle - light.outer_cutoff) /
                             (light.inner_cutoff - light.outer_cutoff);
            }
            else
            {
                lightColor = light.diffuse;
            }
            attenuation = 1.0f / (light.constant_attenuation +
                                  light.linear_attenuation * distance +
                                  light.quadratic_attenuation * distance * distance);
        }
        else // Unknown light type
        {
            //continue;
        }

        // Compute the lighting contribution for this light
        float diff = max(dot(normal, lightDir), 0.0f);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
        fragColor += attenuation * (ambient + diff * lightColor + spec * specular);
    }

    // Output the final color
    o_Color = vec4(fragColor, 1.0f);
}