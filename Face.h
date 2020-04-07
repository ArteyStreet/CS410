/*
 * Face.h
 *
 *  Created on: Oct 30, 2019
 *      Author: halcyonic
 */

#ifndef FACE_H_
#define FACE_H_

#include <cstdlib>
#include <iostream>
#include "Eigen/Dense"
#include "Eigen/LU"
#include "Eigen/Geometry"
#include "Material.h"
#include <vector>
//#include "Object.h"

class Face{
public:
	Eigen::Vector3d A, B, C, N, nA, nB, nC, tex;
	Eigen::Matrix<double, 3, 2> DOF;
	//Eigen::Matrix3d M, M1, M2, M3;
	std::vector<int> vertices;
	int count;
	bool full, smooth;
	Material mat;
	Face(Eigen::Vector3d& a, Eigen::Vector3d& b, Eigen::Vector3d& c);
	Face();
	virtual ~Face();
	bool calculateDOF(Eigen::Vector3d& U, Eigen::Vector3d& V);
	Face(const Face &other);
	Face& operator=(const Face &other);
	Eigen::Vector3d getNormal();
};

#endif /* FACE_H_ */
