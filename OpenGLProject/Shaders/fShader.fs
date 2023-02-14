#version 330 core

struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material material;
out vec4 FragColor;

in vec3 vertexPos;
in vec3 Normal;
in vec2 texCoord;
in vec3 FragPos;


uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixValue;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	//calcs
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 lightDir = normalize(lightPos - FragPos);  
	vec3 reflectDir = reflect(-lightDir, norm);


	float diff = max(dot(norm, lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
	
	//Read tex
	vec3 col = texture(texture1, texCoord).xyz;


	vec3 ambient = material.ambient * lightColor * 0.1f;
	vec3 diffuse = (diff * material.diffuse) * lightColor;
	vec3 specular = material.specular * spec * lightColor; 

	vec3 result = (ambient + diffuse + specular) * col;
	FragColor = vec4(result, 1.0f);
}