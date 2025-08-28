#version 430 core

out vec4 FragColor;

uniform sampler2D	atlas;

in vec3	vert_color;
in vec2	vert_texUV;

void main()
{
	FragColor = vec4(texture(atlas, vert_texUV));
	// FragColor = vec4(vert_color, 1.0f);
}
