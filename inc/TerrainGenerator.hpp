/*
Created by: Emily (Em_iIy) Winnink
Created on: 13/10/2025
*/

#include "Spline.hpp"
#include "Block.hpp"

class TerrainGenerator {
	public:
		TerrainGenerator();
		~TerrainGenerator();

		int				getTerrainHeight(const mlm::ivec2 &pos);
		int				getTerrainHeight(const mlm::ivec3 &pos);

		

		void			setSeed(int seed);
		int				getSeed() const;

		void			setSeaLevel(int seaLevel);
		int				getSeaLevel() const;

		void			setContinentalnessSpline(const Spline &spline);
		const Spline	&getContinentalnessSpline() const;

	private:
		int				_seed;
		int				_seaLevel;
		Spline			_continentalness;
};