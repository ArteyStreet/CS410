#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cfloat>
#include <tuple>
#include "Camera.h"
#include "Sphere.h"
#include "Light.h"
#include "Ray.h"
#include "Eigen/Dense"
#include "Eigen/LU"
#include "Eigen/Geometry"

using namespace std;


Eigen::Vector3d U, V, W;
Eigen::MatrixXd smallest_ts;
int q, b;
Camera cam;
Light amb;
vector<Light> lights;
vector<Sphere> spheres;
vector<Ray> rays;

enum code{
	eye, look, up, d, bounds, res, ambient, light, sphere, null
};

code string_switch(const string& str){
	if (str == "eye") return eye;
	if (str == "look") return look;
	if (str == "up") return up;
	if (str == "d") return d;
	if (str == "bounds") return bounds;
	if (str == "res") return res;
	if (str == "ambient") return ambient;
	if (str == "light") return light;
	if (str == "sphere") return sphere;
	return null;
}

string pop_front(string& str){
	string temp = str.substr(0, str.find(' '));
	str.erase(0, str.find(' ') + 1);
	return temp;
}

Light make_light(string& line){
	Light light;
	double x = stod(pop_front(line)), y = stod(pop_front(line)), z = stod(pop_front(line)), w = stod(pop_front(line));
	double r = stod(pop_front(line)), g = stod(pop_front(line)), b = stod(pop_front(line));
	light.where = Eigen::Vector3d(x, y, z);
	light.w = w;
	light.emittance = Eigen::Vector3d(r, g, b);
	return light;
}

Sphere make_sphere(string& line){
	Sphere sphere;
	double x = stod(pop_front(line)), y = stod(pop_front(line)), z = stod(pop_front(line)), rad = stod(pop_front(line));
	double ar = stod(pop_front(line)), ag = stod(pop_front(line)), ab = stod(pop_front(line)),
			dr = stod(pop_front(line)), dg = stod(pop_front(line)), db = stod(pop_front(line)),
			sr = stod(pop_front(line)), sg = stod(pop_front(line)), sb = stod(pop_front(line)),
			rr = stod(pop_front(line)), rg = stod(pop_front(line)), rb = stod(pop_front(line));
	sphere.location = Eigen::Vector3d(x, y, z); sphere.r = rad;
	sphere.Ka = Eigen::Vector3d(ar, ag, ab); sphere.Kd = Eigen::Vector3d(dr, dg, db);
	sphere.Ks = Eigen::Vector3d(sr, sg, sb); sphere.Kr = Eigen::Vector3d(rr, rg, rb);

	return sphere;
}

void read(fstream in, string driver){

}

// RayTracing

Ray pixel_ray(double i, double j){
	q = i, b = j;
	double width = cam.res[0], height = cam.res[1],
			left = cam.bounds[0], right = cam.bounds[1], bottom = cam.bounds[2], top = cam.bounds[3];
	double x = i/(width-1)*(right-left)+left,
			y = j/(height-1)*(bottom-top)+top;
	Ray ray;
	ray.L = cam.eye + (cam.d * W) + (x * U) + (y * V);
	ray.U = ray.L - cam.eye; ray.U.normalize();
	return ray;
}

template <typename Derived>

bool ray_sphere_test(Ray r, Sphere s, double& t, Eigen::DenseBase<Derived>& pt){
	Eigen::Vector3d T = s.location - r.L;
	double v = T.dot(r.U), tsq = pow(T.norm(), 2), disc = pow(s.r, 2) - (tsq - pow(v,2));
	if (disc < 0){
		return false;
	}else{
		double dist = sqrt(disc);
		t = v-dist;
		pt = r.L + t * r.U;
		return true;
	}
}

template <typename Derived>

