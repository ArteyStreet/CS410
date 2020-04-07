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
#include "Sphere.h"
#include "Model.h"
#include "float.h"



class Ray {
public:
	Eigen::Vector3d L, U, best_pt;
	bool in, full, refr;
	Sphere best_sph;
	Face best_face, current_face;
	Model best_mod, current_mod;
	float best_t;
	Ray();
	Ray(Eigen::Vector3d& loc, Eigen::Vector3d& dir);
	virtual ~Ray();
	Ray(const Ray &other);
	Ray& operator=(const Ray& other);
};

#endif /* RAY_H_ */
