#version 330 core

in vec3 position;
//flat in vec3 normal;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

uniform vec3 viewPos;

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;


out vec4 FragColor;

void main(){
    
    vec3 lightPos = vec3(1,100,400);

    vec3 n = normalize(normal);
    vec3 lightDir = normalize(lightPos - position);

    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, n);

    vec3 _ambient = ambient * 0.1f;
    vec3 _diffuse = diffuse * max(dot(n, lightDir), 0.0f);
    vec3 _specular = specular * pow(max(dot(viewDir, reflectDir), 0.0f), max(shininess, 8));

    FragColor = vec4(_ambient + _diffuse + _specular, 1.0f);
    //FragColor = vec4( abs(normal), 1.0f);
}