bool rayColor(Ray r, Sphere s, Eigen::DenseBase<Derived>& color){
	double t = 0;
	Eigen::Vector3d pt;
	if (ray_sphere_test(r, s, t, pt)){
		if (t < smallest_ts(q,b))
			smallest_ts(q,b) = t;
		Eigen::Vector3d sn = pt - s.location; sn.normalize();
		double d = smallest_ts(q,b);
		if (t > d)
			return false;
		for (int i = 0; i < 3; i++){
			color[i] = amb.emittance[i] * s.Ka[i];
		}
		for (Light l : lights){
			Eigen::Vector3d to = l.where - pt; to.normalize();
			if (sn.dot(to) > 0.0){
				for (int i = 0; i < 3; i++){
					color[i] += (l.emittance[i] * s.Kd[i]) * sn.dot(to);
				}
			}
		}
		return true;
	}else{
		return false;
	}
}

int main(int argc, char **argv) {
	if (argc != 3)
		cerr << "Invalid amount of arguments. Must be: ./raytracer driver_file.txt image_name.ppm\n";
	string driver = argv[1], image = argv[2], line;
	fstream in, out;
	amb.ambient = true;
	in.open(driver);
	while (getline(in, line)){
		string s = pop_front(line);
		switch (string_switch(s)){
			case eye:{
				string x = pop_front(line), y = pop_front(line), z = pop_front(line);
				cam.eye = Eigen::Vector3d(stod(x), stod(y), stod(z));
				break;}
			case look:{
				string x = pop_front(line), y = pop_front(line), z = pop_front(line);
				cam.look = Eigen::Vector3d(stod(x), stod(y), stod(z));
				break;}
			case up:{
				string x = pop_front(line), y = pop_front(line), z = pop_front(line);
				cam.up = Eigen::Vector3d(stod(x), stod(y), stod(z));
				break;}
			case d:
				cam.d = stod(line);
				break;
			case bounds:{
				string l = pop_front(line), r = pop_front(line), b = pop_front(line), t = pop_front(line);
				cam.bounds = Eigen::Vector4d(stod(l), stod(r), stod(b), stod(t));
				break;}
			case res:{
				string w = pop_front(line), h = pop_front(line);
				cam.res = Eigen::Vector2d(stod(w), stod(h));
				break;}
			case ambient:{
				double r = stod(pop_front(line)), g = stod(pop_front(line)), b = stod(pop_front(line));
				amb.emittance = Eigen::Vector3d(r, g, b);
				break;}
			case light:{
				Light lht = make_light(line);
				lights.push_back(lht);
				break;}
			case sphere:{
				Sphere sph = make_sphere(line);
				spheres.push_back(sph);
				break;}
			default:
				break;
		}
	}
	in.close();
	Eigen::Vector3d E = cam.eye, L = cam.look, UP = cam.up;
	W = E - L; W.normalize();
	U = UP.cross(W); U.normalize();
	V = W.cross(U);

	int width = cam.res[0], height = cam.res[1];

	smallest_ts.resize(height, width);
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			smallest_ts(i,j) = DBL_MAX;

	out.open(image, fstream::out);
	while (out.is_open()){
		out << "P3\n" << cam.res[0] << ' ' << cam.res[1] << " 255\n";
		Eigen::MatrixXd img(width*3, height);
		for (double j = 0; j < height; j++){
			for (double i = 0; i < width*3; i++){
				img(i, j) = 0;
			}
		}
		for(Sphere k : spheres){
			for (double j = 0; j < height; j++){
				for (double i = 0; i < width; i++){
					Eigen::Vector3d color;
					color.setZero();
					bool may = rayColor(pixel_ray(i,j), k, color);
					//cout << may << ' ';

					if (may){
						double r = color[0] * 255.0, g = color[1] * 255.0, b = color[2] * 255.0;
						Eigen::Vector3d RGB = Eigen::Vector3d(max(0.0,min(255.0,round(r))), max(0.0,min(255.0,round(g))), max(0.0,min(255.0,round(b))));
						img(i*3, j) = RGB[0], img(i*3+1, j) = RGB[1], img(i*3+2, j) = RGB[2];
					}
				}
			}
		}
		for (double j = 0; j < height; j++){
			for (double i = 0; i < width; i++){
				out << img(i*3, j) << ' ' << img(i*3+1, j) << ' ' << img(i*3+2, j) << ' ';
			}
			out << '\n';
		}
		out.close();
	}

	return 0;
}
