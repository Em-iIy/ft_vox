#version 430 core

layout (location = 0) in vec3	inPos;
layout (location = 1) in vec3	inNormal;
layout (location = 2) in vec2	inTexUV;

uniform mat4 uProjection;
uniform mat4 uModel;
uniform mat4 uView;

out vec3	vertViewWorldPos;
out vec3	vertViewNormal;
out vec2	vertTexUV;

void	main()
{
	// Calculate the view position and NDC* position
	vec4	tempPos = uView * uModel * vec4(inPos, 1.0);
	vertViewWorldPos = tempPos.xyz;
	gl_Position = uProjection * tempPos;

	// Calculate the normal based off of the view and model matrix
	vertViewNormal = transpose(inverse(mat3(uView * uModel))) * normalize(inNormal);

	vertTexUV = inTexUV;
}