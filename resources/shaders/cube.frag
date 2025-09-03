#version 430 core

out vec4 FragColor;

uniform sampler2D	atlas;

in vec3	vert_normal;
in vec2	vert_texUV;

void main()
{
	vec4	texColor = texture(atlas, vert_texUV);
	FragColor = vec4(texColor.rgb * length(vert_normal), texColor.a);
	// FragColor = vec4(vert_normal, 1.0f);
}
