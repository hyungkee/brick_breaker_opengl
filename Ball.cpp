#include<math.h>
#include "Ball.h"
#include <GL/glut.h>
#define max(x,y) ((x)>(y)?(x):(y))
#define sgn(x) ((x)>0?(-1):(1))
#define speed_rate 1.5

Ball::Ball()
{
	radius = 5.f;
}

Ball::~Ball()
{
}

// @Override
void Ball::draw() const
{
	// const������ �ذ��ϱ� ���� ( center[i]�� reference���� ���� ������ ������ ����)
	Vec3 center = this->center;
	Vec3 rgb = this->rgb;
	if (hasType(STATE_BALL_FIRE))
		rgb = Vec3(1,0,0);
	GLfloat mat_d[] = { rgb[0], rgb[1], rgb[2], 1};
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_d);
	glTranslatef(center[0], center[1], center[2]);
	glutSolidSphere(radius, 30, 30);
	glPopMatrix();
}

// @Override
void Ball::setType(int const _type){
	// ȿ�� ����
	removeType(STATE_BALL_SLOW);
	removeType(STATE_BALL_FAST);
	// ȿ�� �߰�
	if ((_type&STATE_BALL_FAST) && !hasType(STATE_BALL_FAST))
		velocity *= speed_rate;
	if ((_type&STATE_BALL_SLOW) && !hasType(STATE_BALL_SLOW))
		velocity /= speed_rate;

	type = _type;
}

// @Override
void Ball::applyType(int const _type){
	// ȿ�� �߰�
	if ((_type&STATE_BALL_FAST) && hasType(STATE_BALL_SLOW)){
		removeType(STATE_BALL_SLOW);
		return;
	}
	if ((_type&STATE_BALL_SLOW) && hasType(STATE_BALL_FAST)){
		removeType(STATE_BALL_FAST);
		return;
	}
	if ((_type&STATE_BALL_FAST) && !hasType(STATE_BALL_FAST))
		velocity *= speed_rate;
	if ((_type&STATE_BALL_SLOW) && !hasType(STATE_BALL_SLOW))
		velocity /= speed_rate;

	type |= _type;
}

// @Override
void Ball::removeType(int const _type){
	if (hasType(_type)){
		// ȿ�� ����
		if (_type&STATE_BALL_FAST)
			velocity /= speed_rate;
		if (_type&STATE_BALL_SLOW)
			velocity *= speed_rate;
		// Ÿ�� ����
		type -= _type;
	}
}


void Ball::setCollideMode(int colMode){
	collideMode = colMode;
}


float getFaceT(float rvel, float pos, float bcen, float length){
	//�Ǻ��� a(t^2) + 2b(t) = c < 0
	float a = rvel*rvel;
	float b = rvel*(pos - bcen);
	float c = (pos - bcen)*(pos - bcen) - length * length;
	float D = b*b - a*c;

	if (D < 0)	return -1;
	return (-b - sqrt(D)) / a;
}
float getEdgeT(Ball* ball, Brick* brick, Vec3 edge, int d){
	Vec3 bacen = ball->center;
	Vec3 brcen = brick->center;
	Vec3 rvel = ball->velocity - brick->velocity;
	bacen[d] = 0;
	brcen[d] = 0;
	edge[d] = 0;
	rvel[d] = 0;
	brcen += edge;

	//�Ǻ��� a(t^2) + 2b(t) = c < 0
	float a = rvel.size()*rvel.size();
	float b = dot(rvel,bacen-brcen);
	float c = (bacen - brcen).size()*(bacen - brcen).size() - ball->radius * ball->radius;
	float D = b*b - a*c;
	if (D < 0)	return -1;
	if (a == 0)	return -1;
	float t1 = (-b - sqrt(D)) / a;
	float t2 = (-b + sqrt(D)) / a;

	if (t1>=0)
		return t1;
	return t1;
}
bool isInBrickWithoutD(Vec3 pos, Brick* brick, int d){
	bool rx = (pos.x >= brick->center.x - brick->width / 2) & (pos.x <= brick->center.x + brick->width / 2);
	bool ry = (pos.y >= brick->center.y - brick->height / 2) & (pos.y <= brick->center.y + brick->height / 2);
	bool rz = (pos.z >= brick->center.z - brick->depth / 2) & (pos.z <= brick->center.z + brick->depth / 2);
	if (d == 0)	return ry&rz;
	if (d == 1)	return rx&rz;
	if (d == 2)	return rx&ry;
	return true;
}
Vec3 translateEdge(Vec3 edge, int i, int j, int d){
	Vec3 new_edge = edge;
	int counter = d + 1;
	if (counter == 3)	counter = 0;
	new_edge[counter] *= i;
	counter++;
	if (counter == 3)	counter = 0;
	new_edge[counter] *= j;
	counter++;

	return new_edge;
}




