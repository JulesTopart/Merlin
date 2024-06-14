//This file was automatically generated 
//DO NOT CHANGE !
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 view;
uniform mat4 projection;

// returns a 90 degree x-axis rotation matrix
mat4 get_z_correction_matrix()
{
    float s = sin(radians(90.0));
    float c = cos(radians(90.0));
    return mat4(
        1, 0, 0, 0,
        0, c, s, 0,
        0, -s, c, 0,
        0, 0, 0, 1
    );
}

void main(){
    vec4 pos = projection * view * get_z_correction_matrix() * vec4(aPos, 1.0f);
    // We want to flip the z axis due to the different coordinate systems (left hand vs right hand)
    texCoords = vec3( aPos.x, aPos.y, aPos.z);
    //pos = vec4(pos.x,pos.y,pos.z, pos.w);
    gl_Position = pos.xyww;
}    