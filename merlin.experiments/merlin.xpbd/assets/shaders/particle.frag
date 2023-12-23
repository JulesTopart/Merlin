#version 450
in vec4 position;
in vec4 screen_position;
//flat in vec3 normal;
in vec4 normal;
in vec4 color;
in vec2 texCoord;

out vec4 FragColor;

uniform vec3 lightPosition = vec3(10); // Position of the light source
uniform vec3 lightColor = vec3(1.0);    // Color of the light

void sprite() {
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
    vec3 ambient = 0.3 * lightColor;
    vec3 diffuse = 0.8*diff * lightColor;
    vec3 specular = vec3(0.0); // Assuming no specular component for simplicity

    // Calculate final color
    vec4 finalColor = vec4(ambient + diffuse + specular, 1.0) * color;

    // Apply smoothstep for alpha blending at the edges
	//finalColor.w = smoothstep(1, 0,length(center)*length(center));
	

    // Output the final color
    FragColor = finalColor;
}

void main() {
	FragColor = color;
    if(gl_PointCoord != vec2(0)) sprite();
}


