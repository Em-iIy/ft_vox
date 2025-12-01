#version 430 core

out vec4	FragColor;

uniform sampler2D	uAtlas;
uniform float		uFogNear;
uniform float		uFogFar;
uniform vec3		uFogColor;
uniform vec3		uLightPos;
uniform vec3		uLightDir;
uniform sampler2D	uShadowMap;
uniform int			uLightingMode;

in vec3	vertWorldPos;
in vec4	vertLightPos;
in vec3	vertNormal;
in vec2	vertTexUV;

const float	shadowStrength = 0.6;
const float	ambientStrength = 0.2;
const vec3	lightColor = vec3(1.0);

float	shadowMapCalculation()
{
	vec3	projectionCoords = vertLightPos.xyz / vertLightPos.w;

	projectionCoords = projectionCoords / 2 + 0.5;

	float	shadowDepth = texture(uShadowMap, projectionCoords.xy).r;
	float	currentDepth = projectionCoords.z;

	float	bias = 0.0005;

	float	shadow = (currentDepth - bias) > shadowDepth ? shadowStrength : 0.0;
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

	float	dist = length(vertWorldPos);
	float	fogFactor = clamp((dist - uFogNear) / (uFogFar - uFogNear), 0.0, 1.0);
	

	vec3	texColor = texture(uAtlas, vertTexUV).rgb;

	float	diffuseAngle = dot(normalize(vertNormal), uLightDir);
	float	horizonFade = clamp((uLightDir.y + 0.1) / 0.3, 0.0, 1.0);

	vec3	diffuse = max(diffuseAngle, 0.0) * horizonFade * lightColor;
	vec3	ambient = ambientStrength * lightColor;

	float	shadow = shadowMapCalculation();
	if (diffuseAngle < 0.0)
		shadow = shadowStrength;
	shadow *= horizonFade;

	// texColor = (texColor) * (1.0 - shadow) + (vec3(1.0, 0.0, 1.0) * (shadow));
	// texColor = texColor * max(clamp((diffuse + ambient), 0.0, 1.0) * (1.0 - shadow), (1.0 - shadowStrength));

	texColor = texColor * length(vertNormal);
	FragColor = lightCalculation(ambient, shadow, diffuse, texColor);
	FragColor.rgb = mix(FragColor.rgb, uFogColor, fogFactor);
}
