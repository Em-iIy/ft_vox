/*
Created by: Emily (Em_iIy) Winnink
Created on: 19/08/2025
*/

#pragma once

#include "glu/gl-utils.hpp"

class ChunkMesh {
	public:
		ChunkMesh();
		~ChunkMesh();
		ChunkMesh(const std::vector<Vertex> &vertices);

		void	draw(Shader &shader);
		
		void	del();
	
	private:
		std::vector<Vertex>		_vertices;
		
		VAO		_vao;
		VBO		_vbo;

		void	setup_mesh();
};