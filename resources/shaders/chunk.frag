#version 430 core

out vec4 FragColor;

uniform sampler2D	atlas;
uniform float		uFogNear;
uniform float		uFogFar;
uniform vec3		uFogColor;
uniform vec3		uLightPos;
uniform vec3		uLightDir;
uniform sampler2D	uShadowMap;

in vec3	vert_world_pos;
in vec4	vert_light_pos;
in vec3	vert_normal;
in vec2	vert_texUV;

const float	shadowStrength = 0.6;

float	shadowMapCalculation()
{
	vec3	projectionCoords = vert_light_pos.xyz / vert_light_pos.w;

	projectionCoords = projectionCoords / 2 + 0.5;

	float	shadowDepth = texture(uShadowMap, projectionCoords.xy).r;
	float	currentDepth = projectionCoords.z;

	float bias = 0.005;

	float shadow = (currentDepth - bias) > shadowDepth ? shadowStrength : 0.0;
	return shadow;
}

void main()
{
	float	dist = length(vert_world_pos);
	float	fogFactor = clamp((dist - uFogNear) / (uFogFar - uFogNear), 0.0, 1.0);
	
	vec4	texColor = texture(atlas, vert_texUV);
	float	shadow = shadowMapCalculation();
	float	horizonFade = clamp((uLightDir.y + 0.1) / 0.2, 0.0, 1.0);
	float	diffuseAngle = dot(normalize(vert_normal), uLightDir);
	float	diffuse = diffuseAngle * horizonFade;
	if (diffuseAngle < 0.0)
		shadow = shadowStrength;
	float	ambient = 0.2;
	shadow *= horizonFade;
	// texColor.rgb = (texColor.rgb) * (1.0 - shadow) + (vec3(1.0, 0.0, 1.0) * (shadow));
	texColor.rgb = texColor.rgb * max((diffuse + ambient) * (1.0 - shadow), shadowStrength);

	texColor.rgb = texColor.rgb * length(vert_normal);

	FragColor = vec4(texColor.rgb, 1.0);
	FragColor.rgb = mix(FragColor.rgb, uFogColor, fogFactor);
}
