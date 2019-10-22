/*
 * Model.h
 *
 *  Created on: Oct 9, 2019
 *      Author: Falkyn Worm
 */

#ifndef SPHERE_H_
#define SPHERE_H_
#include <vector>
#include "Eigen/Dense"
#include "Eigen/LU"

class Sphere {
public:
	Eigen::Vector3d location, Ka, Kd, Ks, Kr;
	double r;
	Sphere& operator=(const Sphere& other);
	Sphere();
	virtual ~Sphere();
	Sphere(const Sphere& other);
};

#endif /* SPHERE_H_ */
