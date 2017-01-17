#pragma once
#include <iostream>
#include <math.h>

class Vec3 {
public : 
	Vec3() : x(0), y(0), z(0) {}
	Vec3(float a, float b, float c) : x(a), y(b), z(c) {}

	//implement here - declare operator overloading functions 
	Vec3 operator-();
	float& operator[](int index);
	Vec3 projTo(const Vec3&a);
	void normalize();
	float size();

	float x,y,z;
};

//implement here - declare operator overloading functions 
std::ostream& operator<<(std::ostream& os, const Vec3& a);
Vec3 operator*(const Vec3& a, float scale);
Vec3 operator/(const Vec3& a, float scale);
Vec3 operator*(float scale, const Vec3& a);
Vec3 operator+(const Vec3& a, const Vec3& b);
Vec3 operator-(const Vec3& a, const Vec3& b);

Vec3& operator*=(Vec3& a, const float scale);
Vec3& operator/=(Vec3& a, const float scale);
Vec3& operator+=(Vec3& a, const Vec3& b);
Vec3& operator-=(Vec3& a, const Vec3& b);

bool operator==(const Vec3& a, const Vec3& b);
bool operator!=(const Vec3& a, const Vec3& b);
bool operator<(const Vec3& a, const Vec3& b);
bool operator>(const Vec3& a, const Vec3& b);
bool operator<=(const Vec3& a, const Vec3& b);
bool operator>=(const Vec3& a, const Vec3& b);

Vec3& operator++(Vec3& a);
Vec3 operator++(Vec3& a, int);

float dot(const Vec3& a, const Vec3& b);