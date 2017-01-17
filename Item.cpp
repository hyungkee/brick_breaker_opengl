#include"Item.h"
#include"Brick.h"
#include<string>

using namespace std;

Item::Item()
{
	Cube();
	width = 20.f;
	height = 20.f;
	depth = 20.f;
	rgb = Vec3(0, 1, 1);
	type = 0;
	velocity = Vec3(0, -100, 0);
}

Item::Item(int _type)
{
	width = 20.f;
	height = 20.f;
	depth = 20.f;
	rgb = Vec3(0.3, 0.8, 0.6);
	setType(_type);
	velocity = Vec3(0, -100, 0);
}

Item::Item(float _w, float _h, float _d, int _type) :Brick(_w, _h, _d)
{
	rgb = Vec3(0.3, 0.8, 0.6);
	setType(_type);
	velocity = Vec3(0, -100, 0);
}

Item::~Item()
{
}

void Item::setType(int const _type){
	type = _type;
	if (type == ITEM_ERASE)
		rgb = Vec3(1, 1, 1);
	if (type == ITEM_BALL_FIRE)
		rgb = Vec3(1, 0, 0);
	if (type == ITEM_BALL_MULTIPLE)
		rgb = Vec3(1, 1, 0);
	if (type == ITEM_BALL_FAST)
		rgb = Vec3(1, 0, 1);
	if (type == ITEM_BALL_SLOW)
		rgb = Vec3(1, .5, .5);
	if (type == ITEM_ENV_BOOST)
		rgb = Vec3(0, 0, 0);
	if (type == ITEM_ENV_FOG)
		rgb = Vec3(.3, .8, .6);
}

void Item::setTypeRandom(){
	int x = rand() % 7;
	switch (x)
	{
	case 0:
		setType(ITEM_ERASE);
		break;
	case 1:
		setType(ITEM_BALL_FAST);
		break;
	case 2:
		setType(ITEM_BALL_FIRE);
		break;
	case 3:
		setType(ITEM_BALL_MULTIPLE);
		break;
	case 4:
		setType(ITEM_BALL_SLOW);
		break;
	case 5:
		setType(ITEM_ENV_BOOST);
		break;
	case 6:
		setType(ITEM_ENV_FOG);
		break;
	}
}


char* Item::getItemName(){
	char name[100] = "";
	if (hasType(ITEM_ERASE))			strcat_s(name, "ERASE,");
	if (hasType(ITEM_BALL_FAST))		strcat_s(name, "FAST,");
	if (hasType(ITEM_BALL_SLOW))		strcat_s(name, "SLOW,");
	if (hasType(ITEM_BALL_MULTIPLE))	strcat_s(name, "MULTI,");
	if (hasType(ITEM_BALL_FIRE))		strcat_s(name, "FIRE,");
	if (hasType(ITEM_ENV_FOG))			strcat_s(name, "FOG,");
	if (hasType(ITEM_ENV_BOOST))		strcat_s(name, "BOOST,");

	if(strlen(name)>0)	name[strlen(name)-1] = NULL; // 마지막 쉼표 제거

	char* result = new char[strlen(name)+1];
	for (int i = 0; i < strlen(name); i++)
		result[i] = name[i];
	result[strlen(name)] = NULL;

	return result;
}