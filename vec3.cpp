#include <math.h>
#include "vec3.h"

std::ostream& operator<<(std::ostream& os, const Vec3& a) { os << a.x << " " << a.y << " " << a.z; return os; }

//implement here - define operator overloading functions

Vec3 operator*(const Vec3& a, const float scale){
	return Vec3(a.x*scale, a.y*scale, a.z*scale);
}

Vec3 operator/(const Vec3& a, const float scale){
	return Vec3(a.x/scale, a.y/scale, a.z/scale);
}

Vec3 operator*(const float scale, const Vec3& a){
	return Vec3(a.x*scale, a.y*scale, a.z*scale);
}

Vec3 operator+(const Vec3& a, const Vec3& b){
	return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3 operator-(const Vec3& a, const Vec3& b){
	return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3& operator*=(Vec3& a, const float scale){
	a.x *= scale;
	a.y *= scale;
	a.z *= scale;
	return a;
}

Vec3& operator/=(Vec3& a, const float scale){
	a.x /= scale;
	a.y /= scale;
	a.z /= scale;
	return a;
}

Vec3& operator+=(Vec3& a, const Vec3& b){
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

Vec3& operator-=(Vec3& a, const Vec3& b){
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

bool operator==(const Vec3& a, const Vec3& b){
	return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}

bool operator!=(const Vec3& a, const Vec3& b){
	return !(a==b);
}

bool operator<(const Vec3& a, const Vec3& b){
	return (a.x*a.x + a.y*a.y + a.z*a.z) < (b.x*b.x + b.y*b.y + b.z*b.z);
}

bool operator>(const Vec3& a, const Vec3& b){
	return (a.x*a.x + a.y*a.y + a.z*a.z) > (b.x*b.x + b.y*b.y + b.z*b.z);
}

bool operator<=(const Vec3& a, const Vec3& b){
	return !(a > b);
}

bool operator>=(const Vec3& a, const Vec3& b){
	return !(a < b);
}

Vec3& operator++(Vec3&a){
	++a.x;
	++a.y;
	++a.z;
	return a;
}

Vec3 operator++(Vec3&a, int){
	Vec3 vec = Vec3(a);
	++a;
	return vec;
}

float dot(const Vec3& a, const Vec3& b){
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vec3 Vec3::projTo(const Vec3&a){
	if (a.x == 0 && a.y == 0 && a.z == 0)	return Vec3(0, 0, 0);
	Vec3 ans = (dot((*this), a) / (a.x*a.x + a.y*a.y + a.z*a.z)) * a;
	return ans;
}


Vec3 Vec3::operator-(){
	return Vec3(-x,-y,-z);
}


float& Vec3::operator[](int index){
	if (index == 0)	return x;
	if (index == 1)	return y;
	return z;
}

void Vec3::normalize(){
	float size = sqrt(x*x+y*y+z*z);
	if (size == 0)	return;
	(*this) /= size;
}

float Vec3::size(){
	return sqrt(x*x+y*y+z*z);
}