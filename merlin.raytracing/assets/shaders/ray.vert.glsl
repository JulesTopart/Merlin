#version 460 core

#define MAX_BOUNCE 5
struct Ray {
	vec3 o; //Origin
	vec3 d; //Direction
	int hitID;
	int bounce;
	float dist;
};

struct Facet {
  vec3 vertex[3]; //triangle 
  vec3 normal;
  uint id;
};

struct Vertex {
  vec3 position;
  vec3 normal;
  vec3 color;
  vec2 texCoord;
};

layout (std430, binding = 1) buffer RayBuffer {
  Ray rays[];
};

layout (std430, binding = 2) buffer FacetBuffer {
  Facet facets[];
};

layout (std430, binding = 3) buffer VertexBuffer {
  Vertex vertices[];
};

layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 _normal;
layout (location = 2) in vec3 _color;
layout (location = 3) in vec2 _texCoord;

uniform mat4 view;
uniform mat4 model;

out vec3 position;
out vec3 offset;
out vec3 normal;
out vec3 color;
out vec2 texCoord;


void main() {
	offset = vec3(rays[gl_InstanceID].o);
	normal = rays[gl_InstanceID].d;
	if(length(_position) == 0.0){
		position = vec3( vec4((_position), 1.0f) );
		position += offset;
	}else{
		if(rays[gl_InstanceID].hitID != uint(-1))
			position = vec3( vec4((normal*rays[gl_InstanceID].dist), 1.0f) );
		else
			position = vec3( vec4((normal*length(_position)), 1.0f) );
			
		position += offset;
	}
	color = rays[gl_InstanceID].hitID != uint(-1) ? vec3(0,1,0) : vec3(1,0,0);
	gl_Position = view * vec4(position, 1.0f);
} 
