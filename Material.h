/*
 * Material.h
 *
 *  Created on: Oct 29, 2019
 *      Author: halcyonic
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_
#include "Eigen/Dense"
#include "Eigen/LU"
//#include "CImg-2.8.0/CImg.h"
#include "Light.h"

class Material {
public:
	Eigen::Vector3d Ka, Kd, Ks, Kr, Ke, Tr;
	double Ns, Ni, d;
	Light Emissive;
	int Illum, texth, textw;
	std::string name;
	bool hasText;
	//cimg_library::CImg<unsigned char> texture;
	Material();
	Material(std::string n, double N, Eigen::Vector3d& a, Eigen::Vector3d& kd, Eigen::Vector3d& s, Eigen::Vector3d& e, double i, double x, int Ill);
	Material(Eigen::Vector3d& a, Eigen::Vector3d& kd, Eigen::Vector3d& s, Eigen::Vector3d& r, double i);
	Material(const Material& other);
	Material& operator=(const Material& other);
	virtual ~Material();
};

#endif /* MATERIAL_H_ */
