#version 430 core

out vec4 FragColor;

uniform sampler2D	renderTex;
uniform float		uWaterOpacity;

in vec2	vert_texUV;

void main()
{
	vec4	texColor = texture(renderTex, vert_texUV);
	if (texColor.r < 0.01 && texColor.g < 0.01 && texColor.b < 0.01)
		discard ;
	FragColor = vec4(texColor.rgb, uWaterOpacity);
}
