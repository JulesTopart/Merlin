#version 460

layout (local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

#define MAX_BOUNCE 5
struct Ray {
    vec3 o; //Origin
    vec3 d; //Direction
    vec3 e; //End
    uint hitID;
    uint bounce;
};

struct Facet {
    vec3 vertex[4];
    vec3 normal;
    uint id;
};

layout (std430, binding = 1) buffer RayBuffer {
  Ray rays[];
};

layout (std430, binding = 2) buffer FacetBuffer {
  Facet facets[];
};

uniform vec3 origin; //TMRT source

void main() {
  uint index = gl_GlobalInvocationID.x;

  Ray r = rays[index];
  r.d = normalize(r.o); //Compute Ray direction
  r.o = origin + r.o;
  r.e = r.o + 0.1*r.d;
  r.hitID = -1;
  r.bounce = 0;
  rays[index] = r;
}