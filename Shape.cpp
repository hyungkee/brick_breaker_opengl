#include "Shape.h"

//implement here
void Shape::move(Vec3 vec){
	center += vec;
}

void Shape::setPos(Vec3 vec){
	center = vec;
}

void Shape::setVel(Vec3 vec){
	velocity = vec;
}

void Shape::setColor(Vec3 color){
	rgb = color;
}

void Shape::setType(int const _type){
	type = _type;
}

void Shape::applyType(int const _type){
	type |= _type;
}

void Shape::removeType(int const _type){
	if (type & _type)
		type -= _type;
}

bool Shape::hasType(int const _type) const{
	return type & _type;
}
