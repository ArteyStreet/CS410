/*
 * Model.cpp
 *
 *  Created on: Oct 28, 2019
 *      Author: Falkyn Worm
 */

#include "Model.h"

using namespace std;

Eigen::Matrix4d Model::rotate(){
	double rad = theta * (M_PI / 180), sa = sin(rad), ca = cos(rad);
	unsigned int k = 0;
	Eigen::Vector3d axis(wx,wy,wz), W = axis, U, V;axis.normalize();
	for (unsigned int i = 0; i < axis.size(); i++)
		if (axis[i] == axis.minCoeff())
			k = i;
	W[k] = 1;
	U = axis.cross(W); U.normalize();
	V = axis.cross(U);
	Eigen::Matrix3d R, Rt, RtR;
	Eigen::Matrix4d rot; rot << rot.Identity();
	R << V, U, axis;
	Rt = R.transpose();
	RtR = R * Rt;
	Eigen::Matrix3d Rz;
	Rz << Rz.Identity();
	Rz(0,0) = ca; Rz(0,1) = -sa;
	Rz(1,0) = sa; Rz(1,1) = ca;
	Eigen::Matrix3d RT;
	RT = Rt * Rz * R;
	for (unsigned int i = 0; i < RT.rows(); i++)
		for (unsigned int j= 0; j < RT.cols(); j++)
			rot(i,j) = RT(i,j);
	return rot;
}
Eigen::Matrix4d Model::scale(){
	Eigen::Matrix4d scal; scal << scal.Identity();
	scal = sc*scal;
	scal(3,3) = 1;
	return scal;
}
Eigen::Matrix4d Model::translate(){
	Eigen::Matrix4d trans; trans << trans.Identity();
	trans(0,3) += tx;
	trans(1,3) += ty;
	trans(2,3) += tz;
	return trans;
}

void Model::transform(){
	count = 0;
	verts.resize(vertices.size(), 4);
	for (Eigen::Vector3d vert: vertices){
		Eigen::Vector4d temp;
		temp<<vert,1;
		verts.row(count) = temp;
		count++;
	}
	verts.transposeInPlace();
	verts = Transformation * verts;
	verts.transposeInPlace();
}


Face Model::makeFaces(string fa){
	stringstream ss(fa);
	string f, A, B, C, At, Bt, Ct, An, Bn, Cn, x, y, z;
	ss >> f >> x >> y >> z;
	A = x.substr(0, x.find('/')); x = x.substr(x.find('/')+1, x.size()-1);
	B = y.substr(0, y.find('/')); y = y.substr(y.find('/')+1, y.size()-1);
	C = z.substr(0, z.find('/')); z = z.substr(z.find('/')+1, z.size()-1);

//	At = x.substr(0, x.find('/')); x = x.substr(x.find('/')+1, x.size()-1);
//	Bt = y.substr(0, y.find('/')); y = y.substr(y.find('/')+1, y.size()-1);
//	Ct = z.substr(0, z.find('/')); z = z.substr(z.find('/')+1, z.size()-1);

	/*An = x.substr(0, x.find('/'));
	Bn = y.substr(0, y.find('/'));
	Cn = z.substr(0, z.find('/'));*/
	int a = stoi(A), b = stoi(B), c = stoi(C);
	Eigen::Vector3d Va(verts.row(a-1).segment(0,3)), Vb(verts.row(b-1).segment(0,3)), Vc(verts.row(c-1).segment(0,3));
	Face face(Va, Vb, Vc); face.vertices.push_back(a); face.vertices.push_back(b); face.vertices.push_back(c);
	/*if (!At.empty() && !Bt.empty() && !Ct.empty()){
		double at = stod(At), bt = stod(Bt), ct = stod(Ct);
		face.tex << at, bt, ct;
		Eigen::Vector3d U, V;
		U << vert_text[at-1](0), vert_text[bt-1](0), vert_text[ct-1](0);
		U << vert_text[at-1](1), vert_text[bt-1](1), vert_text[ct-1](1);
		face.calculateDOF(U, V);
	}*/
	//cout << face.A << '\n' << face.B << '\n' << face.C << '\n' << face.N << '\n' << '\n';
	return face;
}

void Model::makeText(string text){
	stringstream ss(text);
	double u, v;
	string vt;
	ss >> vt >> u >> v;
	Eigen::Vector2d vet(u, v);// vet.transposeInPlace();
	vert_text.push_back(vet);
}

void Model::parseFaces(){
	int count = 0;
	for(string s : f){
		stringstream ss(s);
		string front, mat;
		ss >> front;
		if (front == "f"){
			Face q = makeFaces(s);
			q.mat = mtl;
			q.count = ++count;
			//cout << q.mat.Kd.transpose() << endl;
			faces.push_back(q);
		}else if (front == "usemtl"){
			ss >> mat;
			for(Material mt : materials)
				if (mt.name == mat)
					mtl = mt;
		}
	}
}

void Model::defaultMtl(){
	Eigen::Vector3d Ka(0.2, 0.2, 0.2), Ks(0.9, 0.9, 0.9), Kd(0.95, 0.95, 0.95), Kr(0.9, 0.9, 0.9);
	mtl = Material(Ka, Ks, Kd, Kr, 1);
}

