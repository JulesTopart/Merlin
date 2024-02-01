#version 440 core
out vec4 FragColor;

in vec2 texCoord;
uniform sampler2D screen;


struct Color {
    vec3 color;
    float value;
};

layout(std430, binding = 0) buffer ColorMapBuffer {
    Color colors[];
};

uniform uint colorCount;

vec4 heatMap(const float value) {
	float minValue = 0.0f;
	float maxValue = 1.0f;
	float v = (value - minValue) / (maxValue - minValue);
	vec4 color;
	color.a = 1.0f;
	if (colorCount == 0) return vec4(0,0,0,1);

	for (int i = 0; i < colorCount; i++) {
		Color currC = colors[i];
		if (v <= currC.value) {
			Color prevC = colors[max(0, i - 1)];
			float valueDiff = (prevC.value - currC.value);
			float fractBetween = (valueDiff == 0) ? 0 : (v - currC.value) / valueDiff;
			color.r = (prevC.color.r - currC.color.r) * fractBetween + currC.color.r;
			color.g = (prevC.color.g - currC.color.g) * fractBetween + currC.color.g;
			color.b = (prevC.color.b - currC.color.b) * fractBetween + currC.color.b;
			return color;
		}
	}
	color = vec4(colors[colorCount - 1].color, 1.0);
	return color;
}


void main() {
	
	//FragColor = heatMap(length(texture(screen, texCoord).x));
	//FragColor = heatMap(length(texture(screen, texCoord).w));
    FragColor = vec4(texture(screen, texCoord).xyz,1.0);
}
