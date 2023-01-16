#version 460

layout (local_size_x = 16, local_size_y = 1, local_size_z = 1) in;

#define MAX_BOUNCE 5
struct Ray {
	vec3 o; //Origin
	vec3 d; //Direction
	uint hitID;
	uint bounce;
	float dist;
};

struct Facet {
  uint indices[4]; //quad 
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

  Ray r = rays[index];
  r.o = origin + r.o;
  r.d = r.o - origin; //Compute Ray direction
  r.hitID = -1;
  r.bounce = 0;
  r.dist = 0;
  rays[index] = r;
}