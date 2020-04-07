/*
 * Model.h
 *
 *  Created on: Sep 14, 2019
 *      Author: Falkyn Worm
 */

#ifndef MODEL_H_
#define MODEL_H_
#include <cstdlib>
#include <cmath>
#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
//#include "CImg-2.8.0/CImg.h"
#include "Face.h"

class Model{
public:
	int count, smoothing;
	bool checked, full;
	double wx, wy, wz, theta, sc, tx, ty,tz;
	std::string line, name, s, mtlib;
	std::vector<std::string> f, start;
	std::vector<Face> faces;
	std::vector<Material> materials;
	std::vector<Eigen::Vector3d> vertices, vert_norm;
	std::vector<Eigen::Vector2d> vert_text;
	Eigen::MatrixXd verts;
	std::fstream in;
	Eigen::Matrix4d Transformation;
	Material mtl;
	Eigen::Vector3d makeVertices(std::string vertice);
	Face makeFaces(std::string fa);
	void makeText(std::string text);
	void defaultMtl();
	void makeMaterial();
	void transform();
	void smooth();
	Model();
	Model(double w1, double w2, double w3, double ang, double sca, double t1, double t2, double t3, int sm, std::string mod);
	Eigen::Matrix4d rotate();
	Eigen::Matrix4d scale();
	Eigen::Matrix4d translate();
	void parseFile(std::string obj);
	void parseFaces();
	Model(const Model& model);
	Model& operator=(const Model& model);
	virtual ~Model();
};

#endif /* MODEL_H_ */
