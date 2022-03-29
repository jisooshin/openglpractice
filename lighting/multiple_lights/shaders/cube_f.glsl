#version 330 core

#define NUMBER_POINT_LIGHTS 4
struct Material
{ 
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirectionalLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
struct PointLight
{
	float constant, linearParam, quadraticParam;
	vec3 ambient, diffuse, specular;
	vec3 position;
};

uniform vec3 viewPos;
uniform Material material;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[NUMBER_POINT_LIGHTS];

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

vec3 calculate_directional_light(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 calculate_point_light(PointLight light, vec3 normal, vec3 viewDir);

void main()
{
	vec3 result = calculate_directional_light(directionalLight, Normal, viewPos);
	for (int i = 0; i < 4; i++)
	{
		result += calculate_point_light(pointLights[i], Normal, viewPos);
	}

	FragColor = vec4(result, 1.0);
}


vec3 calculate_directional_light(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	normal = normalize(normal);
	viewDir = normalize(viewDir);
	// diffuse
	vec3 lightDir = normalize(light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	
	// specular
	vec3 reflectDir = normalize(reflect(-lightDir, normal));
	float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * vec3(texture(material.diffuse, TexCoords)) * diff;
	vec3 specular = light.specular * vec3(texture(material.specular, TexCoords)) * spec;
	return ambient + diffuse + specular;
}

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 viewDir)
{
	normal = normalize(normal);
	viewDir = normalize(viewDir);
	// diffuse light
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	// specular 
	vec3 reflectDir = normalize(reflect(-lightDir, normal));
	float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	vec3 diffuse = light.diffuse * texture(material.diffuse, TexCoords).rgb * diff;
	vec3 specular = light.specular * texture(material.specular, TexCoords).rgb * spec;
	// Applying attenuation
	float dist = distance(FragPos, light.position);
	float attenuation = 1.0 / 
	(light.constant + (light.linearParam * dist) + (light.quadraticParam * (dist * dist)));
	
	return (ambient + diffuse + specular) * attenuation;
}