// ���� �긯�� �浹�� �긯�� �������� 26���� �������� ������ �����ؾ� �Ѵ�.
// ������(Vertex), �𼭸�(Edge), ��(Face)�� �����Ѵ�.
// �� ������ ��� �������� ������ �ʿ䰡 ����.
float getCollisionTime_SPHERE(Ball* ball, Brick* brick, float dt){
	float ts = dt;

	// ������ �浹
	for (int d = 0; d < 3; d++){

		float length = 0;

		if (d == 0)	length = brick->width / 2;
		if (d == 1)	length = brick->height / 2;
		if (d == 2)	length = brick->depth / 2;

		// ���ӵ�
		float vel = (ball->velocity[d] - brick->velocity[d]);

	
		float tf1 = dt, tf2 = dt;
		if (vel != 0){
			tf1 = getFaceT(vel, ball->center[d] + ball->radius, brick->center[d], length);
			if (!isInBrickWithoutD(ball->center + tf1*(ball->velocity - brick->velocity), brick, d))
				tf1 = dt;
			tf2 = getFaceT(vel, ball->center[d] - ball->radius, brick->center[d], length);
			if (!isInBrickWithoutD(ball->center + tf2*(ball->velocity - brick->velocity), brick, d))
				tf2 = dt;
		}
		if (0 <= tf1 && tf1<dt && ts>tf1){
			ts = tf1;
		}
		if (0 <= tf2 && tf2<dt && ts>tf2)	ts = tf2;
	}


	// �𼭸��� �浹
	for (int d = 0; d < 3; d++){
		float te[4] = { dt };
		int cnt = 0;
		{
			Vec3 edge = Vec3(brick->width / 2, brick->height / 2, brick->depth / 2);
			for (int i = -1; i <= 1; i += 2){
				for (int j = -1; j <= 1; j += 2){
					Vec3 t_edge = translateEdge(edge,i,j,d);
					te[cnt] = getEdgeT(ball, brick, t_edge, d);

					Vec3 expect_pos = ball->center + te[cnt] * (ball->velocity - brick->velocity);
					if (expect_pos[d] > brick->center[d] + edge[d] || expect_pos[d] < brick->center[d] - edge[d])
						te[cnt] = dt;
					if (0<=te[cnt] && te[cnt]<dt && ts>te[cnt])
						ts = te[cnt];
					cnt++;
				}
			}
		}
	}

	return ts;
};

bool isCollision_SPHERE(Ball* ball, Brick* brick, float dt){

	float tcol = getCollisionTime_SPHERE(ball, brick, dt);
	if (0 <= tcol && tcol < dt)
		return true;

	return false;
}

Vec3 getCollisionNormalVector_SPHERE(Ball* ball, Brick* brick, float dt){
	if (brick == NULL)	return Vec3(0, 0, 0);

	Vec3 normal;

	//���� ������ü�� �����Ѵ�.
	float ts = dt;
	float te = dt;
	float tcol = getCollisionTime_SPHERE(ball, brick, dt);

	// ������ �浹
	for (int d = 0; d < 3; d++){

		float length = 0;

		if (d == 0)	length = brick->width / 2;
		if (d == 1)	length = brick->height / 2;
		if (d == 2)	length = brick->depth / 2;

		// ���ӵ�
		float vel = (ball->velocity[d] - brick->velocity[d]);

		float t1 = dt, t2 = dt;
		if (vel != 0){
			t1 = getFaceT(vel, ball->center[d] + ball->radius, brick->center[d], length);
			if (!isInBrickWithoutD(ball->center + t1*(ball->velocity - brick->velocity), brick, d))
				t1 = dt;
			t2 = getFaceT(vel, ball->center[d] - ball->radius, brick->center[d], length);
			if (!isInBrickWithoutD(ball->center + t1*(ball->velocity - brick->velocity), brick, d))
				t2 = dt;

		}

		if (0 <= t1 && t1 < dt && t1 == tcol){
			normal[d] = -1;
			printf("-�� �浹\n");
			return normal;
		}
		if (0 <= t2 && t2 < dt && t2 == tcol){
			normal[d] = 1;
			printf("+�� �浹\n");
			return normal;
		}


		// �𼭸��� �浹
		float te[4] = { dt };
		int cnt = 0;
		{
			Vec3 edge = Vec3(brick->width / 2, brick->height / 2, brick->depth / 2);
			for (int i = -1; i <= 1; i += 2){
				for (int j = -1; j <= 1; j += 2){
					Vec3 t_edge = translateEdge(edge, i, j, d);
					te[cnt] = getEdgeT(ball, brick, t_edge, d);
					Vec3 expect_pos = ball->center + te[cnt] * (ball->velocity - brick->velocity);
					if (expect_pos[d] > brick->center[d] + edge[d] || expect_pos[d] < brick->center[d] - edge[d])
						te[cnt] = dt;

					if (0 <= te[cnt] && te[cnt] < dt && te[cnt] == tcol){
						int counter = d;
						normal[counter++] = 0;
						if (counter == 3)	counter = 0;
						normal[counter++] = i;
						if (counter == 3)	counter = 0;
						normal[counter++] = j;
						// 90���� ���̸� ���Ʒ� �ӵ��� ����� ������ �ȵ� ���� �־� �̰�츦 �����ϱ� ���� ������ ��ư��.
//						normal[0] *= brick->width;
//						normal[1] *= brick->height;
//						normal[2] *= brick->depth;
						printf("�𼭸� �浹\n");
						return normal;
					}
					cnt++;
				}
			}
		}

	}

	return normal;
}



