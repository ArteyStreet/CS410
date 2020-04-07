/*
 * Face.cpp
 *
 *  Created on: Oct 30, 2019
 *      Author: halcyonic
 */

#include "Face.h"

using namespace std;
using namespace Eigen;

Face::Face() {
	// TODO Auto-generated constructor stub
	A.setZero();B.setZero();C.setZero(); full = false; smooth = false; count = 0;
}

Face::Face(Vector3d& a, Vector3d& b, Vector3d& c){
	//M.setZero(); M1.setZero(); M2.setZero(); M3.setZero();
	A = a; B = b; C = c; full = true; smooth = false;
	Vector3d x = B - A, y = C - A; count = 0;
	nA << nA.setZero(); nB << nB.setZero(); nC << nC.setZero();
	N = x.cross(y); N.normalize();// tN = N;
}

Face::~Face() {
	// TODO Auto-generated destructor stub
}

Face::Face(const Face &other) {
	// TODO Auto-generated constructor stub
	A = other.A; B = other.B; C = other.C; smooth = other.smooth; count = other.count;
	N = other.N; full = other.full; mat = other.mat; vertices = other.vertices;
	nA = other.nA; nB = other.nB; nC = other.nC; tex = other.tex; DOF = other.DOF;
}

bool Face::calculateDOF(Vector3d& U, Vector3d& V){
	Matrix3d M, Min; M << A, B, C; M.transposeInPlace();
	if (M.determinant() != 0){
		Min = M.inverse();
		Vector3d a = Min * U, b = Min * V;
		DOF << a, b;
		return true;
	}
	return false;
}


Face& Face::operator=(const Face &other) {
	// TODO Auto-generated constructor stub
	A = other.A; B = other.B; C = other.C; vertices = other.vertices; count = other.count;
	N = other.N; full = other.full; mat = other.mat; smooth = other.smooth;
	nA = other.nA; nB = other.nB; nC = other.nC; tex = other.tex; DOF = other.DOF;
	return *this;
}

Vector3d Face::getNormal(){
	Vector3d x = B - A, y = C - A, N = x.cross(y); N.normalize();
	return N;
}
