#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float width_offset;
uniform float height_offset;

vec4 make_gray(vec4 color);
vec4 original();
vec4 gaussianBlur();

void main()
{
	FragColor = make_gray(gaussianBlur());
}

vec4 original()
{
	return vec4(texture(screenTexture, TexCoords).rgb, 1.0);
}

vec4 make_gray(vec4 color)
{
	float average = (0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b);
	return vec4(vec3(average), 1.0);
}

vec4 gaussianBlur()
{
	vec2 offsets[9] = vec2[](
		vec2(-width_offset, height_offset),
		vec2(0.0, height_offset),
		vec2(width_offset, height_offset),

		vec2(-width_offset, 0.0),
		vec2(0.0, 0.0),
		vec2(width_offset, 0.0),

		vec2(-width_offset, -height_offset),
		vec2(0.0, -height_offset),
		vec2(width_offset, -height_offset)
	);
	float kernel[9] = float[](
		1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
		2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
		1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
	);

	vec3 sampleTex[9];
	for (int i = 0; i < 9; i++)
	{
		sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
	}
	vec3 color = vec3(0.0);
	for (int i = 0; i < 9; i++)
	{
		color += sampleTex[i] * kernel[i];
	}

	return vec4(color, 1.0);
}