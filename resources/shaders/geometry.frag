#version 430 core

layout (location = 0) out vec4	gColor;
layout (location = 1) out vec4	gNormal;
layout (location = 2) out vec4	gPosition;

uniform sampler2D	uAtlas;

in vec3	vertViewWorldPos;
in vec3	vertViewNormal;
in vec2	vertTexUV;

void	main()
{
	gNormal = vec4(normalize(vertViewNormal), 1.0);
	gPosition = vec4(vertViewWorldPos, 1.0);
	gColor = vec4(texture(uAtlas, vertTexUV).rgb, 1.0);
}