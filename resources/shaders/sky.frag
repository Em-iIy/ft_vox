#version 430 core

uniform vec4	uColors[4];
uniform vec4	uStops;
uniform int		uStopCount;

uniform vec3	uSunDir;

in vec3		viewDir;

out vec4	FragColor;

// void	main()
// {
// 	vec3	color = vec3(0.0);
// 	float	height = 2.0;
// 	int		sampleCount = 20;
// 	vec3	dir = normalize(viewDir);
// 	float	upFactor = viewDir.y;

// 	for (int i = 0; i < sampleCount; i++)
// 	{
// 		height += 0.01;
// 		float	distToHeight = height / upFactor;
// 		vec3	heightPos = distToHeight * viewDir;
// 		color += vec3(smoothstep(0.15, 0.0, abs(fract(heightPos.x) - 0.5))) * 0.2;
// 	}
// 	FragColor = vec4(color * upFactor, upFactor);
// }

vec3	sun(vec3 dir, vec3 sunDir)
{
	float	f = pow(max(dot(dir, sunDir), 0.0), 128);
	vec3	color = vec3(1.0, 1.0, 0.0) * smoothstep(0.65, 0.9, f);
	return (color);
}

vec3	moon(vec3 dir, vec3 moonDir)
{
	float	f = pow(max(dot(dir, moonDir), 0.0), 256);
	vec3	color = vec3(0.8, 0.9, 1.0) * smoothstep(0.9, 1.0, f);
	return (color);
}

void	main()
{
	vec3	dir = normalize(viewDir);
	float	t = clamp(viewDir.y * 0.5 + 0.5, 0.0, 1.0);

	vec4	color = uColors[0];
	if (t <= uStops[0])
		color = uColors[0];
	else if (t >= uStops[uStopCount - 1])
		color = uColors[uStopCount - 1];
	else
	{
		for (int i = 1; i < uStopCount; i++)
		{
			float	a = uStops[i - 1];
			float	b = uStops[i];
			if (t >= a && t <= b)
			{
				float	f = (t - a) / (b - a);
				color = mix(uColors[i - 1], uColors[i], f);
				break ;
			}
		}
	}
	FragColor = color;
	FragColor.xyz += sun(dir, uSunDir);
	FragColor.xyz += moon(dir, -uSunDir);
}
