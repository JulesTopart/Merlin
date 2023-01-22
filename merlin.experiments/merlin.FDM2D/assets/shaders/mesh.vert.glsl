#version 450

struct Node {
	double temperature;
    vec3 position;
};

struct Color {
	vec3 color;
    float value;
};

layout (std430, binding = 1) buffer NodeBuffer {
  Node nodes[];
};

layout (std430, binding = 2) buffer ColorMapBuffer {
  Color colors[];
};

layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 _normal;
layout (location = 2) in vec3 _color;
layout (location = 3) in vec2 _texCoord;

out vec3 position;
out vec3 offset;
out vec3 normal;
out vec3 color;
//out flat vec3 color;
out vec2 texCoord;

uniform mat4 view;
uniform mat4 model;
uniform float scale;
uniform int mode;

uniform uint colorCount;
vec3 heatMap(const double value){
    float minValue = 20.0f;
    float maxValue = 60.0f;
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
	position = vec3(model * vec4(_position*scale, 1.0f));
	if(mode == 0) color = heatMap(nodes[gl_VertexID].temperature); //Divide by max temperature for color mapping
	if(mode == 1) color = vec3(1); //Divide by max temperature for color mapping
	normal = _normal;

	gl_Position = view * vec4(position, 1.0f);
}


/*
vec3 heatMap(double v){
    float level = float(v*3.14159265/2.);
    // Time varying pixel color
   
    vec3 col;
    col.r = sin(level);
    col.g = sin(level*2.);
    col.b = cos(level);
    return col;
}
*/