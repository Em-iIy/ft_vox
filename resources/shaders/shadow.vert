#version 430 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texUV;

uniform mat4 lightProjection;
uniform mat4 model;
uniform mat4 lightView;

void main()
{
	gl_Position = lightProjection * lightView * model * vec4(in_pos, 1.0);
}
