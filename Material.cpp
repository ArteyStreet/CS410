/*
 * Material.cpp
 *
 *  Created on: Oct 29, 2019
 *      Author: halcyonic
 */

#include "Material.h"

using namespace std;

Material::Material(){
	hasText = false;
	textw = 0; texth = 0;
	Ns = 0;
	Illum = 2;
	Ni = 1.0;
	d = 1.0;
}

Material::Material(string n, double N, Eigen::Vector3d& a, Eigen::Vector3d& kd, Eigen::Vector3d& s, Eigen::Vector3d& e, double i, double x, int Ill) {
	// TODO Auto-generated constructor stub
	name = n; Ns = N; d = x;
	Ka = a; Kd = kd; Ks = s; Ke = e;
	Ni = i; Illum = Ill; texth = 0; textw = 0;
	switch (Illum){
		case 3:
			Tr = Eigen::Vector3d().Zero();
			Kr = Ks;
			break;
		case 6:
			Kr = Ks;
			Tr = Eigen::Vector3d().Ones() - Kr;
			break;
		default:
			Tr = Eigen::Vector3d().Zero();
			Kr = Eigen::Vector3d().Zero();
			break;
	}
}

Material::Material(Eigen::Vector3d& a, Eigen::Vector3d& kd, Eigen::Vector3d& s, Eigen::Vector3d& r, double i){
	Ni = i;
	Ns = 16; /*texture = NULL; */texth = 0; textw = 0;
	Ka = a; Kd = kd; Ks = s; Kr = r; d = 1.0; hasText = false;
	if (i){
		Illum = 6;
		Tr = Eigen::Vector3d().Ones() - Kr;
	}
	else
		Illum = 3;
}

Material::Material(const Material& other){
	name = other.name; Tr = other.Tr;// texture = other.texture;
	Ns = other.Ns; Ni = other.Ni; d = other.d; hasText = other.hasText;
	Ka = other.Ka; Ks = other.Ks; Kd = other.Kd; Kr = other.Kr;
	Ke = other.Ke; Illum = other.Illum;texth = other.texth; textw = other.textw;
}


Material& Material::operator=(const Material& other){
	name = other.name; Tr = other.Tr; d = other.d; hasText = other.hasText;
	Ns = other.Ns; Ni = other.Ni; //texture = other.texture;
	Ka = other.Ka; Ks = other.Ks; Kd = other.Kd; Kr = other.Kr;
	Ke = other.Ke; Illum = other.Illum;  texth = other.texth; textw = other.textw;
	return *this;
}

Material::~Material() {
	// TODO Auto-generated destructor stub
}

