#version 430 core

out vec4	FragColor;

in vec2	vertTexUV;

uniform sampler2D	uGColor;
uniform sampler2D	uGNormal;
uniform sampler2D	uGPosition;

uniform sampler2D	uShadowMap;
uniform sampler2D	uSSAO;

uniform vec3		uLightPos;
uniform vec3		uLightDir;

uniform mat4		uProjection;
uniform mat4		uView;
uniform mat4		uLightProjection;
uniform mat4		uLightView;

const float	shadowStrength = 0.6;
const float	ambientStrength = 0.4;
const vec3	lightColor = vec3(1.0);

void	main()
{
	vec3	color = texture(uGColor, vertTexUV).xyz;
	vec3	normal = normalize(texture(uGNormal, vertTexUV).xyz);
	vec3	fragPos = texture(uGPosition, vertTexUV).xyz;
	float	SSAO = texture(uSSAO, vertTexUV).r;

	mat4	inverseView = inverse(uView);
	vec3	worldPos = (inverseView * vec4(fragPos, 1.0)).xyz;

	vec3	ViewlightDir = mat3(uView) * uLightDir;
	float	diffuse = dot(normal, ViewlightDir);
	// FragColor = vec4(worldPos, 1.0);
	// FragColor = vec4(diffuse * color, 1.0);
	FragColor = vec4(color * (SSAO * diffuse), 1.0);
}