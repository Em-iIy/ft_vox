#version 430 core

out vec4	FragColor;

uniform sampler2D	uTexture;

in vec2	vertTexUV;

void	main()
{
    FragColor = texture(uTexture, vertTexUV);
}