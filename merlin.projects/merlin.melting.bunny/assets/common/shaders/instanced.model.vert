//This file was automatically generated 
//DO NOT CHANGE !
#version 430 core

layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 _normal;
layout (location = 2) in vec3 _color;
layout (location = 3) in vec2 _texcoord;
layout (location = 4) in vec3 _tangent;
layout (location = 5) in vec3 _bitangent;

#define MAX_LIGHTS 10

out VS_out{
	vec3 position;
	vec3 normal;
	vec3 color;
	vec2 texcoord;
	vec3 viewPos;
	mat3 tangentBasis;
} vout;


layout(std430) buffer position_buffer {
	vec4 ssbo_position[];
};

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform vec3 viewPos;


void main() {
	vec3 offset = ssbo_position[gl_InstanceID].xyz;
	vout.position = vec3(model * (vec4(_position + vec3(offset),1)));
	vout.color = _color;
	vout.normal = _normal;
	vout.texcoord = _texcoord;
	vout.viewPos = viewPos;

	if(length(_tangent) == 0 || length(_bitangent) == 0 || length(_normal) == 0){
		vout.tangentBasis = mat3(1);//set to identity
	}else{
		vec3 T = normalize(vec3(model * vec4(_tangent, 0.0)));
		vec3 B = normalize(vec3(model * vec4(_bitangent, 0.0)));
		vec3 N = normalize(vec3(model * vec4(_normal, 0.0)));
	
		// re-orthogonalize T with respect to N
		T = normalize(T - dot(T, N) * N);
		// then retrieve perpendicular vector B with the cross product of T and N
		B = cross(N, T);

		vout.tangentBasis = transpose(mat3(T, B, N));   
	}

	gl_Position = projection * view * vec4(vout.position, 1.0f);
}
