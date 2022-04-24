#version 330 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D planeTexture;

void main()
{
	FragColor = vec4(texture(planeTexture, TexCoord).rgb, 1.0);
	// FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}