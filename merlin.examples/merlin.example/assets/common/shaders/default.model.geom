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

// Edges of the triangle
    vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    // Lengths of UV differences
    vec2 deltaUV0 = vin[1].texcoord - vin[0].texcoord;
    vec2 deltaUV1 = vin[2].texcoord - vin[0].texcoord;

    // one over the determinant
    float invDet = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

    vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
    vec3 bitangent = vec3(invDet * (-deltaUV1.x * edge0 + deltaUV0.x * edge1));

    vec3 T = normalize(vec3(model * vec4(tangent, 0.0f)));
    vec3 B = normalize(vec3(model * vec4(bitangent, 0.0f)));
    vec3 N = normalize(vec3(model * vec4(cross(edge1, edge0), 0.0f)));

    mat3 TBN = mat3(T, B, N);
    // TBN is an orthogonal matrix and so its inverse is equal to its transpose
    TBN = transpose(TBN);
    vout.tangentBasis = TBN;

    gl_Position = projection * view * gl_in[0].gl_Position;
    vout.normal = vin[0].normal;
    vout.color = vin[0].color;
    vout.texcoord = vin[0].texcoord;

    vout.position = TBN * gl_in[0].gl_Position.xyz;
    vout.viewPos = TBN * vin[0].viewPos;
    EmitVertex();

    gl_Position = projection * view * gl_in[1].gl_Position;
    vout.normal = vin[1].normal;
    vout.color = vin[1].color;
    vout.texcoord = vin[1].texcoord;

    vout.position = TBN * gl_in[1].gl_Position.xyz;
    vout.viewPos = TBN * vin[1].viewPos;
    EmitVertex();

    gl_Position = projection * view * gl_in[2].gl_Position;
    vout.normal = vin[2].normal;
    vout.color = vin[2].color;
    vout.texcoord = vin[2].texcoord;

    vout.position = TBN * gl_in[2].gl_Position.xyz;
    vout.viewPos = TBN * vin[2].viewPos;
    EmitVertex();

    EndPrimitive();
}  