/*
Created by: Emily (Em_iIy) Winnink
Created on: 19/08/2025
*/

#include "ChunkMesh.hpp"

ChunkMesh::ChunkMesh() {}

ChunkMesh::~ChunkMesh()
{
}

ChunkMesh::ChunkMesh(const std::vector<Vertex> &vertices): _vertices(vertices)
{
	setup_mesh();
}

void	ChunkMesh::draw(Shader &shader)
{
	(void)shader;
	_vao.bind();
	glDrawArrays(GL_TRIANGLES, 0, _vertices.size());
	_vao.unbind();
}


void	ChunkMesh::setup_mesh()
{
	_vao.init();
	_vao.bind();

	// Setup Vertex buffer
	_vbo = VBO(reinterpret_cast<GLfloat *>(_vertices.data()), static_cast<GLsizeiptr>(_vertices.size() * sizeof(Vertex)));
	_vao.link_attr(_vbo, 0, sizeof(Vertex::pos) / sizeof(GLfloat), GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, pos));
	_vao.link_attr(_vbo, 1, sizeof(Vertex::normal) / sizeof(GLfloat), GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, normal));
	_vao.link_attr(_vbo, 2, sizeof(Vertex::texUV) / sizeof(GLfloat), GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, texUV));

	_vao.unbind();
	_vbo.unbind();
}

void	ChunkMesh::del()
{
	_vao.del();	
	_vbo.del();	
}