#version 330 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;


struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D bump;
	float shininess;
};

struct PointLight 
{
	float constant, linearParam, quadParam, shiness;
	vec3 ambient, diffuse, sepcular;
	vec3 position;
	vec3 cameraPosition;
};

uniform Material material[3]; // 일단 3개만 선언해놓고 판단
uniform PointLight point;

vec3 calculate_point_light(PointLight point, Material material, float ambientParam = 0.2);

void main()
{
	vec3 result = calculate_point_light(point, material[0]);
	FragColor = vec4(result, 1.0);
}




vec3 calculate_point_light(PointLight point, Material material, float ambientParam = 0.2)
{
	// base 
	vec3 normal = normalize(Normal);
	vec3 lightPosition = normalize(point.position);

	// attenuation
	float dst = length(point.position - FragPos);
	float att = 1.0 / (point.constant + (point.linearParam * dst) + (point.quadParam * (dst * dst)));

	// ambient
	vec3 ambient = texture(material.diffuse, TexCoord).rgb * ambientParam;

	// diffuse
	vec3 diffuse = texture(material.diffuse, TexCoord).rgb * max(dot(lightPosition, normal), 0.0);

	// sepcular
	vec3 ref = reflect(-lightPosition, normal);
	vec3 viewDir = (point.cameraPosition - FragPos);
	viewDir = normalize(viewDir);

	float spec = pow(max(dot(viewDir, ref), 0.0), point.shiness);
	vec3 specular = texture(material.diffuse, TexCoord).rgb * spec * normalize(texture(material.specular, TexCoord).rgb);

	ambient  *= att;
	diffuse  *= att;
	specular *= att;

	return (ambient + diffuse + specular);
}