#version 430 core

out vec4	FragColor;

uniform sampler2D	uTexture;

in vec2	vertTexUv;

void	main()
{
	float	depth = texture(uTexture, vertTexUv).r;

    FragColor = vec4(vec3(depth), 1.0);
}