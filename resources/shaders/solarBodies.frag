#version 430 core

uniform vec3	uSunDir;

in vec3		viewDir;

out vec4	FragColor;

uniform vec4	uSunDiskColor;
uniform float	uSunDiskFactor;
uniform float	uSunDiskSize;
uniform vec4	uSunGlowColor;
uniform float	uSunGlowFactor;
uniform float	uSunGlowSharpness;

uniform vec4	uMoonDiskColor;
uniform float	uMoonDiskFactor;
uniform float	uMoonDiskSize;
uniform vec4	uMoonGlowColor;
uniform float	uMoonGlowFactor;
uniform float	uMoonGlowSharpness;

vec4	sun(vec3 dir, vec3 sunDir)
{
	float	theta = max(dot(dir, sunDir), 0.0);
	float	disk = smoothstep(1.0 - uSunDiskSize, 1.0, theta);
	float	glow = pow(theta, uSunGlowSharpness);

	vec4	color;
	color = uSunDiskColor * uSunDiskFactor * disk;
	color += uSunGlowColor * uSunGlowFactor * glow;
	return (color);
}

vec4	moon(vec3 dir, vec3 moonDir)
{
	float	theta = max(dot(dir, moonDir), 0.0);
	float	disk = smoothstep(1.0 - uMoonDiskSize, 1.0, theta);
	float	glow = pow(theta, uMoonGlowSharpness);

	vec4	color;
	color = uMoonDiskColor * uMoonDiskFactor * disk;
	color += uMoonGlowColor * uMoonGlowFactor * glow;
	return (color);
}

void	main()
{
	vec3	dir = normalize(viewDir);

	FragColor += sun(dir, uSunDir);
	FragColor += moon(dir, -uSunDir);
}
