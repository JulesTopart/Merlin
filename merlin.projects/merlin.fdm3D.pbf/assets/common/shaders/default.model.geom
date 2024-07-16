#version 420 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_out {
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 texcoord;
    vec3 viewPos;
    mat3 tangentBasis;
} vin[];

out GS_out {
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 texcoord;
    vec3 viewPos;
    mat3 tangentBasis;
} vout;

vec3 GetNormal() {
    vec3 a = vec3(gl_in[2].gl_Position) - vec3(gl_in[0].gl_Position);
    vec3 b = vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position);
    return normalize(cross(a, b));
}

uniform bool use_flat_shading = false;

void main() {
    vec3 flat_normal = GetNormal();
    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;

        // Correctly propagate vertex shader data
        vout.position = vin[i].position;
        vout.normal = use_flat_shading ? flat_normal : vin[i].normal;
        vout.color = vin[i].color;
        vout.texcoord = vin[i].texcoord;
        vout.viewPos = vin[i].viewPos;
        vout.tangentBasis = vin[i].tangentBasis;

        EmitVertex();
    }
    EndPrimitive();
}