#pragma once
#include "Sphere.h"
#include "Brick.h"

int const STATE_BALL_FIRE = 1 << 1;	// ���� ��� ����
int const STATE_BALL_FAST = 1 << 2;	// ���ӵ� 2��
int const STATE_BALL_SLOW = 1 << 3;	// ���ӵ� 1/2��

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