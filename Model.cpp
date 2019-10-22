/*
 * Model.cpp
 *
 *  Created on: Sep 14, 2019
 *      Author: Falkyn Worm
 */

#include "Model.h"

Model::Model(){
	checked = false;
	count = 0;
}

Model::Model(const Model& model){
	name = model.name;
	s = model.s;
	faces = model.faces;
	start = model.start;
	vertices = model.vertices;
	verts = model.verts;
	count = model.count;
	checked = model.checked;
}

Model::Model(std::string object) {
	// TODO Auto-generated constructor stub
	name = object.substr(0, object.find('.'));
	object = "./models/" + object;
	in.open(object);
	if (in.is_open()){
		while(getline(in, line)){
			switch(line.front()){
				case '#':
					start.push_back(line);
					break;
				case 'f':
					faces.push_back(line);
					break;
				case 's':
					s = line;
					break;
				case 'v':
				{
					if (line.compare(0,2, "vn") != 0){
					Eigen::Vector3d vert;
					vert = makeVertices(line);
					vertices.push_back(vert);
					}
					break;
				}
				default:
					break;
			}
		}
	}

}

Eigen::Vector3d Model::makeVertices(std::string vertices){
	int count=0;
	double x, y, z;
	Eigen::Vector3d verts;
	std::istringstream word(vertices);
	for (std::string current; word >> current; ){
		switch (current.front()){
			case 'v':
				count = 0;
				break;
			default:
				switch(count){
					case 1:
						x = std::stod(current);
						break;
					case 2:
						y = std::stod(current);
						break;
					case 3:
						z = std::stod(current);
						break;
					default:
						std::cout<<count<<'\n';
						break;
				}
				break;
		}
		count++;
	}
	verts << x, y, z;
	return verts;
}

Model::~Model() {
	// TODO Auto-generated destructor stub
}
