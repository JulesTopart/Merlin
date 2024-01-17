//? #version 430
#ifndef INCLUDE_BUFFERS_GLSL
#define INCLUDE_BUFFERS_GLSL

struct Particle {
	vec4 initial_position;	// initial position    x0
	vec4 pposition;			// predicted position  x~
	vec4 position;			// current position    x
	vec4 new_position;		// predicted position  x+1
	vec4 velocity;			// velocity			   u
	vec4 new_velocity;		// velocity			   u*
	vec4 acceleration;		// acceleration		   a
	float mass;				// mass				   m   (or pseudo mass for boundary particles)
	float density;			// density			   rho
	float pressure;			// pressure			   P
	float temperature;		// temperature		   T
	float new_temperature;	// new temperature	   T+1
	float lambda;			// lagrange multiplier lambda
	float dummyA;			//available
	float dummyB;			//available
	float dummyC;			//available
	uint phase;				// phase (liquid, solid...)
	uint newIndex;			// sorted index
	uint binIndex;			// bin index
};

layout(std430) buffer ParticleBuffer {
	Particle particles[];
};

struct Bin {
	uint count;
	uint sum;
	uint csum;
	uint index;
};

layout(std430) buffer BinBuffer {
	Bin bins[];
};

#endif// INCLUDE_BUFFERS_HLSL