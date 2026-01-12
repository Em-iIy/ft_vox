#version 430 core

out vec4	FragColor;

uniform sampler2D	uTexture;

in vec2	vertTexUV;

void	main()
{
	vec2	texelSize = 1.0 / vec2(textureSize(uTexture, 0));
	float	result = 0.0;
	for (int x = -2; x < 2; ++x)
	{
		for (int y = -2; y < 2; ++y)
		{
			vec2	offset = vec2(float(x), float(y)) * texelSize;
			result += texture(uTexture, vertTexUV + offset).r;
		}
	}
	FragColor = vec4(vec3(result / (4.0 * 4.0)), 1.0);
}
