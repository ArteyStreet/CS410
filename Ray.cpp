/*
 * Ray.cpp
 *
 *  Created on: Oct 10, 2019
 *      Author: halcyonic
 */

#include "Ray.h"
#include <iostream>

using namespace std;

Ray::Ray() {
	// TODO Auto-generated constructor stub
	best_t = FLT_MAX; in = 0;  full = false; refr = false;
}

Ray::Ray(Eigen::Vector3d& loc, Eigen::Vector3d& dir){
	L = loc; U = dir; U.normalize(); best_t = FLT_MAX; in = 0; full = true; best_sph = Sphere();
	best_face = Face(); best_mod = Model(); refr = false;
}

Ray::~Ray() {
	// TODO Auto-generated destructor stub
}

Ray::Ray(const Ray &other) {
	// TODO Auto-generated constructor stub
	L = other.L; U = other.U; best_t = other.best_t; best_face = other.best_face; best_mod = other.best_mod; refr = other.refr;
	best_sph = other.best_sph; best_pt = other.best_pt; in = other.in; full = other.full; current_face = other.current_face; current_mod = other.current_mod;

}

Ray& Ray::operator=(const Ray& other){
	L = other.L; U = other.U; best_t = other.best_t; best_face = other.best_face; best_mod = other.best_mod; refr = other.refr;
	best_sph = other.best_sph; best_pt = other.best_pt; in = other.in; full = other.full; current_face = other.current_face; current_mod = other.current_mod;
	return *this;
}

