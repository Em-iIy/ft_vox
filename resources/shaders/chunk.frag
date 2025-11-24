#version 430 core

out vec4	FragColor;

uniform sampler2D	uAtlas;
uniform float		uFogNear;
uniform float		uFogFar;
uniform vec3		uFogColor;
uniform vec3		uLightPos;
uniform vec3		uLightDir;
uniform sampler2D	uShadowMap;

in vec3	vertWorldPos;
in vec4	vertLightPos;
in vec3	vertNormal;
in vec2	vertTexUV;

const float	shadowStrength = 0.6;

float	shadowMapCalculation()
{
	vec3	projectionCoords = vertLightPos.xyz / vertLightPos.w;

	projectionCoords = projectionCoords / 2 + 0.5;

	float	shadowDepth = texture(uShadowMap, projectionCoords.xy).r;
	float	currentDepth = projectionCoords.z;

	float	bias = 0.005;

	float	shadow = (currentDepth - bias) > shadowDepth ? shadowStrength : 0.0;
	return (shadow);
}

void	main()
{
	float	dist = length(vertWorldPos);
	float	fogFactor = clamp((dist - uFogNear) / (uFogFar - uFogNear), 0.0, 1.0);
	
	vec4	texColor = texture(uAtlas, vertTexUV);
	float	shadow = shadowMapCalculation();
	float	horizonFade = clamp((uLightDir.y + 0.1) / 0.2, 0.0, 1.0);
	float	diffuseAngle = dot(normalize(vertNormal), uLightDir);
	float	diffuse = diffuseAngle * horizonFade;
	if (diffuseAngle < 0.0)
		shadow = shadowStrength;
	float	ambient = 0.2;
	shadow *= horizonFade;
	// texColor.rgb = (texColor.rgb) * (1.0 - shadow) + (vec3(1.0, 0.0, 1.0) * (shadow));
	texColor.rgb = texColor.rgb * max((diffuse + ambient) * (1.0 - shadow), shadowStrength);

	texColor.rgb = texColor.rgb * length(vertNormal);

	FragColor = vec4(texColor.rgb, 1.0);
	FragColor.rgb = mix(FragColor.rgb, uFogColor, fogFactor);
}
