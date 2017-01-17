#include "Sphere.h"
#include <GL/glut.h>

Sphere::Sphere()
{
	rgb = Vec3(.8, .8, .8);
	radius = 1.0;
}

Sphere::Sphere(float _r)
{
	rgb = Vec3(.8, .8, .8);
	radius = _r;
}

void Sphere::draw() const
{
	// const������ �ذ��ϱ� ���� ( center[i]�� reference���� ���� ������ ������ ����)
	Vec3 center = this->center;
	Vec3 rgb = this->rgb;
	GLfloat mat_d[] = { rgb[0], rgb[1], rgb[2], 1.0 };
	glPushMatrix();
	glEnable(GL_NORMALIZE); // glScalef���� normal vector�� scale�Ǵ� ���� ����
	glMatrixMode(GL_MODELVIEW);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_d);
	glTranslatef(center[0], center[1], center[2]);
	glutSolidSphere(radius, 30, 30);
	glPopMatrix();
}
