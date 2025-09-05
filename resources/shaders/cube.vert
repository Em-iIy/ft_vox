#version 430 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texUV;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

out vec3	vert_world_pos;
out vec3	vert_normal;
out	vec2	vert_texUV;

void main()
{
	vert_normal = in_normal;
	vert_texUV = in_texUV;
	vec4 world_pos = model * vec4(in_pos, 1.0);
	vert_world_pos = world_pos.xyz;
	gl_Position = projection * view * world_pos;
}
