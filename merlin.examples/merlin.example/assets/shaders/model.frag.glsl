#version 330 core

in vec3 position;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

out vec4 FragColor;

uniform vec3 viewPos;

void main(){
    

    vec3 lightPos = vec3(1,1,2);
    vec3 lightColor = vec3(1);

    vec3 n = normalize(normal);
    vec3 lightDir = normalize(lightPos - position);

    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, n);

    vec3 ambient = lightColor * 0.2f;
    vec3 diffuse = lightColor * max(dot(n, lightDir), 0.0f);
    float specular = pow(max(dot(viewDir, reflectDir), 0.0f), 0.3);

    //o_Color = vec4((ambient + diffuse + specular) * vec3(texture(diffuse0, texCoord)), 1.0f);
    FragColor = vec4((ambient + diffuse + specular) * lightColor, 1.0f);
}