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
	vec3 lightPosition = normalize(point.lv_Position);

	// attenuation
	float dst = length(point.lv_Position - FragPos);
	float att = 1.0 / (point.lf_Constant + (point.lf_LinearParam * dst) + (point.lf_QuadParam * (dst * dst)));

	// ambient
	vec3 ambient = texture(material.ms_Diffuse, TexCoord).rgb * ambientParam;

	// diffuse
	vec3 diffuse = texture(material.ms_Diffuse, TexCoord).rgb * max(dot(lightPosition, normal), 0.0);

	// sepcular
	vec3 ref = reflect(-lightPosition, normal);
	vec3 viewDir = (point.lv_CameraPosition - FragPos);
	viewDir = normalize(viewDir);

	float spec = pow(max(dot(viewDir, ref), 0.0), material.mf_Shininess);//material.mShininess);
	vec3 specular = texture(material.ms_Diffuse, TexCoord).rgb * spec * normalize(texture(material.ms_Specular, TexCoord).rgb);

	ambient  *= point.lf_Power * att * material.mv_AmbientCoeff;
	diffuse  *= point.lf_Power * att * material.mv_DiffuseCoeff;
	specular *= point.lf_Power * att * material.mv_SpecularCoeff;

	return (ambient + diffuse + specular);
}