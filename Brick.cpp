#include"Brick.h"
#include <math.h>
#include <GL/glut.h>

#define sgn(x) ((x)>0?(-1):(1))

Brick::Brick()
{
	Cube();
	width = 20.f;
	height = 5.f;
	depth = 5.f;
	strength = 1;
}

Brick::Brick(float _w, float _h, float _d) :Cube(_w, _h, _d)
{
	strength = 1;
}


void Brick::setItem(Item* new_item){
	item = new_item;
}

void Brick::draw() const
{
	Vec3 rgb = this->rgb;
	GLfloat mat_d[] = { rgb[0], rgb[1], rgb[2], 1.0 };
	glMatrixMode(GL_MODELVIEW);

	//division(2^damage) 등분하여 출력
	int division = pow(2, damage);

	float width = this->width / division - 1;
	float height = this->height / division - 1;
	float depth = this->depth / division - 1;

	for (int i = 0; i < division; i++){
		for (int j = 0; j < division; j++){
			for (int k = 0; k < division; k++){
				// 각각을 출력
				Vec3 center = this->center;
				center.x += this->width * ((float)(i + 0.5) / division - 0.5);
				center.y += this->height * ((float)(j + 0.5) / division - 0.5);
				center.z += this->depth * ((float)(k + 0.5) / division - 0.5);

				glPushMatrix();
				glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_d);
				glTranslatef(center[0], center[1], center[2]);
				glScalef(width, height, depth);
				glutSolidCube(1);
				glPopMatrix();
			}
		}
	}

}

void Brick::hit(){
	damage++;
}

int Brick::getLife() const{
	return strength - damage;
}

void Brick::setStrength(int _strength){
	strength = _strength;
}


Brick::~Brick()
{
}


float Brick::getCollisionTime(Brick* brick, float dt){

	float ts = -1;
	float te = dt;

	for (int d = 0; d < 3; d++){

		float length = 0;

		if (d == 0)	length = brick->width / 2 + width / 2;
		if (d == 1)	length = brick->height / 2 + height / 2;
		if (d == 2)	length = brick->depth / 2 + depth / 2;

		// 상대속도
		float vel = (velocity[d] - brick->velocity[d]);

		float t1 = -1, t2 = dt;
		if (vel != 0){
			//판별식 a(t^2) + 2b(t) = c < 0
			float a = vel*vel;
			float b = (float)vel*(this->center[d] - brick->center[d]);
			float c = (this->center[d] - brick->center[d])*(this->center[d] - brick->center[d]) - length * length;

			t1 = (-b - sqrt(b*b - a*c)) / a;
			t2 = (-b + sqrt(b*b - a*c)) / a;
		}
		if (vel == 0){
			if (abs(this->center[d] - brick->center[d]) > abs(length))
				t1 = dt; // 가능성 제거
		}

		if (ts < t1)	ts = t1;
		if (te > t2)	te = t2;
	}

	if (ts > te)	return -1;
	return ts;
};

bool Brick::isCollision(Brick* brick, float dt){

	float tcol = getCollisionTime(brick, dt);
	if (0 <= tcol && tcol < dt)
		return true;

	return false;
}

Vec3 Brick::getCollisionNormalVector(Brick* brick, float dt){
	if (brick == NULL)	return Vec3(0, 0, 0);

	Vec3 normal;

	//공을 직육면체로 생각한다.
	float ts = 0;
	float te = dt;
	float tcol = getCollisionTime(brick, dt);

	for (int d = 0; d < 3; d++){

		float length = 0;

		if (d == 0)	length = brick->width / 2 + width / 2;
		if (d == 1)	length = brick->height / 2 + height / 2;
		if (d == 2)	length = brick->depth / 2 + depth / 2;

		float vel = (velocity[d] - brick->velocity[d]);

		float t1 = -1, t2 = dt;
		if (vel != 0){
			// 상대속도
			float a = vel*vel;
			float b = (float)vel*(this->center[d] - brick->center[d]);
			float c = (this->center[d] - brick->center[d])*(this->center[d] - brick->center[d]) - length * length;

			t1 = (-b - sqrt(b*b - a*c)) / a;
			t2 = (-b + sqrt(b*b - a*c)) / a;
		}
		if (vel == 0){
			if (abs(this->center[d] - brick->center[d]) > abs(length))
				t1 = dt;
		}

		if (t1 == tcol)
			normal[d] = sgn(vel);
	}
	return normal;
}