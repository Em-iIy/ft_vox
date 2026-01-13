#version 430 core

out vec4	FragColor;

uniform sampler2D	uRenderTex;
uniform float		uWaterOpacity;

in vec2	vertTexUV;

void	main()
{
	vec4	texColor = texture(uRenderTex, vertTexUV);
	if (texColor.r < 0.01 && texColor.g < 0.01 && texColor.b < 0.01)
		discard ;
	FragColor = vec4(texColor.rgb, uWaterOpacity);
}
