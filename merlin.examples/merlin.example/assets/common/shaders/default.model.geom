//This file was automatically generated 
//DO NOT CHANGE !
#version 330
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out Vertex{
	vec3 position;
	vec3 normal;
	vec3 color;
	vec2 texcoord;
	vec3 viewPos;
	mat3 tangentBasis;
} vout;

in Vertex{
	vec3 position;
	vec3 normal;
	vec3 color;
	vec2 texcoord;
    vec3 viewPos;
	mat3 tangentBasis;
} vin[];
  
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main(){
    

    gl_Position = projection * view * gl_in[0].gl_Position;
    vout.normal = vin[0].normal;
    vout.color = vin[0].color;
    vout.texcoord = vin[0].texcoord;
    vout.texcoord = vin[0].texcoord;
    vout.tangentBasis = vin[0].tangentBasis;
    vout.position = vout.tangentBasis * gl_in[0].gl_Position.xyz;
    vout.viewPos = vout.tangentBasis * vin[0].viewPos;
    EmitVertex();

    gl_Position = projection * view * gl_in[1].gl_Position;
    vout.normal = vin[1].normal;
    vout.color = vin[1].color;
    vout.texcoord = vin[1].texcoord;
    vout.tangentBasis = vin[1].tangentBasis;
    vout.position = vout.tangentBasis * gl_in[1].gl_Position.xyz;
    vout.viewPos = vout.tangentBasis * vin[1].viewPos;
    EmitVertex();

    gl_Position = projection * view * gl_in[2].gl_Position;
    vout.normal = vin[2].normal;
    vout.color = vin[2].color;
    vout.texcoord = vin[2].texcoord;
    vout.tangentBasis = vin[2].tangentBasis;
    vout.position = vout.tangentBasis * gl_in[2].gl_Position.xyz;
    vout.viewPos = vout.tangentBasis * vin[2].viewPos;
    EmitVertex();
    


    EndPrimitive();
}  
