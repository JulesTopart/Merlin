#version 330 core
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;


vec4 sun(){
    vec3 sunVector = vec3(1,-1,-1);
    gl_FragDepth   = gl_DepthRange.far;
    float gradient = dot(normalize(sunVector), normalize(texCoords)) / 2.0 + 0.5;
    return vec4(pow(gradient, 32), pow(gradient, 48) / 2.0 + 0.5, gradient / 4.0 + 0.75, 1.0);
}

void main()
{    
    //FragColor = texture(skybox, texCoords); //Cubemap texture
    FragColor = vec4(vec3(1.0, -texCoords.y + 0.5, -texCoords.y)*0.2+0.6,1.0f); //texcoord as color

}