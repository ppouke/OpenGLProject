#version 330 core

struct Material{

	sampler2D diffuse;
	sampler2D specular;
	float shininess;
	sampler2D emission;
};

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 FragColor;

in vec3 vertexPos;
in vec3 Normal;
in vec2 texCoord;
in vec3 FragPos;

uniform Material material;
uniform Light light;


uniform vec3 viewPos;

void main()
{
	//calcs
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 lightDir = normalize(light.position - FragPos);  
	vec3 reflectDir = reflect(-lightDir, norm);


	float diff = max(dot(norm, lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
	

	vec3 ambient = light.ambient * texture(material.diffuse, texCoord).rgb;
	vec3 diffuse = light.diffuse * (diff * texture(material.diffuse, texCoord).rgb);
	vec3 specular = light.specular * texture(material.specular,texCoord).rgb * spec; 

	vec3 emit = texture(material.emission, texCoord).xyz;

	vec3 result = (ambient + diffuse + specular + emit);
	FragColor = vec4(result, 1.0f);
}