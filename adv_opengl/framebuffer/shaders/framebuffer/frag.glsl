#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture[1];

void main()
{
	vec3 col = texture(screenTexture[0], TexCoords).rgb;
	FragColor = vec4(col, 1.0);
}

