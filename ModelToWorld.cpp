#include <cstdlib>
#include <cmath>
#include <bits/stdc++.h>
#include "Eigen/Dense"
#include "Eigen/LU"
#include "Eigen/Geometry"
#include <fstream>
#include <iostream>
#include "Model.h"
#include <regex>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

using namespace std;

Eigen::Matrix4d rotate(Eigen::Matrix4d transformation, double wx, double wy, double wz, double angle){
	double rad = angle * (M_PI / 180), sa = sin(rad), ca = cos(rad);
	unsigned int j;
	Eigen::Vector3d axis(wx,wy,wz), W = axis, U, V;axis.normalize();
	for (unsigned int i = 0; i < axis.size(); i++)
		if (axis[i] == axis.minCoeff())
			j = i;
	W[j] = 1;
	U = axis.cross(W); U.normalize();
	V = axis.cross(U);
	Eigen::Matrix3d R, Rt, RRt;
	R << U, V, axis;
	Rt = R.transpose();
	RRt = R * Rt;

	Eigen::Matrix3d Rz;
	Rz << Rz.Identity();
	Rz(0,0) = ca; Rz(0,1) = sa;
	Rz(1,0) = -sa; Rz(1,1) = ca;

	Eigen::Matrix3d RT;
	RT = Rt * Rz * R;

	for (unsigned int i = 0; i < RT.rows(); i++)
		for (unsigned int j= 0; j < RT.cols(); j++)
			transformation(i,j) = RT(i,j);

	return transformation;
}

Eigen::Matrix4d scale(Eigen::Matrix4d transformation, double sc){
	transformation = sc*transformation;
	transformation(3,3) = 1;
	return transformation;
}

Eigen::Matrix4d translate(Eigen::Matrix4d transformation, double tx, double ty, double tz){
	transformation(0,3) += tx;
	transformation(1,3) += ty;
	transformation(2,3) += tz;
	return transformation;
}

string pad(int count){
	if (count < 10 && count >= 0)
		return "0" + to_string(count);
	return to_string(count);
}

void save(Model model, Eigen::Matrix4d transformation, string fldr){
	string num = pad(model.count);
	fldr.insert(fldr.size(), "/");
	string txt = model.name;
	txt.append("_transform_mw" + num + ".txt");
	txt.insert(0, fldr);
	cout << model.verts << '\n';
	string shape = model.name;
	shape.append("_mw" + num + ".obj");
	shape.insert(0, fldr);
	ofstream file(txt);
	while(file.is_open()){
		file << "# Transformation matrix\n" << fixed
				<< setprecision(3) << transformation << "\n \n"
				<< "# Inverse transformation matrix\n"
				<< transformation.inverse() << "\n \n" << setprecision(6)
				<< "# Sum absolute translations from original to transformed\n"
				<< "\n \n"
				<< "# Sum absolute translations from original to transformed to \"original\"\n"
				<< '\n';
		file.close();
	}
	file.open(shape);
	while (file.is_open()){
		for (string s : model.start)
			file << s <<'\n';
		for (unsigned int i = 0; i < model.vertices.size(); i++){
			file << "v ";
			for (int j = 0; j < 3; j++)
				file << model.verts.row(i)(j) << ' ';
		file << '\n';
		}
		file << model.s <<'\n';
		for (string s: model.faces)
			file << s << '\n';
		file.close();
	}
}

int main(int argc, char **argv) {
	string fldr = argv[1];
	int start = fldr.find_last_of('/') + 1, end = fldr.find_last_of('.');
	fldr = fldr.substr(start, end - start);
	cout<< fldr << '\n';
	cout<< mkdir(fldr.c_str(), 0777) << '\n';
	ifstream in;
	in.open(argv[1]);
	string line, obj;
	Eigen::Matrix4d transformation;
	transformation << transformation.Identity();
	vector<Model> models;
	double wx, wy, wz, angle, sc, tx, ty, tz;
	if (in.is_open()){
		while (getline(in, line)){
			int count = 0;
			istringstream word(line);
			for (string current; word >> current; ){
				switch (count){
					case 1:
						wx = stod(current);
						count++;
						break;
					case 2:
						wy = stod(current);
						count++;
						break;
					case 3:
						wz = stod(current);
						count++;
						break;
					case 4:
						angle = stod(current);
						count++;
						break;
					case 5:
						sc = stod(current);
						count++;
						break;
					case 6:
						tx = stod(current);
						count++;
						break;
					case 7:
						ty = stod(current);
						count++;
						break;
					case 8:
						tz = stod(current);
						count++;
						break;
					default:
						obj = current;
						count++;
						break;
				}
				current.clear();
			}
			Model model(obj);
			vector<Eigen::Vector3d> vertices = model.vertices;
			transformation = rotate(transformation, wx, wy, wz, angle);
			transformation = scale(transformation, sc);
			transformation = translate(transformation, tx, ty, tz);
			Eigen::MatrixXd verts(vertices.size(), 4);
			count = 0;
			for (Eigen::Vector3d vert: vertices){
				Eigen::Vector4d temp;
				temp<<vert,1;
				verts.row(count) = temp;
				count++;
			}
			verts.transposeInPlace();
			verts = transformation * verts;
			verts.transposeInPlace();
			model.verts = verts;
			models.push_back(model);
		}
		for (unsigned int i = 0; i < models.size() - 1; i++){
			int val = 0;
			for (unsigned int j = 1; j < models.size(); j++){
				if (!models[j].checked && !models[i].name.compare(models[j].name)){
					models[j].count = ++val;
					models[j].checked = true;
				}
			}
		}
		for (Model model : models){
			save(model, transformation, fldr);
		}
		in.close();
	}
	else cout << "File can't be opened. \n";
	return 0;
}
