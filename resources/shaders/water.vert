#version 430

layout (location = 0) in vec3	inPos;
layout (location = 1) in vec3	inNormal;
layout (location = 2) in vec2	inTexUV;

out vec2	vertTexUV;

void	main()
{
	vertTexUV = inTexUV;
	gl_Position = vec4(inPos, 1.0f);
}