#version 460 core

struct Particle {
	dvec3 position;
	dvec3 velocity;
	double temperature;
	double conductivity;
	double capacity;
	double density;
	double pressure;
	double mass;
};

layout (std430, binding = 1) buffer ParticleBuffer {
  Particle particles[];
};

layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 _normal;
layout (location = 2) in vec3 _color;
layout (location = 3) in vec2 _texCoord;

out vec3 position;
out vec3 offset;
out vec3 normal;
out vec3 color;
out vec2 texCoord;

uniform mat4 view;
uniform mat4 model;

void main() {
	offset = vec3(particles[gl_InstanceID].position);

	position = vec3(model * vec4((_position) + offset, 1.0f) );
	//color = normalize(offset);
	color = vec3(particles[gl_InstanceID].temperature);
	gl_Position = view * vec4(position, 1.0f);
}
