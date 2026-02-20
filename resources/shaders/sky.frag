#version 430 core

uniform vec4	uColors[4];
uniform vec4	uStops;
uniform int		uStopCount;

uniform vec3	uSunDir;

in vec3		viewDir;

out vec4	FragColor;

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
}
