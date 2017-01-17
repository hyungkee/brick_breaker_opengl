#pragma once
#include "Brick.h"

// ������ ����
int const ITEM_ERASE = 1;	// ��� ȿ���� ������

int const ITEM_BALL_FIRE = 1 << 1;	// ���� ��� ����v
int const ITEM_BALL_FAST = 1 << 2;	// ���ӵ� rate��v
int const ITEM_BALL_SLOW = 1 << 3;	// ���ӵ� 1/rate��v
int const ITEM_BALL_MULTIPLE = 1 << 4;	// ���� 3���� ��v

/*
int const ITEM_STICK_STICKY = 1 << 8;	// ������ ���밡 ��
int const ITEM_STICK_REVERSE = 1 << 9;	// ����Ű�� ������ �ݴ�� ����
int const ITEM_STICK_LONG = 1 << 10;	// ������ ���� 2��
int const ITEM_STICK_SHORT = 1 << 11;	// ������ ���� 1/2��
int const ITEM_STICK_GUN = 1 << 12;	// ���밡 ���� �� �� ����
*/

int const ITEM_ENV_FOG = 1 << 16;	// �Ȱ�v
int const ITEM_ENV_BOOST = 1 << 17;	// 5�ʰ� �������·� ���� �ſ� ������

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