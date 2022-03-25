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
	vec3 direction;
	float constant, linear, quadratic;
	float innerCutOff, outerCutOff;
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
	//////////////////////
	// ambient lighting //
	//////////////////////
	vec3 ambient  = texture(material.diffuse, TexCoords).rgb * light.ambient;

	//////////////////////
	// diffuse lighting // 
	//////////////////////
	// 각 fragment 에서의 normal
	vec3 norm = normalize(Normal);
	// fragment 에서 light source를 바라보는 벡터 만들기
	vec3 lightDirection  = normalize(light.position - FragPos);
	// diffuse intensity parameter
	float diff = max(dot(norm, lightDirection), 0.0);
	vec3 diffuse = texture(material.diffuse, TexCoords).rgb * light.diffuse * diff;

	///////////////////////
	// specular lighting //	
	///////////////////////
	vec3 viewDirection = normalize(viewPos - FragPos);
	vec3 reflectDirection = reflect(-lightDirection, norm);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specular  = (spec  * texture(material.specular, TexCoords).rgb) * light.specular;


	// spotlight (soft edges)
    float theta = dot(lightDirection, normalize(-light.direction)); 
    float epsilon = (light.innerCutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

	float dist = length(light.position - FragPos);
	float att  = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);

	vec3 result = (ambient + diffuse + specular) * att;
	FragColor = vec4(result, 1.0);
}