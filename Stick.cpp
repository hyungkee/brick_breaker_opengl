#include "Stick.h"
#include <GL/glut.h>

Stick::Stick()
{
	width = 20.f;
	height = 5.f;
	depth = 5.f;
	rgb = Vec3(.5, .5, 1);
	strength = -2;
}

Stick::Stick(float _w, float _h, float _d) :Brick(_w, _h, _d)
{
	strength = -2;
	rgb = Vec3(.5, .5, 1);
}



Stick::~Stick()
{
}

void Stick::draw(int life) const
{
	// const문제를 해결하기 위함 ( center[i]가 reference여서 직접 넣으면 문제가 생김)
	Vec3 center = this->center;
	Vec3 rgb = this->rgb;
	GLfloat mat_d[] = { rgb[0], rgb[1], rgb[2], 1.0 };
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_d);
	glTranslatef(center[0], center[1], center[2]);
	glScalef(width, height, depth);
	glutSolidCube(1);
	glPopMatrix();

	// 녹색 LIFE 표시기
	mat_d[0] = .5;
	mat_d[1] = 1;
	mat_d[2] = .5;

	if (life >= 1){
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_d);
		glTranslatef(center[0] - width / 3, center[1], center[2] + 2);
		glScalef(width / 3 - 3, height - 3, depth);
		glutSolidCube(1);
		glPopMatrix();
	}
	if (life >= 2){
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_d);
		glTranslatef(center[0], center[1], center[2] + 2);
		glScalef(width / 3 - 3, height - 3, depth);
		glutSolidCube(1);
		glPopMatrix();
	}
	if (life >= 3){
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_d);
		glTranslatef(center[0] + width / 3, center[1], center[2] + 2);
		glScalef(width / 3 - 3, height - 3, depth);
		glutSolidCube(1);
		glPopMatrix();
	}
}
