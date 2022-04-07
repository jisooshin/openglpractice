#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

// 여러 개의 texture가 있을 경우에는 어떻게 하면 좋을까
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
	gl_FragColor = texture(texture_diffuse1, TexCoord);
}