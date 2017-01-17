#pragma once
#include "Brick.h"

class Stick : public Brick
{
public:
	Stick();
	Stick(float _w, float _h, float _d);
	~Stick();

	void draw(int life) const;
};

