#version 430 core

out vec4	FragColor;

uniform sampler2D	uGNormal;
uniform sampler2D	uGPosition;
uniform sampler2D	uNoiseTex;

uniform vec3		uSamples[64];
uniform int			uSampleCount;

uniform float		uRadius;

uniform float		uScreenWidth;
uniform float		uScreenHeight;

uniform mat4		uProjection;

in vec2	vertTexUV;

const float			bias = 0.025;

void	main()
{
	vec3	normal = texture(uGNormal, vertTexUV).xyz;
	vec3	fragPos = texture(uGPosition, vertTexUV).xyz;

	vec2	noiseScale = vec2(uScreenWidth / 4.0f, uScreenHeight / 4.0f);
	vec3	randomVec = texture(uNoiseTex, vertTexUV * noiseScale).xyz;

	vec3	tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3	bitangent = cross(normal, tangent);
	mat3	TBN = mat3(tangent, bitangent, normal);

	float	occlusion = 0.0f;
	for (int i = 0; i < uSampleCount; ++i)
	{
		vec3	samplePos = TBN * uSamples[i];
		samplePos = fragPos + samplePos * uRadius;

		vec4	offset = vec4(samplePos, 1.0);
		offset = uProjection * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;

		float	sampleDepth = texture(uGPosition, offset.xy).z;

		float	rangeCheck = smoothstep(0.0, 1.0, uRadius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - (occlusion / uSampleCount);

	FragColor = vec4(vec3(occlusion), 1.0);
}
