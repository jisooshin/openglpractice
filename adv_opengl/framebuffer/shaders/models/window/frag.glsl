#version 330 core
layout (location = 0) out vec4 FragColor;


struct Material
{
	sampler2D ms_Diffuse;
	sampler2D ms_Specular;
	sampler2D ms_Bump;
	float mf_Shininess;
	vec3 mv_AmbientCoeff, mv_DiffuseCoeff, mv_SpecularCoeff;
};

in vec2 TexCoord;

uniform Material material[2];

void main()
{
	vec4 result = texture(material[0].ms_Diffuse, TexCoord);
	FragColor = result;
}

