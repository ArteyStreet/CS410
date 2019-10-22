/*
 * Light.cpp
 *
 *  Created on: Oct 9, 2019
 *      Author: Falkyn Worm
 */

#include "Light.h"

using namespace std;

Light::Light() {
	// TODO Auto-generated constructor stub
	ambient = false;
	w = 1;
}

Light::~Light() {
	// TODO Auto-generated destructor stub
}

Light::Light(const Light &other) {
	// TODO Auto-generated constructor stub
	where = other.where;
	emittance = other.emittance;
	w = other.w;
	ambient = other.ambient;
}

Light& Light::operator=(const Light& other){
	Light temp(other);
	return temp;
}

