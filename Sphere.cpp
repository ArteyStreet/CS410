/*
 * Sphere.cpp
 *
 *  Created on: Oct 9, 2019
 *      Author: Falkyn Worm
 */

#include "Sphere.h"

Sphere::Sphere() {
	// TODO Auto-generated constructor stub
	r = 0;

}

Sphere::~Sphere() {
	// TODO Auto-generated destructor stub
}

Sphere::Sphere(const Sphere &other) {
	// TODO Auto-generated constructor stub
	location = other.location;
	r = other.r;
	Ka = other.Ka;
	Kd = other.Kd;
	Ks = other.Ks;
	Kr = other.Kr;
}

Sphere& Sphere::operator=(const Sphere& other){
	Sphere temp(other);
	return temp;
}

