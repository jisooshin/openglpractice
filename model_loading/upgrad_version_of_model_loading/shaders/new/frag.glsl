#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	float shininess;
};

struct PointLight 
{
	float constant, linearParam, quadParam;
	vec3 ambient, diffuse, sepcular;
	vec3 position;
};

uniform Material material[3]; // 일단 3개만 선언해놓고 판단

void main()
{
	FragColor = texture(material[0].normal, TexCoord);
}

vec3 calculate_point_light(PointLight point, Material material)
{
	vec3 result;
	return result;
}