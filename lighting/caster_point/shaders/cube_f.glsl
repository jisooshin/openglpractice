#version 330 core
struct Material
{ 
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;


void main()
{
	//ambient lighting
	vec3 ambient  = texture(material.diffuse, TexCoords).rgb * light.ambient;

	// diffuse lighting
	vec3 norm = normalize(Normal);
	vec3 lightDirection  = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDirection), 0.0);
	vec3 diffuse = texture(material.diffuse, TexCoords).rgb * light.diffuse * diff;

	vec3 viewDirection = normalize(viewPos - FragPos);
	vec3 reflectDirection = reflect(-lightDirection, norm);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specular  = (spec  * texture(material.specular, TexCoords).rgb) * light.specular;

	vec3 result = (ambient + diffuse + specular);
	FragColor = vec4(result, 1.0);
}