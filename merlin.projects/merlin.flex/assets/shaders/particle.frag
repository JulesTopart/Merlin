#version 450 core

in vec3 position;
//flat in vec3 normal;
in vec3 normal;
in vec4 color;
in vec2 texCoord;

out vec4 FragColor;



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

float near = 0.1; 
float far  = 90.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() {

	vec4 lightDir = normalize(vec4(1, 1, -2, 1.0));
	vec4 lightColor = vec4(1.0f, 1.0f, 1.0f,1.0);

	vec4 n = normalize(vec4(normal,1.0));

	vec4 ambient = lightColor * 0.5f;
	vec4 diffuse = lightColor * max(dot(n, lightDir), 0.0);
	//float specular = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	FragColor = heatMap(color.r);
	FragColor.w = 0.5;
	//FragColor = (ambient + diffuse) * color;


	//float depth = LinearizeDepth(gl_FragCoord.z) / far;
	//FragColor = vec4(vec3(depth), 1.0);
}