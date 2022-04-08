#version 330 core

in vec2 TexCoord;
out vec4 gl_FragColor;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
	
};
// 여러 개의 texture가 있을 경우에는 어떻게 하면 좋을까
uniform Material material[3];

void main()
{
	gl_FragColor = texture(material[0].diffuse, TexCoord);
}