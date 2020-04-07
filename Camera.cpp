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

Camera::Camera(Eigen::Vector3d& e, Eigen::Vector3d& l, Eigen::Vector3d& u, int x, Eigen::Vector4d& bnds, Eigen::Vector2d r){
	eye = e; look = l; up = u; d = x; bounds = bnds; res = r;
	setUVW();
}

void Camera::setUVW(){
	W = eye - look; W.normalize();
	U = up.cross(W); U.normalize();
	V = W.cross(U);
}

Ray Camera::pixel_ray(double i, double j){
	double width = res[0], height = res[1],
			left = bounds[0], right = bounds[1], bottom = bounds[2], top = bounds[3];
	double x = i/(width-1)*(right-left)+left,
			y = j/(height-1)*(bottom-top)+top;
	Eigen::Vector3d L, u;
	L = eye + (d * W) + (x * U) + (y * V);
	u = L - eye;
	Ray ray(L, u);
	return ray;
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


