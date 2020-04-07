/*
 * Camera.h
 *
 *  Created on: Oct 9, 2019
 *      Author: Falkyn Worm
 */

#ifndef CAMERA_H_
#define CAMERA_H_
#include <vector>
#include "Eigen/Dense"
#include "Eigen/LU"
#include "Eigen/Geometry"
#include "Ray.h"


class Camera {
public:
	Eigen::Vector2d res;
	Eigen::Vector3d eye, look, up, W, U, V;
	Eigen::Vector4d bounds;
	int d;
	void setUVW();
	Ray pixel_ray(double i, double j);
	Camera();
	Camera(Eigen::Vector3d& e, Eigen::Vector3d& l, Eigen::Vector3d& u, int x, Eigen::Vector4d& bnds, Eigen::Vector2d r);
	virtual ~Camera();
	Camera(const Camera &other);
};


#endif /* CAMERA_H_ */
