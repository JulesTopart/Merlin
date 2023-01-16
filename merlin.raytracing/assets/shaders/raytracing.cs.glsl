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


uniform uint size; //number of facet
uniform vec3 origin; //TMRT source

// triangle degined by vertices v0, v1 and  v2
vec3 triIntersect( in vec3 ro, in vec3 rd, in vec3 v0, in vec3 v1, in vec3 v2 ){
    vec3 v1v0 = v1 - v0;
    vec3 v2v0 = v2 - v0;
    vec3 rov0 = ro - v0;
    vec3  n = cross( v1v0, v2v0 );
    vec3  q = cross( rov0, rd );
    float d = 1.0/dot( rd, n );
    float u = d*dot( -q, v2v0 );
    float v = d*dot(  q, v1v0 );
    float t = d*dot( -n, rov0 ); 
    if( u<0.0 || v<0.0 || (u+v)>1.0 ) t = -1.0; //No intersection, return vec3(-1.)
    return vec3( t, u, v );
}

vec3 quadIntersect(in vec3 ro, in vec3 rd, in vec3 v0, in vec3 v1, in vec3 v2, in vec3 v3) {
    vec3 t1col = triIntersect(ro, rd, v0, v1, v2);
    vec3 t2col = triIntersect(ro, rd, v0, v2, v3);

    if (t1col.x < 0.0 && t2col.x < 0.0) {
        return vec3(-1.0); // no intersection
    } else if (t1col.x >= 0.0 && t2col.x < 0.0) {
        return t1col;
    } else if (t1col.x < 0.0 && t2col.x >= 0.0) {
        return t2col;
    } else {
        return t1col.x < t2col.x ? t1col : t2col;
    }
}


vec3 planeIntersect(in vec3 ro, in vec3 rd, in vec3 planeNormal, in vec3 planePoint) {
    float d = dot(planeNormal, planePoint);
    float t = (d - dot(planeNormal, ro)) / dot(planeNormal, rd);
    if (t < 0.0) {
        return vec3(-1.0); // no intersection
    }
    return ro + rd * t;
}


void main() {
    uint index = gl_GlobalInvocationID.x;
    Ray r = rays[index];

    float dist = 0;

    for(uint i = 0; i < size; i++){   //Loop for identify the facet intersected by the ray
        Facet f = facets [i];

        //Intersection test, result : Facet id
        //Find the closest point of the geometry on the ray path
        vec3 a = vertices[f.indices[0]].position;
        vec3 b = vertices[f.indices[1]].position;
        vec3 c = vertices[f.indices[2]].position;
        vec3 d;
        vec3 pt1 =vec3(0);

        if(f.indices[3] != -1){
            d = vertices[f.indices[3]].position;
            pt1 = quadIntersect( r.o, r.d, a, b, c, d);

        }else{
            pt1 = triIntersect( r.o, r.d, a, b, c );
        }


        if(pt1 != vec3(-1.0)){ //we hit facet i
            if(length(pt1-r.o) < dist || dist == 0){
                dist = length(pt1-r.o);
                r.hitID = f.id;

                if(length(f.normal) != 0){//specular facet, we continue the path ray
                    



                }
            }
            // if length(f.normal) == 0, diffuse facet
        }
    }

    if(r.hitID == -1){//No intersection with the geometry, Ray goes to the sky or vaccum
            //Calculate intersection with the infinite floor
        vec3 Ptfloor = planeIntersect( r.o, r.d, vec3(0.,0.,1.), vec3 (0.));
        if(Ptfloor != vec3(-1.0)){
            r.hitID = -2;
        }else{ //no intersection with the infinite floor, the ray hit the sky
            r.hitID = -3;
        }
    }else{
        r.dist = dist;//We intersect the geometry
    }
    rays[index] = r;
}