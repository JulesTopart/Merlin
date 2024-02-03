#version 440 core
out vec4 FragColor;
in vec2 texCoord;


void main() {
	
	//FragColor = heatMap(length(texture(screen, texCoord).x));
	//FragColor = heatMap(length(texture(screen, texCoord).w));
    FragColor = vec4(texCoord.xy,0.0,1.0);
}