float getCollisionTime_BRICK(Ball* ball, Brick* brick, float dt){

	float ts = -1;
	float te = dt;

	for (int d = 0; d < 3; d++){

		float length = 0;

		if (d == 0)	length = brick->width / 2 + ball->radius;
		if (d == 1)	length = brick->height / 2 + ball->radius;
		if (d == 2)	length = brick->depth / 2 + ball->radius;

		// ���ӵ�
		float vel = (ball->velocity[d] - brick->velocity[d]);

		float t1 = -1, t2 = dt;
		if (vel != 0){
			//�Ǻ��� a(t^2) + 2b(t) = c < 0
			float a = vel*vel;
			float b = (float)vel*(ball->center[d] - brick->center[d]);
			float c = (ball->center[d] - brick->center[d])*(ball->center[d] - brick->center[d]) - length * length;

			t1 = (-b - sqrt(b*b - a*c)) / a;
			t2 = (-b + sqrt(b*b - a*c)) / a;
		}
		if (vel == 0){
			if (abs(ball->center[d] - brick->center[d]) > abs(length))
				t1 = dt; // ���ɼ� ����
		}

		if (ts < t1)	ts = t1;
		if (te > t2)	te = t2;
	}

	if (ts > te)	return -1;
	return ts;
};

bool isCollision_BRICK(Ball* ball, Brick* brick, float dt){

	float tcol = getCollisionTime_BRICK(ball, brick, dt);
	if (0 <= tcol && tcol < dt)
		return true;

	return false;
}

Vec3 getCollisionNormalVector_BRICK(Ball* ball, Brick* brick, float dt){
	if (brick == NULL)	return Vec3(0, 0, 0);

	Vec3 normal;

	//���� ������ü�� �����Ѵ�.
	float ts = 0;
	float te = dt;
	float tcol = getCollisionTime_BRICK(ball, brick, dt);

	for (int d = 0; d < 3; d++){

		float length = 0;

		if (d == 0)	length = brick->width / 2 + ball->radius;
		if (d == 1)	length = brick->height / 2 + ball->radius;
		if (d == 2)	length = brick->depth / 2 + ball->radius;

		float vel = (ball->velocity[d] - brick->velocity[d]);

		float t1 = -1, t2 = dt;
		if (vel != 0){
			// ���ӵ�
			float a = vel*vel;
			float b = (float)vel*(ball->center[d] - brick->center[d]);
			float c = (ball->center[d] - brick->center[d])*(ball->center[d] - brick->center[d]) - length * length;

			t1 = (-b - sqrt(b*b - a*c)) / a;
			t2 = (-b + sqrt(b*b - a*c)) / a;
		}
		if (vel == 0){
			if (abs(ball->center[d] - brick->center[d]) > abs(length))
				t1 = dt;
		}

		if (t1 == tcol)
			normal[d] = sgn(vel);
	}
	return normal;
}



bool Ball::isCollision(Brick* brick, float dt){
	if (collideMode == 1)
		return isCollision_BRICK(this, brick, dt);
	if (collideMode == 2)
		return isCollision_SPHERE(this, brick, dt);
}
float Ball::getCollisionTime(Brick* brick, float dt){
	if (collideMode == 1)
		return getCollisionTime_BRICK(this, brick, dt);
	if (collideMode == 2)
		return getCollisionTime_SPHERE(this, brick, dt);
}
Vec3 Ball::getCollisionNormalVector(Brick* brick, float dt){
	if (collideMode == 1)
		return getCollisionNormalVector_BRICK(this, brick, dt);
	if (collideMode == 2)
		return getCollisionNormalVector_SPHERE(this, brick, dt);
}
