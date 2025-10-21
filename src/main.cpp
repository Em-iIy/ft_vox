/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/07/2025
*/

#include "VoxEngine.hpp"
#include <iostream>

int main()
{
	VoxEngine engine;

	TerrainGeneratorDTO terrainDto = {
		.seed = 4242,
		.seaLevel = 110,
		.caveDiameter = 0.2f,
		.continentalness = {
			.spline = Spline({
				mlm::vec2(-1.0f, 250.0f),
				mlm::vec2(-0.8f, 95.0f),
				mlm::vec2(-0.2f, 95.0f),
				mlm::vec2(0.0f, 108.0f),
				mlm::vec2(0.25f, 120.0f),
				mlm::vec2(0.45f, 180.0f),
				mlm::vec2(1.00f, 200.0f)
			}),
			.depth = 5,
			.step = 2.0,
			.zoom = 400,
			.dimensions = 2,
		},
		.cave = {
			.spline = Spline({
				mlm::vec2(-1.0f, -1.0f),
				mlm::vec2(1.0f, 1.0f),
			}),
			.depth = 1,
			.step = 1.0,
			.zoom = 160,
			.dimensions = 3,
		},
	};

	TerrainGenerator gen(terrainDto);

	engine.run();
	return (0);
}