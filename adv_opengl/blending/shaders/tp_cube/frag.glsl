#version 330 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;


struct Material
{
	sampler2D ms_Diffuse;
	sampler2D ms_Specular;
	sampler2D ms_Bump;
	float mf_Shininess;
	vec3 mv_AmbientCoeff, mv_DiffuseCoeff, mv_SpecularCoeff;
};


uniform Material material[3]; // 일단 3개만 선언해놓고 판단


void main()
{
	
	vec4 texColor = texture(material[0].ms_Diffuse, TexCoord);
	if (texColor.a < 0.1) discard;
	FragColor = texColor;
}

