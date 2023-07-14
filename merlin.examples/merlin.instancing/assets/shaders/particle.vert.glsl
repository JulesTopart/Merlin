
#version 460 core

struct Particle {
	vec3 position;      // position (x, y, z)
	float mass;			// mass(w)

	vec3 velocity;      // velocity (x, y, z) and density (w)
	float density;		// density g/m3

	vec3 stressTensorX; // stress tensor (xx, xy, xz)
	float temperature;	// temperature (K)

	vec3 stressTensorY; // stress tensor (yx, yy, yz, 0)
	float pressure;		// pressure (Pa)

	vec3 stressTensorZ; // stress tensor (zx, zy, zz, 0)
	float strain;		// strain (Pa)
};

layout(std430, binding = 1) buffer ParticleBuffer {
	Particle particles[];
};

layout(location = 0) in vec3 _position;
layout(location = 1) in vec3 _normal;
layout(location = 2) in vec3 _color;
layout(location = 3) in vec2 _texCoord;

out vec3 position;
out vec3 normal;
out vec3 color;
out vec2 texCoord;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main() {

	vec3 water = vec3(0.05, 0.2, 1.0);
	vec3 foam = vec3(1, 1, 1);

	vec3 offset = particles[gl_InstanceID].position.xyz;
	position = vec3(model * vec4((_position) + offset, 1.0f));
	color = mix(foam, water, particles[gl_InstanceID].density / 1000.0);
	normal = _normal;
	texCoord = _texCoord;

	gl_Position = projection * view * vec4(position, 1.0f);
}
