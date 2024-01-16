#version 450
in vec4 position;
in vec4 screen_position;
//flat in vec3 normal;
in vec4 normal;
in vec4 color;
in vec2 texCoord;

out vec4 FragColor;

 
// --- POLY6 KERNEL ---
//Computed the poly6 scalar smoothing kernel


void main() {
	FragColor = color;

	if(gl_PointCoord == vec2(0)){
		FragColor.w = 1;
	}else{
		vec2 center = (gl_PointCoord - vec2(0.5))*2;
		if(length(center) > 1.0) discard;
		FragColor.w = smoothstep(1, 0,length(center));
	}
}