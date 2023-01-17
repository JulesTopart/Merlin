#version 450 core

layout (location = 0) out vec4 o_Color;

in vec3 position;
in vec3 normal;
flat in vec3 color;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float shininess;

vec4 phong(){
    vec3 ambient = lightColor * 0.2f;

    vec3 n = normalize(normal);
    vec3 lightDir = normalize(lightPos - position);
    vec3 diffuse = lightColor * max(dot(n, lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, n);
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    return vec4((ambient + diffuse + specular) * color, 1.0f);
}

void main(){

    //o_Color = vec4(color, 1.0f);
    o_Color = phong();
}
