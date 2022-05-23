#version 330 core
in vec3 Normal;
layout (location = 0) out vec4 FragColor;

uniform vec3 light_position;
void main()
{

	vec3 diffuse = vec3(1.0) * max(dot(light_position, Normal), 0.0);
	FragColor = vec4(diffuse, 1.0);	
	// FragColor = vec4(1.0);	
}