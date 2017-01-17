#include"Boundary.h"

Boundary::Boundary()
{
	Brick();
	width = 20.f;
	height = 5.f;
	depth = 5.f;
	rgb[0] = rgb[1] = rgb[2] = 0.5f;
	strength = -1;
}

Boundary::Boundary(float _w, float _h, float _d) :Brick(_w, _h, _d)
{
	rgb[0] = rgb[1] = rgb[2] = 0.5f;
	strength = -1;
}


Boundary::~Boundary()
{
}