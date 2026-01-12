#version 430 core

out vec4	FragColor;

uniform sampler2D	uGColor;
uniform sampler2D	uGNormal;
uniform sampler2D	uGPosition;

uniform sampler2D	uShadowMap;
uniform sampler2D	uSSAO;
uniform float		uFogNear;
uniform float		uFogFar;
uniform vec3		uFogColor;
uniform vec3		uLightPos;
uniform vec3		uLightDir;
uniform int			uLightingMode;

uniform mat4		uProjection;

in vec2	vertTexUV;

const float	shadowStrength = 0.6;
const float	ambientStrength = 0.4;
const vec3	lightColor = vec3(1.0);

float	shadowMapCalculation()
{
	vec3	projectionCoords = vertLightPos.xyz / vertLightPos.w;

	projectionCoords = projectionCoords / 2 + 0.5;

	float	shadowDepth = texture(uShadowMap, projectionCoords.xy).r;
	float	currentDepth = projectionCoords.z;

	float	bias = 0.0005;

	float	shadow = 0.0;
	vec2	texelSize = 1.0 / textureSize(uShadowMap, 0);
	for(int x = -2; x <= 2; ++x)
	{
		for(int y = -2; y <= 2; ++y)
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
	if (uLightingMode == 0)
	{
		vec3	lighting = (ambient + (1.0 - shadow) * diffuse) * texColor;
		return (vec4(lighting, 1.0));
	}
	else if (uLightingMode == 1)
		return (vec4(ambient * texColor, 1.0));
	else if (uLightingMode == 2)
		return (vec4(diffuse * texColor, 1.0));
	else if (uLightingMode == 3)
		return (vec4((1.0 - shadow) * texColor, 1.0));
	else
		return (vec4(texColor, 1.0));
}

void	main()
{
	vec3	color = texture(uGColor, vertTexUV).xyz;
	vec3	normal = texture(uGNormal, vertTexUV).xyz;
	vec3	fragPos = texture(uGPosition, vertTexUV).xyz;

	
}
