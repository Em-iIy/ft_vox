#version 430 core

out vec4 FragColor;

uniform vec3 color;

in vec3	vert_color;

void main()
{
	FragColor = vec4(vert_color, 1.0f);
}
