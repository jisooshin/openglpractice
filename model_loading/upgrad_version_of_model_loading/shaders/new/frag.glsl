#version 330 core

in vec2 TexCoord;
out vec4 gl_FragColor;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
	
};

uniform Material material[3]; // 일단 3개만 선언해놓고 판단

void main()
{
	gl_FragColor = texture(material[0].diffuse, TexCoord);
}