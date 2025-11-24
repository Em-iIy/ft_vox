#version 430 core

layout (location = 0) in vec3	inPos;
layout (location = 1) in vec3	inNormal;
layout (location = 2) in vec2	inTexUV;

uniform mat4 uProjection;
uniform mat4 uModel;
uniform mat4 uView;

uniform mat4 uLightProjection;
uniform mat4 uLightView;

out vec3	vertWorldPos;
out vec4	vertLightPos;
out vec3	vertNormal;
out	vec2	vertTexUV;

void	main()
{
	vertNormal = inNormal;
	vertTexUV = inTexUV;
	vec4 worldPos = uModel * vec4(inPos, 1.0);
	vertWorldPos = worldPos.xyz;
	gl_Position = uProjection * uView * worldPos;
	vertLightPos = uLightProjection * uLightView * worldPos;
}
