#version 330 core

out vec4 FragColor;

float near  = 0.1;
float far = 10.0;

float LinearizeDepth(float depth){
	float ndc = depth * 2.0 - 1;
	float linearDepth = (2.0 * near * far) / (far + near - ndc * (far - near));
	return linearDepth;
}

void main(){

	
	float linearDepth = LinearizeDepth(gl_FragCoord.z) / far;
	FragColor = vec4(vec3(linearDepth), 1.0);
}