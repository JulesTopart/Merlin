#version 450 core
layout (location = 0) out vec4 o_Color;
in vec2 texCoord;

struct Color {
	vec3 color;
    float value;
};

layout (std430, binding = 2) buffer ColorMapBuffer {
  Color colors[];
};

uniform uint colorCount;
vec3 heatMap(const double value){
    float v = float(value);
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

void main(){
	o_Color = vec4(heatMap(texCoord.y), 1.0f);
}