#version 430 core

uniform sampler3D	uNoiseTex;
uniform float		uTime;
uniform float		uNightFactor;

in vec3		viewDir;

out vec4	FragColor;

float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float noise(vec3 p){
	vec3 a = floor(p);
	vec3 d = p - a;
	d = d * d * (3.0 - 2.0 * d);

	vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
	vec4 k1 = perm(b.xyxy);
	vec4 k2 = perm(k1.xyxy + b.zzww);

	vec4 c = k2 + a.zzzz;
	vec4 k3 = perm(c);
	vec4 k4 = perm(c + 1.0);

	vec4 o1 = fract(k3 * (1.0 / 41.0));
	vec4 o2 = fract(k4 * (1.0 / 41.0));

	vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
	vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

	return o4.y * d.y + o4.x * (1.0 - d.y);
}

// Settings
const float	sWiggleSpeed = 0.17;
const float	sWiggleScale = 1.0;
const float	sWiggleStrength = 1.4;

const float	sFlowSpeed = 0.005;
const float	sFlowSpeedX = -0.3;
const float	sFlowScale = 0.08;
const float	sFlowStrength = 2.4;

const float	sDensity = 0.07;
const float	sSharpness = 1.8;

const float	sAlpha = 0.7;

const int	sSampleCount = 60;
const float	sSampleOpacity = 0.18;
const float	startHeight = 2.1;
const float	endHeight = 4.8;

const vec3	sTopCol = vec3(0.0, 1.0, 0.2);
const vec3	sBottomCol = vec3(0.0, 1.0, 0.6);

// size value expected to be between 0.0 -> 0.5
float makeStripe(float x, float halfSizeNormalized)
{
	float baseValue = fract(x);
	float left = smoothstep(0.5 - halfSizeNormalized, 0.5, baseValue);
	float right = smoothstep(0.5 + halfSizeNormalized, 0.5, baseValue);
	float stripe = left * right;
	return stripe;
}

vec2	getWiggle(vec2 worldPos, float time)
{
	vec2	wigglePos = worldPos * sWiggleScale;
	float	wiggleOffset = wigglePos.x + wigglePos.y;
	vec2	wiggleNoise = normalize(vec2(
		noise(vec3(wigglePos, wiggleOffset + time)),
		noise(vec3(wigglePos, wiggleOffset + time + 0.5))
	));
	vec2	wiggle = wiggleNoise * sWiggleStrength;
	return (wiggle);
}

vec2	getFlow(vec2 worldPos, float time)
{
	vec2	flowPos = worldPos * sFlowScale;
	vec2	flowNoise = normalize(vec2(
		noise(vec3(flowPos, time)),
		noise(vec3(flowPos, time + 0.5))
	));
	vec2	flow = flowNoise * sFlowStrength;
	return (flow);
}

void	main()
{
	vec3	dir = normalize(viewDir);
	if (dir.y < 0.001)
	{
		FragColor = vec4(0.0);
		return ;
	}

	vec3	color = vec3(0.0);
	float	upFactor = smoothstep(0.05, 0.7, dir.y);
	float	alpha = 0.0;

	float	wTime = uTime * sWiggleSpeed;
	float	fTime = uTime * sFlowSpeed;

	for (int i = 0; i < sSampleCount; i++)
	{
		float	tHeight = float(i) / float(sSampleCount);
		float	currHeight = mix(startHeight, endHeight, tHeight);

		// Get position of current sample
		float	distToHeight = currHeight / upFactor;
		vec2	worldPos = (distToHeight * dir).xz;

		// Offset sample position with noise
		vec2	flow = getFlow(worldPos, fTime);
		vec2	wiggle = getWiggle(worldPos, wTime);
		vec2	warpedPos = worldPos + flow + wiggle + sFlowSpeedX * uTime;

		// Lower intensity right at the bottom and the top half
		float	verticalIntensity = smoothstep(0.0, 0.15, tHeight) * smoothstep(1.0, 0.5, tHeight);

		// Create 2 sets of bands and pick the most significant one
		float 	largeBands = makeStripe(warpedPos.x * sDensity, 0.2);
		float 	smallerBands = makeStripe(warpedPos.x * sDensity * 1.7, 0.1);
		float	band = pow(max(largeBands, smallerBands), sSharpness) * verticalIntensity;

		// Scale sample based on alpha values
		float	sampleAlpha = band * sSampleOpacity;
		float	sampleWeight = sampleAlpha * (1.0 - alpha);

		// Update alpha and color
		vec3	sampleColor = mix(sBottomCol, sTopCol, tHeight);
		color += sampleColor * band * sampleWeight;
		alpha += sampleAlpha * (1.0 - alpha);

		// Break if alpha is almost fully opaque
		if (alpha > 0.95)
			break ;
	}
	FragColor = vec4(color, alpha) * upFactor * sAlpha * smoothstep(0.0, 0.5, uNightFactor);
}
