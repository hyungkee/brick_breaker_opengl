#pragma once
#include <iostream>
#include "vec3.h"

class Shape {
public:
	Vec3 rgb;
	Vec3 center;
	Vec3 velocity;
	int type = 0;

	Shape(){ type = 0; };

	void move(Vec3 vec);
	void setPos(Vec3 vec);
	void setVel(Vec3 vec);
	virtual void draw() const = 0;
	void setColor(Vec3 color);

	void setType(int const _type);
	void applyType(int const _type);
	void removeType(int const _type);
	bool hasType(int const _type) const;

};