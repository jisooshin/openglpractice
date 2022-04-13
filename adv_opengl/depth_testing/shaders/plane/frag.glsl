#version 330 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D floorTexture;

void main()
{
	FragColor = vec4(texture(floorTexture, TexCoord).rgb, 1.0);
}
