#version 430 core

out vec4 FragColor;

uniform sampler2D	atlas;
uniform float		uFogNear;
uniform float		uFogFar;
uniform vec3		uFogColor;

in vec3	vert_world_pos;
in vec3	vert_normal;
in vec2	vert_texUV;

void main()
{
	float	dist = length(vert_world_pos);
	float	fogFactor = clamp((dist - uFogNear) / (uFogFar - uFogNear), 0.0, 1.0);
	
	vec4	texColor = texture(atlas, vert_texUV);
	texColor.rgb = texColor.rgb * length(vert_normal);
	FragColor = vec4(texColor.rgb, texColor.a);
	FragColor.rgb = mix(texColor.rgb, uFogColor, fogFactor);
}
