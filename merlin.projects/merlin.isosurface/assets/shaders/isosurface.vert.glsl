#version 450 core

out vec3 position;
out vec3 normal;
out vec3 color;

uniform mat4 view;
uniform mat4 model;

struct Vertex {
  vec3 pos;
  float scale;
  vec3 normal;
  float size;
};

layout(std430, binding = 2) buffer GeomBuffer {
  Vertex vertices[];
};


void main() {
	vec3 position = vec3(model * vec4(vertices[gl_VertexID].pos, 1.0f));
	vec3 color = vertices[gl_VertexID].normal;
	vec3 normal = vertices[gl_VertexID].normal;

	gl_Position = view * vec4(position, 1.0f);
}
