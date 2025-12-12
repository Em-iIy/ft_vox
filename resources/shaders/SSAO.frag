#version 430 core

out vec4	FragColor;

uniform sampler2D	uGNormal;
uniform sampler2D	uGPosition;

uniform vec3		uSamples[64];
uniform int			uSampleCount = 64;


in vec2	vertTexUV;

void	main()
{
	vec3	fragPos = texture(uGPosition, vertTexUV).xyz;
	vec3	normal = texture(uGNormal, vertTexUV).xyz;
}