#include "Cube.h"
#include <GL/glut.h>

//implement here
Cube::Cube()
{
	rgb = Vec3(0, 1, 0);
	width = 20.0;
	height = 5.0;
	depth = 1.0;
}

Cube::Cube(float _w, float _h, float _d)
{
	rgb = Vec3(0, 1, 0);
	width = _w;
	height = _h;
	depth = _d;
}

void Cube::draw() const
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
}
