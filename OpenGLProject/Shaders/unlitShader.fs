#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 texCoord;
out vec4 FragColor;

uniform vec3 col;


void main(){
	
	FragColor = vec4(col, 1.0f);
}