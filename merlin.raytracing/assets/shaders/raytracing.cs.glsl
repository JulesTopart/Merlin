#version 460

layout (local_size_x = 16, local_size_y = 1, local_size_z = 1) in;

#define MAX_BOUNCE 5
struct Ray {
	vec3 o; //Origin
	vec3 d; //Direction
	int hitID;
	int bounce;
};

struct Facet {
  vec3 vertex[3]; //quad 
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

uniform vec3 origin; //TMRT source

void main() {
  uint index = gl_GlobalInvocationID.x;



}