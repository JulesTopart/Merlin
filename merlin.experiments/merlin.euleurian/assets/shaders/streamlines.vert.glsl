#version 460 core

struct FieldProperty {
	vec3 u;		//Velocity
	float p;	//Pressure

	float d;	//Divergence
	float m;	//mass
	float t;	//Temperature
	float s;	//Grid element type (solid, liquid...)
};

struct Color {
	vec3 color;
	float value;
};

layout(std430, binding = 1) buffer FieldBuffer {
	FieldProperty field[];
};

layout(std430, binding = 2) buffer NewFieldBuffer {
	FieldProperty newField[];
};

layout(std430, binding = 3) buffer ColorMapBuffer {
	Color colors[];
};

layout(location = 0) in vec3 _position; //position


out vec3 position;
out vec3 color;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

uniform float grid;
uniform float gridSpacing;

uniform uint colorCount;

vec3 heatMap(const float value) {
	float minValue = 0.0f;
	float maxValue = 1.0f;
	float v = (value - minValue) / (maxValue - minValue);
	vec3 color;
	if (colorCount == 0) return vec3(0);

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
	color = colors[colorCount - 1].color;
	return color;
}


void main() {
	uint gid = gl_InstanceID;
	float ix = mod(gid, grid);
	float iy = mod(gid, (grid * grid)) / grid;
	float iz = gid / (grid * grid); 

	vec3 start = vec3((ix - grid/2.0) * gridSpacing, (iy - grid / 2.0) * gridSpacing, iz * gridSpacing);
	vec3 end = start + normalize(field[gid].u)*0.08;

	if (length(_position) == 0.0) {
		position = start;
	}else {
		position = end;
	}
	color = heatMap(abs(length(field[gid].u)));
	//color = heatMap(abs(field[gid].p));
	gl_Position = model * projection * view * vec4(position, 1.0f);
}
