#version 430 core

uniform vec3	uSunDir;

in vec3		viewDir;

out vec4	FragColor;

uniform vec4	uSunDiskColor;
uniform vec4	uSunDiskFactor;
uniform vec4	uSunDiskSize;
uniform vec4	uSunGlowColor;
uniform vec4	uSunGlowFactor;
uniform vec4	uSunGlowSharpness;

uniform vec4	uMoonDiskColor;
uniform vec4	uMoonDiskFactor;
uniform vec4	uMoonDiskSize;
uniform vec4	uMoonGlowColor;
uniform vec4	uMoonGlowFactor;
uniform vec4	uMoonGlowSharpness;

vec4	sun(vec3 dir, vec3 sunDir)
{
	const vec4	sunCol = vec4(1.0, 1.0, 0.0, 1.0);
	const vec4	glowCol = vec4(1.0, 0.7, 0.3, 1.0);

	float	theta = max(dot(dir, sunDir), 0.0);
	float	disk = smoothstep(0.998, 1.0, theta);
	float	glow = pow(theta, 64);

	vec4	color;
	color = sunCol * (disk * 0.8);
	color += glowCol * (glow * 0.2);
	return (color);
}

vec4	moon(vec3 dir, vec3 sunDir)
{
	const vec4	moonCol = vec4(0.8, 0.9, 1.0, 1.0);
	const vec4	glowCol = vec4(0.2, 0.2, 0.9, 1.0);

	float	theta = max(dot(dir, sunDir), 0.0);
	float	disk = smoothstep(0.9995, 1.0, theta);
	float	glow = pow(theta, 128);

	vec4	color;
	color = moonCol * (disk * 1.0);
	color += glowCol * (glow * 0.2);
	return (color);
}

void	main()
{
	vec3	dir = normalize(viewDir);

	FragColor += sun(dir, uSunDir);
	FragColor += moon(dir, -uSunDir);
}
