#pragma once
#include "Sphere.h"
#include "Brick.h"

int const STATE_BALL_FIRE = 1 << 1;	// 공이 모두 뚫음
int const STATE_BALL_FAST = 1 << 2;	// 공속도 2배
int const STATE_BALL_SLOW = 1 << 3;	// 공속도 1/2배

class Ball : public Sphere
{
public:
	Ball();
	Ball(float _r) : Sphere(_r) {};
	~Ball();

	bool isCollision(Brick* brick, float dt);
	float getCollisionTime(Brick* brick, float dt);
	Vec3 getCollisionNormalVector(Brick* brick, float dt);

	void draw() const; // @Override
	void setType(int const _type); // @Override
	void applyType(int const _type); // @Override
	void removeType(int const _type); // @Override

	void setCollideMode(int colMode);
	int collideMode = 1;
};