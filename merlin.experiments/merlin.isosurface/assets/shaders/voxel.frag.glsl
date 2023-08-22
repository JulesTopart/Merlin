#version 450

in vec3 position;
in vec3 offset;
in vec3 normal;
in vec3 color;
in vec2 texCoord;
in float opacity;

out vec4 FragColor;

void main() {
  if(opacity < 0.1) discard;
  FragColor = vec4(color, opacity);
}