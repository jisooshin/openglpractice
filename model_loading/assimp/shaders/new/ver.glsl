#version 330 core

layout (location = 0) in vec aPos;
layout (location = 1) in vec aNormal;
layout (location = 2) in vec aTexCoord;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;
out vec3 TexCoord;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 0.0);
	Normal = vec3(transpose(inverse(model)) * vec4(aNormal, 0.0));
	FragPos = vec3(model * vec4(aPos, 0.0));
	TexCoord = aTexCoord;
}