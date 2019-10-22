/*
 * Camera.cpp
 *
 *  Created on: Oct 9, 2019
 *      Author: Falkyn Worm
 */

#include "Camera.h"

Camera::Camera() {
	// TODO Auto-generated constructor stub
	d = 0;
}

Camera::~Camera() {
	// TODO Auto-generated destructor stub
}

Camera::Camera(const Camera &other) {
	// TODO Auto-generated constructor stub
	res = other.res;
	eye = other.eye;
	up = other.up;
	look = other.look;
	bounds = other.bounds;
	d = other.d;
}


