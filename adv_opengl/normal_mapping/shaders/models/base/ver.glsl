#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

struct tMatrix
{
	mat4 model, view, projection;
};

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform tMatrix matrix;


void main()
{
	gl_Position = matrix.projection * matrix.view * matrix.model * vec4(aPos, 1.0);
	Normal = vec3(transpose(inverse(matrix.model)) * vec4(aNormal, 0.0));
	TexCoord = aTexCoord;
	FragPos = aPos;

	vec3 T = normalize(vec3(matrix.model * vec4(aTangent, 0.0)));
	vec3 B = normalize(vec3(matrix.model * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(matrix.model * vec4(aNormal, 0.0)));
	mat3 TBN = mat3(T, B, N);
}