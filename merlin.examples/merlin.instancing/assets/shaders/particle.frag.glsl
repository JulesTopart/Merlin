#version 450

in vec3 position;
in vec3 offset;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float shininess;

vec3 directLighting(){
    vec3 ambient = lightColor * 0.2f;

    vec3 n = normalize(normal);
    vec3 lightDir = normalize(lightPos - position);
    vec3 diffuse = lightColor * max(dot(n, lightDir), 0.0f);

    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, n);
    float specular = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
    return (ambient + diffuse + specular) * color;
}


void main() {
  FragColor = vec4(directLighting(), 1.0);
}