#version 430 core

layout (location = 0) in vec3	inPos;
layout (location = 1) in vec3	inNormal;
layout (location = 2) in vec2	inTexUV;

uniform mat4	uProjection;
uniform mat4	uModel;
uniform mat4	uView;

out	vec3	viewDir;

void	main()
{
	vec4 pos = uProjection * uView * vec4(inPos, 1.0);
	gl_Position = pos;

	viewDir = inPos;
}
