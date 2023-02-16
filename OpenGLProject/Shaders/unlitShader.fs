#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D texture_diffuse1;

void main(){
	vec3 col = texture(texture_diffuse1, texCoord).xyz;
	FragColor = vec4(Normal, 1.0f);
}