#version 430 core

layout (location = 0) in vec3	inPos;
layout (location = 1) in vec3	inNormal;
layout (location = 2) in vec2	inTexUV;

uniform mat4	uProjection;
uniform mat4	uModel;
uniform mat4	uView;

out vec2	vertTexUv;

void	main()
{
	gl_Position = uProjection * uView * uModel * vec4(inPos, 1.0);
	vertTexUv = inTexUV;
}
