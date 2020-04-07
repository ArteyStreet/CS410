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
	full = false;

}

Sphere::Sphere(Eigen::Vector3d& loc, double rad, Material m){
	location = loc;
	r = rad;
	mat = m;
	full = true;
}

Sphere::~Sphere() {
	// TODO Auto-generated destructor stub
}

Sphere::Sphere(const Sphere &other) {
	// TODO Auto-generated constructor stub
	location = other.location;
	r = other.r;
	mat = other.mat;
	full = other.full;
}

Sphere& Sphere::operator=(const Sphere& other){
	location = other.location;
	r = other.r;
	mat = other.mat;
	full = other.full;
	return *this;
}

Eigen::Vector3d Sphere::getNormal(){
	return location;
}
