#version 460

layout (local_size_x = 1, local_size_y = 1) in;

layout(rg32f, binding = 0) uniform image2D velocity;	//Velocity field u velocity, v velocity
layout(r32f, binding = 1) uniform image2D phase;		//Phase field
layout(r32f, binding = 2) uniform image2D pressure;		//Pressure field

uniform float width;
uniform float height;
uniform float dt;
uniform float speed;

vec4 bilinearInterpolation(vec4 v00, vec4 v10, vec4 v01, vec4 v11, float s, float t) {
    return mix(mix(v00, v10, s), mix(v01, v11, s), t);
}

void advect(int x, int y) {
    vec2 pos = vec2(x, y);
    vec2 vel = imageLoad(velocity, ivec2(pos)).xy;

    // Compute the backtracked position
    vec2 posBack = pos - dt * speed * vel / 100.0;

    // Clamp to border
    posBack = clamp(posBack, vec2(0.0), vec2(width-1, height-1));

    // Fetch the four neighboring velocity values
    ivec2 iPos = ivec2(floor(posBack));
    vec4 v00 = imageLoad(velocity, iPos);
    vec4 v10 = imageLoad(velocity, iPos + ivec2(1, 0));
    vec4 v01 = imageLoad(velocity, iPos + ivec2(0, 1));
    vec4 v11 = imageLoad(velocity, iPos + ivec2(1, 1));

    // Interpolate
    vec2 fPos = fract(posBack);
    vec4 result = bilinearInterpolation(v00, v10, v01, v11, fPos.x, fPos.y);

    // Store the result in the current cell, only advecting velocity components
    vec4 current = imageLoad(velocity, ivec2(pos));
    current.x = result.x;
    current.y = result.y;
    imageStore(velocity, ivec2(pos), current);
}

void main() {
    ivec2 iPos = ivec2(gl_GlobalInvocationID.xy);
    if (imageLoad(phase, iPos).x == 0 || iPos.x == 0 || iPos.y == 0 || iPos.x == width-1 || iPos.y == height-1) return; //boundary
    advect(iPos.x, iPos.y);
}
