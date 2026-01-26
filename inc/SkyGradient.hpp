/*
Created by: Emily (Em_iIy) Winnink
Created on: 26/01/2026
*/

#include "glu/gl-utils.hpp"

struct GradientDTO {
	mlm::vec4	_sunriseCol;
	mlm::vec4	_dayLowCol;
	mlm::vec4	_dayHighCol;
	mlm::vec4	_sunsetCol;
	mlm::vec4	_nightLowCol;
	mlm::vec4	_nightHighCol;
};

class SkyGradient
{
public:
	SkyGradient();
	~SkyGradient();

	void		init(const GradientDTO dto);

	mlm::vec4	sampleAt(const float t) const;

private:
	std::vector<std::pair<float, mlm::vec4>>	_stops;
};
