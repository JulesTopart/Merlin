#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 pos = projection * view * vec4(aPos, 1.0f);

    // We want to flip the z axis due to the different coordinate systems (left hand vs right hand)
    texCoords = vec3( aPos.x, -aPos.z, -aPos.y);
    //texCoords = normalize(vec3( aPos.x, aPos.y, aPos.z));
    // Having z equal w will always result in a depth of 1.0f
    pos = vec4(pos.x,pos.y,pos.z, pos.w);
    gl_Position = pos.xyww;

}    