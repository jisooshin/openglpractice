#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

vec4 make_gray();

void main()
{
	FragColor = make_gray();
}

vec4 make_gray()
{
	vec3 color = texture(screenTexture, TexCoords).rgb;
	float average = (0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b);
	return vec4(vec3(average), 1.0);
}