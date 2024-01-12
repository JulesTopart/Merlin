#version 440 core
layout(location = 0) out vec4 o_Color;

in vec2 texCoord;
uniform sampler2D screen;
//uniform sampler2DMS  screen;
uniform int mode = 0;
void main() {
	if (mode == 1) {
		o_Color = vec4(vec3(texture(screen, texCoord).r), 1);

		float z = texture(screen, texCoord).r;      // fetch the z-value from our depth texture
		float n = 0.1f;                                // the near plane
		float f = 20.0f;                               // the far plane
		float c = (2.0 * n) / (f + n - z * (f - n));  // convert to linear values 

		if (c >= 0.98) o_Color = vec4(vec3(0, 0, 1), 1);                      // linear
		else o_Color = vec4(vec3(c, 0, 0), 1);                      // linear
	}
	else o_Color = texture(screen, texCoord);
}
