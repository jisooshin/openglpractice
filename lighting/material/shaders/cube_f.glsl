#version 330 core
struct Material { vec3 ambient, diffuse, specular; float shininess; };
struct Light { vec3 position, ambient, diffuse, specular; };

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;


void main()
{
	vec3 norm = normalize(Normal);

	//ambient lighting
	float ambientStrength = 0.1;
	vec3 ambient = material.ambient * light.ambient;
	
	// diffuse lighting
	vec3 lightDirection = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDirection), 0.0);
	vec3 diffuse = (diff * material.diffuse) * light.diffuse;

	vec3 viewDirection = normalize(viewPos - FragPos);
	vec3 reflectDirection = reflect(-lightDirection, norm);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 64.0);
	vec3 specular = (spec *  material.specular) * light.specular;

	vec3 result = (ambient + specular) * vec3(0.3, 0.5, 0.2);
	FragColor = vec4(result, 1.0);
}