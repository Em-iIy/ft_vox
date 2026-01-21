#version 430 core

uniform float	uTime;

in vec3		viewDir;

out vec4	FragColor;

void	main()
{
	vec3	color = vec3(0.0);
	float	height = 2.0;
	int		sampleCount = 20;
	vec3	dir = normalize(viewDir);
	float	upFactor = viewDir.y;

	for (int i = 0; i < sampleCount; i++)
	{
		height += 0.01;
		float	distToHeight = height / upFactor;
		vec3	heightPos = distToHeight * viewDir;
		color += vec3(smoothstep(0.15, 0.0, abs(fract(heightPos.x) - 0.5))) * 0.2;
	}
	FragColor = vec4(color * upFactor, upFactor);
}
