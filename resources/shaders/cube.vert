#version 430 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_norm;
layout (location = 2) in vec3 in_texUV;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

out vec3	vert_color;

void main()
{
	vert_color = in_norm;
	gl_Position = projection * view * model * vec4(in_pos, 1.0);
}
