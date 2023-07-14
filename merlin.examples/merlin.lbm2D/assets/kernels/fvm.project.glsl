#version 460

layout(local_size_x = 1, local_size_y = 1) in;

layout(rg32f, binding = 0) uniform image2D velocity;	//Velocity field u velocity, v velocity
layout(r32f, binding = 1) uniform image2D phase;		//Phase field
layout(r32f, binding = 2) uniform image2D pressure;		//Pressure field

uniform float dt;
uniform float speed;
uniform float it;

#define OVERRELAXATION 1.9

uniform float width;
uniform float height;

bool isSolid(ivec2 pos){
	vec4 P = imageLoad(phase, pos);
	return P.x == 0.0;
}

float getSolid(ivec2 pos){
	return imageLoad(phase, pos).x;
}

float getPressure(ivec2 pos){
	return imageLoad(pressure, pos).y;
}

float getU(ivec2 pos){
	if(isSolid(pos) || pos.x == 0 || pos.y == 0 || pos.x == width-1 || pos.y == height-1) return 0.0;
	return imageLoad(velocity, pos).x;
}

float getV(ivec2 pos){
	if(isSolid(pos) || pos.x == 0 || pos.y == 0 || pos.x == width-1 || pos.y == height-1) return 0.0;
	return imageLoad(velocity, pos).y;
}

void storeU(ivec2 pos, vec4 value){
	if(isSolid(pos)) return;
	imageStore(velocity, pos, value);
}
void storeUi(ivec2 pos, float value){
	if(isSolid(pos + ivec2(0,1))) return;
	vec4 pU = imageLoad(velocity, pos);
	imageStore(velocity, pos + ivec2(1,0), vec4(value, pU.y, 0, 0));
}
void storeVj(ivec2 pos, float value){
	if(isSolid(pos + ivec2(0,1))) return;
	vec4 pU = imageLoad(velocity, pos);
	imageStore(velocity, pos + ivec2(0,1), vec4(pU.x, value, 0, 0));
}
void storePression(ivec2 pos, float value){
	if(isSolid(pos)) return;
	imageStore(pressure, pos, vec4(value,0,0,0));
}


void main() {
	ivec2 iPos = ivec2(gl_GlobalInvocationID.xy);
	if (iPos.x == 0 || iPos.y == 0 || iPos.x == width-1 || iPos.y == height-1) return; //boundary
	

	float rho = 1.0;
	float spacing = 1;

	float ts = dt*speed / it;

	float u  = getU(iPos + ivec2(0, 0));
	float v  = getV(iPos + ivec2( 0,0));
	float ui = getU(iPos + ivec2( 1, 0));
	float vj = getV(iPos + ivec2( 0, 1));
	
	float s_i = getSolid(iPos + ivec2(-1, 0));
	float s_j = getSolid(iPos + ivec2( 0,-1));
	float si  = getSolid(iPos + ivec2( 1, 0));
	float sj  = getSolid(iPos + ivec2( 0, 1));

	float s = s_i + s_j + si + sj;
	float p = getPressure(iPos);

	float d = (ui - u + vj - v) * OVERRELAXATION;
	u += d * s_i / s;
	ui -= d * si / s; //velocity flowing outside
	v += d * s_j / s;
	vj -= d * sj / s; //velocity flowing outside
	p += (d/s)*(rho*spacing/ts) ;
	

	if (iPos.x == 1) u = 0.8;

	storeU(iPos, vec4(u,v,0,0));
	storeUi(iPos, ui);
	storeVj(iPos, vj);
	storePression(iPos, p);

}