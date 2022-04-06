#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

// 여러 개의 texture가 있을 경우에는 어떻게 하면 좋을까
uniform sampler2D texture_diffuse1;

void main()
{
	FragColor = texture(texture_diffuse1, TexCoord);
}