/*
Created by: Emily (Em_iIy) Winnink
Created on: 19/08/2025
*/

#pragma once

#include "glu/gl-utils.hpp"

class ChunkMesh {
		public:
		std::vector<Vertex>		_vertices;
		std::vector<uint32_t>	_indices;
		ChunkMesh();
		~ChunkMesh();
		ChunkMesh(const std::vector<Vertex> &vertices);

		void	draw(Shader &shader);
	
	private:
		VAO		_vao;
		VBO		_vbo;

		void	setup_mesh();
};