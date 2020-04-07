/*
 * Model.h
 *
 *  Created on: Oct 9, 2019
 *      Author: Falkyn Worm
 */

#ifndef SPHERE_H_
#define SPHERE_H_
#include <vector>
#include "Material.h"
#include <cmath>
//#include "Object.h"

class Sphere{
public:
	Eigen::Vector3d location, Ka, Kd, Ks, Kr;
	double r;
	bool full;
	Material mat;
	Sphere& operator=(const Sphere& other);
	Sphere();
	Sphere(Eigen::Vector3d& loc, double rad, Material m);
	virtual ~Sphere();
	Sphere(const Sphere& other);
	Eigen::Vector3d getNormal();
};

#endif /* SPHERE_H_ */
