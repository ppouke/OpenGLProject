#version 330 core

out vec4 FragColor;
in vec3 Normal;
in vec2 texCoord;
in vec3 FragPos;



struct Material{
	sampler2D texture_specular1;
	sampler2D texture_diffuse1;
	float shininess;
};

struct DirLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform DirLight dirLight;
uniform vec3 viewPos;

	
vec3 CalcDirLight(DirLight light,vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir),0), material.shininess);

	vec3 ambient = light.ambient * texture(material.texture_diffuse1, texCoord).rgb;
	vec3 diffuse = light.diffuse * (diff * texture(material.texture_diffuse1, texCoord).rgb);
	vec3 specular = light.specular * texture(material.texture_specular1,texCoord).rgb * spec; 


	return ambient + specular + diffuse;
}

void main(){


	float clip = texture(material.texture_diffuse1, texCoord).a;
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result = CalcDirLight(dirLight, norm, viewDir);


	FragColor =  vec4(result, clip);

}