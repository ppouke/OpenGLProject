#version 330 core

struct Material{
	sampler2D texture_specular1;
	sampler2D texture_diffuse1;
	float shininess;
};

struct PointLight {


	vec3 position;
	//float cutOff;
	//float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	

	float constant;
	float linear;
	float quadratic;
};

struct DirLight{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

};

out vec4 FragColor;

in vec3 Normal;
in vec2 texCoord;
in vec3 FragPos;

uniform Material material;

#define NR_POINT_LIGHTS 4  
uniform PointLight pointLights[NR_POINT_LIGHTS];

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

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	float diff = max(dot(normal, lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
	
	
	
	float distance = length(light.position - fragPos);
	float attenuation = 1/(light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient = light.ambient * texture(material.texture_diffuse1, texCoord).rgb;
	vec3 diffuse = light.diffuse * (diff * texture(material.texture_diffuse1, texCoord).rgb);
	vec3 specular = light.specular * texture(material.texture_specular1,texCoord).rgb * spec;
	

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return ambient + diffuse + specular;


}



void main()
{
	// spotlight calcs
	
	//float theta = dot(lightDir, normalize(-light.direction));
	//float epsilon = light.cutOff - light.outerCutOff;
	//float intensity = clamp((theta - light.outerCutOff)/epsilon,0.0,1.0); 
	


	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = CalcDirLight(dirLight, norm, viewDir);

	for(int i = 0; i < NR_POINT_LIGHTS; i++){
		result += CalcPointLight(pointLights[i], norm, FragPos,  viewDir);
	}


	

	//vec3 emit = texture(material.emission, texCoord).xyz * clamp((1 - texture(material.texture_specular1, texCoord).r * 20), 0, 1);

	//attenuation

	

	

	//vec3 exDiff = texture(material.texture_diffuse1, texCoord).xyz;
	FragColor = vec4(result, 1.0f);
}