void Model::makeMaterial(){
	ifstream in(mtlib);
	string p;
	if (in.is_open()){
		while (getline(in, p)){
			stringstream ss(p);
			string beg, name, text;
			ss >> beg;
			if (beg == "newmtl"){
				double Ns, Ni, d, r, g, b;
				int illum;
				Eigen::Vector3d Ka, Ks, Kd, Ke, Tr;
				Tr = Eigen::Vector3d().Zero();
				ss >> name;
				while (getline(in, p)){
					string here;
					stringstream ll(p);
					ll >> here;
					if (here == "Ns") { ll >> Ns; }
					else if (here == "Ka") { ll >> r >> g >> b; Ka << r, g, b; }
					else if (here == "Kd") { ll >> r >> g >> b; Kd << r, g, b; }
					else if (here == "Ks") { ll >> r >> g >> b; Ks << r, g, b; }
					else if (here == "Ke") { ll >> r >> g >> b; Ke << r, g, b; }
					else if (here == "Ni") { ll >> Ni; }
					else if (here == "Tr") { ll >> r >> g >> b; Tr << r, g, b; }
					else if (here == "d") { ll >> d; }
					else if (here == "illum") { ll >> illum; }
					else if (here == "map_kd") { ll >> text; text.insert(0, "./models/"); }
					else if (beg == "newmtl") { break; }
				}
				Material mt(name, Ns, Ka, Kd, Ks, Ke, Ni, d, illum);
				if (Tr.sum() != 0)
					mt.Tr = Tr;
				/*if (!text.empty()){
					mt.hasText = true;
					//mt.texture = cimg_library::CImg<unsigned char>(text.c_str());
					//cout << mt.texture._height << '\n';
				}*/
				materials.push_back(mt);
			}
		}
		in.close();
	}
	if (materials.empty())
			defaultMtl();
}

void Model::smooth(){
	int un = 0;
	for (Face f1 : faces){
		f1.smooth = true;
		f1.nA += f1.N; f1.nB += f1.N; f1.nC += f1.N;
		int count = 0;
		for (int v1 : f1.vertices){
//			cout << v << '\n';
			for (Face f2 : faces){
				for (int v2 : f2.vertices){
					if (v2 == v1){
						int ang = acos(f1.N.dot(f2.N)) * 180 / M_PI;
//						cout << ang << '\n';
						if (ang < smoothing && ang > 0){
							switch (count) {
								case 0:
									f1.nA += f2.N;
									break;
								case 1:
									f1.nB += f2.N;
									break;
								case 2:
									f1.nC += f2.N;
									break;
								default:
									continue;
							}
						}
					}
				}
			}
			count++;
		}
		f1.nA.normalize(); f1.nB.normalize(); f1.nC.normalize();
		faces[un] = f1;
		un++;
	}
}

Model::Model(){
	defaultMtl();
	checked = false;
	count = 0;
	Transformation << Eigen::Matrix4d::Identity();
	wx = 0.0; wy = 0.0; wz = 0.0; theta = 0.0; sc = 0.0; tx = 0.0; ty = 0.0; tz = 0.0;
}

Model::Model(double w1, double w2, double w3, double ang, double sca, double t1, double t2, double t3, int sm, string mod) {
	// TODO Auto-generated constructor stub
	wx = w1; wy = w2; wz = w3; theta = ang; sc = sca; tx = t1; ty = t2; tz = t3; full = true; smoothing = sm;
	Eigen::Matrix4d rot, scal, trans;
	parseFile(mod);
	Transformation << Eigen::Matrix4d::Identity();
	rot = rotate(); scal = scale(); trans = translate();
	Transformation = trans * scal * rot;
	transform();
	makeMaterial();
	parseFaces();
	if (smoothing)
		smooth();
}

Model::Model(const Model& model){
	full = model.full;
	mtl = model.mtl;
	name = model.name;
	s = model.s; mtlib = model.mtlib;
	faces = model.faces; f = model.f; vert_norm = model.vert_norm;
	start = model.start; materials = model.materials;
	vertices = model.vertices; verts = model.verts;
	count = model.count; vert_text = model.vert_text;
	checked = model.checked; smoothing = model.smoothing;
	Transformation = model.Transformation;
	wx = model.wx; wy = model.wy; wz = model.wz; theta = model.theta;
	sc = model.sc; tx = model.tx; ty = model.ty; tz = model.tz;
}

Model& Model::operator=(const Model& model){
	full = model.full;
	mtl = model.mtl;
	name = model.name;
	s = model.s; mtlib = model.mtlib;
	faces = model.faces; f = model.f; vert_norm = model.vert_norm;
	start = model.start; materials = model.materials;
	vertices = model.vertices; verts = model.verts;
	count = model.count; vert_text = model.vert_text;
	checked = model.checked;
	Transformation = model.Transformation;
	wx = model.wx; wy = model.wy; wz = model.wz; theta = model.theta;
	sc = model.sc; tx = model.tx; ty = model.ty; tz = model.tz;
	return *this;
}


void Model::parseFile(string obj){
	ifstream in;
	name = obj.substr(0, obj.find('.'));
	obj = "./models/" + obj;
	in.open(obj);
	if (in.is_open()){
		while(getline(in, line)){
			switch(line.front()){
				// faces
				case 'f':
					f.push_back(line);
					break;
				// s value
				case 's':
					s = line;
					break;
				// vertices
				case 'v':
				{
					if (line.compare(0,2, "vn") == 0){

					}else if (line.compare(0,2, "vt") == 0){
						makeText(line);
					}else{
						Eigen::Vector3d vert;
						vert = makeVertices(line);
						vertices.push_back(vert);
					}
					break;
				}
				// mtlib
				case 'm':{
					stringstream ss(line);
					string ma;
					ss >> ma >> mtlib;
					mtlib.insert(0, "./models/");
					break;}
				// usemtl
				case 'u':{
					f.push_back(line);
					break;}
				default:
					break;
			}
		}
	}
}



Eigen::Vector3d Model::makeVertices(string vertice){
	string v;
	double x, y, z;
	Eigen::Vector3d verts;
	stringstream word(vertice);
	word >> v >> x >> y >> z;
	verts << x, y, z;
	return verts;
}

Model::~Model() {
	// TODO Auto-generated destructor stub
}
