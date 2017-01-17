#pragma once
#include "Cube.h"

class Item;//Item.h와 Brick.h의 상호 참조를 막기 위함

class Brick : public Cube
{
public:
	Brick();
	Brick(float _w, float _h, float _d);
	~Brick();

	void setItem(Item* new_item);
	void draw() const;
	int getLife() const;
	void hit();
	void setStrength(int _strength);

	bool isCollision(Brick* brick, float dt);
	float getCollisionTime(Brick* brick, float dt);
	Vec3 getCollisionNormalVector(Brick* brick, float dt);

	Item* item = NULL;

protected:
	int strength = 0;
	int damage = 0;
};