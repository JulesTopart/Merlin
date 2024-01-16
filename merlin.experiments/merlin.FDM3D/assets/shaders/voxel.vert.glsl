#version 450

struct Node {
    vec3 U;		//Position
	vec3 V;		//Velocity

	float h;	//Smoothing radius
	float d;	//Density
	float p;	//Pressure
	float T;	//Temperature

	vec4 S;		//stress [Sxx, Sxy, Syy, Szz]
	vec4 R;		//artificial Stress [Rxx, Rxy, Ryy, -]
	
	vec4 S_d;	//dS(u)/du Stress spatial derivative
	vec4 V_d;	//dV(u)/du ? spatial derivative

	//plastic strain and strain rate
	float eps_pl;
	float eps_pl_dot;

	//Time derivatives
	vec3 U_t;	//dU(t)/dt
	vec3 V_t;	//dV(t)/dt

	float d_t;	//dd(t)/dt
	vec4  S_t;	//dS(t)/dt
	float T_t;	//dT(t)/dt

	//Boundary condition
	int fix;				//particle is fixed in space
	int enable;			//particle is deactivated

	uint index;
};

struct Color {
	vec3 color;
    float value;
};


layout (std430, binding = 0) buffer ColorMapBuffer {
  Color colors[];
};

layout (std430, binding = 1) buffer NodeBuffer {
  Node nodes[];
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
out float opacity;

uniform mat4 view;
uniform mat4 model;
uniform float scale;
uniform int mode;

uniform uint colorCount;
vec3 heatMap(const double value){
    float minValue = 20.0f;
    float maxValue = 200.0f;
    float v = (float(value) - minValue)/(maxValue-minValue);
    vec3 color;
    if(colorCount == 0) return vec3(0);
    
    for(int i=0; i < colorCount; i++){
        Color currC = colors[i];
        if(v < currC.value){
            Color prevC  = colors[max(0,i-1)];
            float valueDiff = (prevC.value - currC.value);
            float fractBetween = (valueDiff == 0) ? 0 : (v - currC.value) / valueDiff;
            color.r = (prevC.color.r - currC.color.r) * fractBetween + currC.color.r;
            color.g = (prevC.color.g - currC.color.g) * fractBetween + currC.color.g;
            color.b = (prevC.color.b - currC.color.b) * fractBetween + currC.color.b;
            return color;
        }
    }
    color = colors[colorCount-1].color;
    return color;
}

void main() {
	position = vec3(model * vec4((_position + nodes[gl_InstanceID].U)*scale, 1.0f));
	if(nodes[gl_InstanceID].enable == 1) color = heatMap(nodes[gl_InstanceID].T); //Divide by max temperature for color mapping
	if(nodes[gl_InstanceID].enable == 0) color = vec3(1);
	normal = _normal;
	opacity = ((nodes[gl_InstanceID].T*nodes[gl_InstanceID].T)/(90.0f*90.0f));
	if(nodes[gl_InstanceID].enable == 0) opacity = 0.0f;

	gl_Position = view * vec4(position, 1.0f);
}
