#pragma once
#include "Shape.h"

class Sphere:public Shape{
public:
	Sphere();
	Sphere(float _r);
	void draw() const;
	float radius;
};