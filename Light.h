/*
 * Light.h
 *
 *  Created on: Oct 9, 2019
 *      Author: Falkyn Worm
 */

#ifndef LIGHT_H_
#define LIGHT_H_
#include <vector>
#include "Eigen/Dense"
#include "Eigen/LU"

class Light {
public:
	Eigen::Vector3d where, emittance;
	bool ambient;
	int w;
	Light& operator=(const Light& other);
	Light();
	virtual ~Light();
	Light(const Light& other);
};

#endif /* LIGHT_H_ */
