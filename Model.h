/*
 * Model.h
 *
 *  Created on: Sep 14, 2019
 *      Author: Falkyn Worm
 */

#ifndef MODEL_H_
#define MODEL_H_
#include <fstream>
#include "Eigen/Dense"
#include <iostream>
#include <sstream>
#include <vector>

class Model {
public:
	int count;
	bool checked;
	std::string line;
	std::string name;
	std::string s;
	std::vector<std::string> faces;
	std::vector<std::string> start;
	std::vector<Eigen::Vector3d> vertices;
	std::fstream in;
	Eigen::MatrixXd verts;
	Eigen::Vector3d makeVertices(std::string vertices);
	Model();
	Model(const Model& model);
	Model(std::string object);
	virtual ~Model();
};

#endif /* MODEL_H_ */
