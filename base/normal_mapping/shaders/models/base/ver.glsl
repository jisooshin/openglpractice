#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

struct tMatrix
{
	mat4 model, view, projection;
};

out VS_OUT
{
	vec2 TexCoord;
	vec3 Normal;
	vec3 FragPos;
	mat3 TBN;
} vs_out;

uniform tMatrix matrix;


void main()
{
	gl_Position = matrix.projection * matrix.view * matrix.model * vec4(aPos, 1.0);
	vs_out.Normal = vec3(transpose(inverse(matrix.model)) * vec4(aNormal, 0.0));
	vs_out.TexCoord = aTexCoord;
	vs_out.FragPos = aPos;

	vec3 T = normalize(vec3(matrix.model * vec4(aTangent, 0.0)));
	vec3 N = normalize(vec3(matrix.model * vec4(aNormal, 0.0)));
	vec3 B = cross(N, T);
	// normalize(vec3(matrix.model * vec4(aBitangent, 0.0)));
	vs_out.TBN = mat3(T, B, N);
}