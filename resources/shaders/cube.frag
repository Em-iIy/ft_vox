#version 430 core

out vec4 FragColor;

uniform sampler2D	atlas;

in vec3	vert_world_pos;
in vec3	vert_normal;
in vec2	vert_texUV;

const float fogNear = 100.0;
const float fogFar = 160.0;

void main()
{
	float	dist = length(vert_world_pos);
	float	fogFactor = clamp((dist - fogNear) / (fogFar - fogNear), 0.0, 1.0);
	
	vec4	texColor = texture(atlas, vert_texUV);
	texColor.rgb = texColor.rgb * length(vert_normal);
	FragColor = vec4(texColor.rgb, texColor.a);
	FragColor.rgb = mix(texColor.rgb, vec3(0.4f, 0.7f, 0.9f), fogFactor);
}
