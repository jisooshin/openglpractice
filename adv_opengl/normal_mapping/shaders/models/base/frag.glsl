#version 330 core
layout (location = 0) out vec4 FragColor;


in VS_OUT
{
	vec2 TexCoord;
	vec3 Normal;
	vec3 FragPos;
	mat3 TBN;
} fs_in;

struct Material
{
	sampler2D ms_Diffuse;
	sampler2D ms_Specular;
	sampler2D ms_Normal;
	bool isExist_Diffuse;
	bool isExist_Specular;
	bool isExist_Normal;
	float mf_Shininess;
	vec3 mv_AmbientCoeff, mv_DiffuseCoeff, mv_SpecularCoeff;
};

struct PointLight 
{
	float lf_Constant, lf_LinearParam, lf_QuadParam;
	float lf_Power;
	vec3 lv_LightColor;
	vec3 lv_Position;
	vec3 lv_CameraPosition;
};

uniform Material material[3]; // 일단 3개만 선언해놓고 판단
uniform PointLight point;
uniform bool isNormalMap;

vec4 calculate_point_light(PointLight point, Material material);

void main()
{
	vec4 result = calculate_point_light(point, material[0]);
	FragColor = result;
}


vec4 calculate_point_light(PointLight point, Material material)
{
	// base 

	vec4 origin = texture(material.ms_Diffuse, fs_in.TexCoord);
	vec3 normal;
	if (isNormalMap)
	{
		normal = texture(material.ms_Normal, fs_in.TexCoord).rgb;
		normal = normalize(normal) * 2.0 - 1.0;
		normal = normalize(normal);
	}
	else
	{
		normal = normalize(fs_in.Normal);
	}
	vec3 lightPosition = normalize(point.lv_Position);

	// attenuation
	float dst = length(point.lv_Position - fs_in.FragPos);
	float att = 1.0 / (point.lf_Constant + (point.lf_LinearParam * dst) + (point.lf_QuadParam * (dst * dst)));

	// ambient
	vec3 ambient = texture(material.ms_Diffuse, fs_in.TexCoord).rgb;

	// diffuse
	vec3 diffuse = texture(material.ms_Diffuse, fs_in.TexCoord).rgb * max(dot(lightPosition, normal), 0.0);

	// sepcular
	vec3 ref = reflect(-lightPosition, normal);
	vec3 viewDir = (point.lv_CameraPosition - fs_in.FragPos);
	viewDir = normalize(viewDir);

	float spec = pow(max(dot(viewDir, ref), 0.0), material.mf_Shininess);
	vec3 specular = texture(material.ms_Diffuse, fs_in.TexCoord).rgb * spec * normalize(texture(material.ms_Specular, fs_in.TexCoord).rgb);

	ambient  *= att * material.mv_AmbientCoeff;
	diffuse  *= att * material.mv_DiffuseCoeff;
	specular *= att * material.mv_SpecularCoeff;


	vec4 _out = vec4((ambient + diffuse + specular) * point.lv_LightColor * point.lf_Power, origin.a);
	return _out;
}