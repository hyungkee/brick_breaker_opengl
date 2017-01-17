#pragma once
#include "Shape.h"


//implement here
class Cube :public Shape{
public:
	Cube();
	Cube(float _w, float _h, float _d);
	void draw() const;
	float width;
	float height;
	float depth;
};