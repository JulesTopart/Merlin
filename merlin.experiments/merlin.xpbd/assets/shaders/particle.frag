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

/*
void main() {
	FragColor = color;

	if(gl_PointCoord == vec2(0)){
		FragColor.w = 1;
	}else{
		vec2 center = (gl_PointCoord - vec2(0.5))*2;
		if(length(center) > 1.0) discard;
		FragColor.w = smoothstep(1, 0,length(center));
	}
}*/


// Fragment shader
uniform vec3 lightPosition = vec3(200,100,300); // Position of the light source
uniform vec3 lightColor = vec3(1);    // Color of the light

void main() {
    // Calculate the center and radius of the sphere
    vec2 center = (gl_PointCoord - vec2(0.5)) * 2.0;
    float radius = length(center);

    // Discard fragments outside of the sphere's radius
    if (radius > 1.0) discard;

    // Calculate the normal vector for the sphere surface at this fragment
    vec3 normal = normalize(vec3(center.x, center.y, sqrt(1.0 - radius * radius)));

    // Light calculations
    vec3 lightDir = normalize(lightPosition - gl_FragCoord.xyz);
    float diff = max(dot(normal, lightDir), 0.0);

    // Ambient, Diffuse, and Specular Components
    vec3 ambient = 0.1 * lightColor;
    vec3 diffuse = diff * lightColor;
    vec3 specular = vec3(0.0); // Assuming no specular component for simplicity

    // Calculate final color
    vec4 finalColor = vec4((ambient + diffuse + specular, 1.0)) * color;

    // Apply smoothstep for alpha blending at the edges
    if(gl_PointCoord == vec2(0)){
		FragColor.w = 1;
	}else{
		vec2 center = (gl_PointCoord - vec2(0.5))*2;
		if(length(center) > 1.0) discard;
		FragColor.w = smoothstep(1, 0,length(center));
	}

    // Output the final color
    FragColor = finalColor;
}