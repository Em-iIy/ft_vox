#version 430 core

out vec4	FragColor;

in vec2	vertTexUV;

uniform sampler2D	uGColor;
uniform sampler2D	uGNormal;
uniform sampler2D	uGPosition;

uniform sampler2D	uShadowMap;
uniform sampler2D	uSSAO;

uniform mat4		uProjection;
uniform mat4		uView;

uniform mat4		uLightProjection;
uniform mat4		uLightView;
uniform vec3		uLightDir;

uniform bool		uIsWater;

const float	shadowStrength = 0.6;
const float	ambientStrength = 0.4;
const vec3	lightColor = vec3(1.0);

float	shadowMapCalculation(vec4 worldPos)
{
	vec4	posLightSpace = uLightProjection * uLightView * worldPos;
	vec3	projectionCoords = posLightSpace.xyz / posLightSpace.w;

	projectionCoords = projectionCoords / 2 + 0.5;

	float	shadowDepth = texture(uShadowMap, projectionCoords.xy).r;
	float	currentDepth = projectionCoords.z;

	const float	bias = 0.0005;

	float	shadow = 0.0;
	vec2	texelSize = 1.0 / textureSize(uShadowMap, 0);
	for (int x = -2; x <= 2; ++x)
	{
		for (int y = -2; y <= 2; ++y)
		{
			float pcfDepth = texture(uShadowMap, projectionCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 25.0;

	return (shadow);
}

vec4	lightCalculation(vec3 ambient, float shadow, vec3 diffuse, vec3 texColor)
{
	vec3	lighting = (ambient + (1.0 - shadow) * diffuse) * texColor;
	return (vec4(lighting, 1.0));
}

void	main()
{
	vec3	color = texture(uGColor, vertTexUV).xyz;
	vec3	normal = normalize(texture(uGNormal, vertTexUV).xyz);
	vec3	fragPos = texture(uGPosition, vertTexUV).xyz;
	float	SSAO = texture(uSSAO, vertTexUV).r;

	mat4	inverseView = inverse(uView);
	vec4	worldPos = inverseView * vec4(fragPos, 1.0);

	vec3	ViewlightDir = mat3(uView) * uLightDir;
	float	diffuseAngle = dot(normal, ViewlightDir);
	float	horizonFade = clamp((uLightDir.y + 0.1) / 0.3, 0.0, 1.0);

	vec3	diffuse = max(diffuseAngle, 0.0) * horizonFade * lightColor;
	vec3	ambient = ambientStrength * lightColor;

	float	shadow = shadowMapCalculation(worldPos);
	if (diffuseAngle < 0.0)
		shadow = shadowStrength;
	shadow *= horizonFade;

	FragColor = lightCalculation(ambient, shadow, diffuse, color);
	if (uIsWater == false)
		FragColor *= SSAO;
}