#version 430 core

out vec4	FragColor;

uniform sampler2D	uTexture;

in vec2	vertTexUV;

void	main()
{
	float	depth = texture(uTexture, vertTexUV).r;

    FragColor = vec4(vec3(depth), 1.0);
}