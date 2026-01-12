#version 430 core

out vec4	FragColor;

uniform sampler2D	uTexture;

in vec2	vertTexUV;

void	main()
{
    FragColor = vec4(texture(uTexture, vertTexUV).rgb, 1.0);
}