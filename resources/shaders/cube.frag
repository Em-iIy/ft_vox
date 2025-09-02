#version 430 core

out vec4 FragColor;

uniform sampler2D	atlas;

in vec3	vert_normal;
in vec2	vert_texUV;

void main()
{
	FragColor = vec4(texture(atlas, vert_texUV).rgba * length(vert_normal));
	// FragColor = vec4(vert_normal, 1.0f);
}
