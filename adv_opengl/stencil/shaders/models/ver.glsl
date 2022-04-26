#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

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
}