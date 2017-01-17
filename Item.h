#pragma once
#include "Brick.h"

// 아이템 종류
int const ITEM_ERASE = 1;	// 모든 효과를 무시함

int const ITEM_BALL_FIRE = 1 << 1;	// 공이 모두 뚫음v
int const ITEM_BALL_FAST = 1 << 2;	// 공속도 rate배v
int const ITEM_BALL_SLOW = 1 << 3;	// 공속도 1/rate배v
int const ITEM_BALL_MULTIPLE = 1 << 4;	// 공이 3개가 됨v

/*
int const ITEM_STICK_STICKY = 1 << 8;	// 끈끈이 막대가 됨
int const ITEM_STICK_REVERSE = 1 << 9;	// 방향키의 영향을 반대로 받음
int const ITEM_STICK_LONG = 1 << 10;	// 막대의 길이 2배
int const ITEM_STICK_SHORT = 1 << 11;	// 막대의 길이 1/2배
int const ITEM_STICK_GUN = 1 << 12;	// 막대가 총을 쏠 수 있음
*/

int const ITEM_ENV_FOG = 1 << 16;	// 안개v
int const ITEM_ENV_BOOST = 1 << 17;	// 5초간 무적상태로 공이 매우 빨라짐

class Item : public Brick
{
public:
	Item();
	Item(int _type);
	Item(float _w, float _h, float _d, int _type);
	~Item();

	void setType(int const _type); // Override
	void setTypeRandom();

	char* getItemName();

private:

};