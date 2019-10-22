/*
 * Ray.h
 *
 *  Created on: Oct 10, 2019
 *      Author: halcyonic
 */

#ifndef RAY_H_
#define RAY_H_
#include "Eigen/Dense"
#include "Eigen/LU"
#include "Eigen/Geometry"


class Ray {
public:
	Eigen::Vector3d L, U;
	Ray();
	virtual ~Ray();
	Ray(const Ray &other);
};

#endif /* RAY_H_ */
