#version 330 core

layout (location = 0) in vec3 aPos;


struct tMatrix
{
	mat4 model, view, projection;
};

uniform tMatrix matrix;

void main()
{
	gl_Position = matrix.projection * matrix.view * matrix.model * vec4(aPos, 1.0);
	// gl_Position = vec4(aPos, 1.0);
}