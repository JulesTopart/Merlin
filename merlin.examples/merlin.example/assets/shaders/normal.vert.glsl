#version 150
in vec4 gxl3d_Position;
in vec4 gxl3d_Normal;

out vec3 position;
out vec4 normal;

uniform vec3 viewPos;
uniform mat4 view;
uniform mat4 model;

void main()
{
  position = vec3(model * gxl3d_Position);
  normal = gxl3d_Normal;
  gl_Position = view * vec4(position, 1.0f);;
}