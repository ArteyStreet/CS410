#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cfloat>
#include <tuple>
#include "Model.h"
#include "Camera.h"
#include "Sphere.h"
#include "Light.h"
#include "Ray.h"
#include "Eigen/Dense"
#include "Eigen/LU"
#include "Eigen/Geometry"
#include <chrono>

using namespace std;

Camera cam;
Light amb;
int dep;
vector<Light> lights;
vector<Sphere> spheres;
vector<Ray> rays;
vector<Model> models;
Eigen::Matrix3d M, M1, M2, M3;
double eta_outside = 1;

enum code{
	eye, look, up, d, bounds, res, ambient, light, sphere, model, depth, null
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
	if (str == "model") return model;
	if (str == "recursionlevel") return depth;
	return null;
}

string pop_front(string& str){
	string temp = str.substr(0, str.find(' '));
	str.erase(0, str.find(' ') + 1);
	return temp;
}

Light make_light(string& line){
	double x, y, z, w, r, g, b;
	stringstream ss(line);
	ss >> x >> y >> z >> w >> r >> g >> b;
	Eigen::Vector3d here(x, y, z), emit(r, g, b);
	Light light(here, emit, w);
	return light;
}

Sphere make_sphere(string& line){
	double x, y, z, rad, ar, ag, ab, dr, dg, db, sr, sg, sb, rr, rg, rb, i;
	stringstream ss(line);
	ss >> x >> y >> z >> rad >> ar >> ag >> ab >> dr >> dg >> db >> sr >> sg >> sb >> rr >> rg >> rb >> i;
	Eigen::Vector3d Ka(ar, ag, ab), Kd(dr, dg, db), Ks(sr, sg, sb), Kr(rr, rg, rb), loc(x, y, z);
	Material mat(Ka, Kd, Ks, Kr, i);
	Sphere sph(loc, rad, mat);

	return sph;
}

Model make_model(string& line){
	double wx, wy, wz, theta, sc, tx, ty, tz;
	int sm;
	string mod;
	stringstream ss(line);
	ss >> wx >> wy >> wz >> theta >> sc >> tx >> ty >> tz >> sm >> mod;
	Model mode(wx, wy, wz, theta, sc, tx, ty, tz, sm, mod);
	return mode;
}



// RayTracing

bool sphere_intersect(Sphere& sph, Ray& r){
	Eigen::Vector3d T = sph.location - r.L;
	double v = T.dot(r.U), tsq = T.dot(T), disc = pow(sph.r, 2) - (tsq - pow(v,2));
	if (disc < 0){
		return false;
	}else{
		double dist = sqrt(disc);
		double t = v-dist;
		if ((t > 0.00001) && (t < r.best_t)){
			r.best_t = t;
			r.best_pt = r.L + t * r.U;
			r.best_sph = sph;
			return true;
		}
		return false;
	}
}

bool face_intersect(Face& f, Ray& r){
	double beta, gamma, t, det1, det2, det3, detM;
	M1.setZero(); M2.setZero(); M3.setZero(); M.setZero();
	M << f.A-f.B, f.A-f.C, r.U; M.transpose();
	Eigen::Vector3d R, Pb, Y(f.A - r.L);
//	cout << Y.transpose() << '\n';
	M1 = M; M2 = M; M3 = M;
	for (int i = 0; i < 3; i++){
		M1(i,0) = Y[i];
		M2(i,1) = Y[i];
		M3(i,2) = Y[i];
	}
	//cout << f.A << ' ' << f.B << '\n' << '\n';
	detM = M.determinant(); if (detM == 0) return false;
	det1 = M1.determinant(); beta = det1/detM; if (beta < 0.0) return false;
	det2 = M2.determinant(); gamma = det2/detM; if (gamma < 0.0) return false;
	det3 = M3.determinant(); t = det3/detM; if (t < 0) return false;
	//cout << r.best_t << '\n';
	if (t > 0.00001 && t < r.best_t){
		if (r.in || (beta + gamma) <= 1){
			if (f.smooth && !r.in)
				f.N = (1-gamma-beta) * f.nA + beta * f.nB + gamma * f.nC;
			r.best_face = f;
			r.best_t = t;
			r.best_pt = r.L + t * r.U;
			return true;
		}
	}
	return false;
}

Sphere& ray_find_sphere(Ray& r){
	for (Sphere s : spheres)
		sphere_intersect(s, r);
	return r.best_sph;
}

Face& ray_find_model(Ray& r){
	for (Model m : models){
		if (r.refr && m.name.compare(r.current_mod.name) == 0)
			continue;
		for (Face f : m.faces){
			double dot = f.getNormal().dot(r.current_face.getNormal());
			if (dot == 1){
				continue;
			}
//			cout << f.smooth << '\n';
			if (face_intersect(f, r))
				r.best_mod = m;
		}
	}
	return r.best_face;
}

Face& ray_find_inside(Ray& r, Model& m, Face& fa){
	for (Face f : m.faces){
		double dot = f.getNormal().dot(fa.getNormal());
		if (dot == 1){
			continue;
		}
		face_intersect(f, r);
	}
	return r.best_face;
}


bool shadow(Light lt, Eigen::Vector3d& pt){
	Eigen::Vector3d L = lt.where - pt;
	Ray r(pt, L);
	double d = L.dot(r.U);
	if ((ray_find_sphere(r).full || ray_find_model(r).full) && r.best_t < d){
		return true;
	}
	return false;
}

Eigen::Vector3d refract_ray(Eigen::Vector3d& W, Eigen::Vector3d& N, double eta1, double eta2){
	double etar = eta1 / eta2, a = -1 * etar, wn = W.dot(N), radsq = pow(etar, 2) * (pow(wn,2) - 1) + 1;
//	cout  << etar << ',';
	if (radsq < 0.0)
		return Eigen::Vector3d().Zero();
	else {
		double b = (etar * wn) - sqrt(radsq);
		Eigen::Vector3d T = W; T *= a; T += b * N;
		return T;
	}
}

Ray sphere_exit(Sphere& s, Eigen::Vector3d& pt, Eigen::Vector3d& W, double eta_in, double eta_out){
	Eigen::Vector3d N = pt - s.location; N.normalize();
	Eigen::Vector3d T1 = refract_ray(W, N, eta_out, eta_in);
	//cout << T1.transpose() << ':' << W.transpose() << '\n';
//	r.in = true;
	if (T1.sum() == 0.0)
		return Ray();
	else{
		N = s.location - pt;
		Eigen::Vector3d exit, Nin, Tn = T1; exit = pt + 2 * T1.dot(N) * T1; Nin = s.location - exit; Tn *= -1; Nin.normalize();
		Eigen::Vector3d T2 = refract_ray(Tn, Nin, eta_in, eta_out);
//		r.in = false;
		Ray refR(exit, T2);
		return refR;
	}
}

Ray model_exit(Model& m, Face& f, Eigen::Vector3d& pt, Eigen::Vector3d& W, Eigen::Vector3d N, double eta_in, double eta_out){
	Eigen::Vector3d T1 = refract_ray(W, N, eta_out, eta_in); T1 *= -1;
//	cout << T1.transpose() << ':' << W.transpose() << '\n';
	if (T1.sum() == 0.0)
		return Ray();
	else {
		Ray in(pt, T1); in.in = true;
		Face hit = ray_find_inside(in, m, f);
		//cout << hit.full << '\n';
		if (hit.full){
			Eigen::Vector3d Nin = hit.getNormal();
			if (Nin.dot(T1) > 0.0){
				Nin *= -1;
			} T1 *= -1;
			Eigen::Vector3d T2 = refract_ray(T1, Nin, eta_in, eta_out); T2 *= -1;
			in = Ray(in.best_pt, T2); in.in = false;
			in.current_face = f;
		}
		return in;
	}
}

void illumination(Ray r, Eigen::Vector3d& N, Material mat, Eigen::Vector3d& rgb, Eigen::Vector3d& ref){
	Eigen::Vector3d color; color.setZero();
	for (int i = 0; i < 3; i++)
		color[i] = amb.emittance[i] * mat.Ka[i];
	for (Light l : lights){
		Eigen::Vector3d to = l.where - r.best_pt; to.normalize();
		double Nto = N.dot(to);
		if (Nto > 0.0 && !shadow(l,r.best_pt)){
			for (int i = 0; i < 3; i++)
				color[i] += mat.Kd[i] * l.emittance[i] * Nto;
			Eigen::Vector3d toC = r.L - r.best_pt, spR; spR = (2 * Nto * N) - to; toC.normalize(), spR.normalize();
			double Cr = toC.dot(spR);
			//cout << Cr << '\n';
			if (Cr > 0.0)
				for (int i = 0; i < 3; i++)
					color[i] += (mat.Ks[i] * l.emittance[i]) * pow(Cr,mat.Ns);
		}
	}
	for (int i = 0; i < 3; i++)
		rgb[i] += ref[i] * color[i];
}


void ray_trace(Ray& r, Eigen::Vector3d& rgb, Eigen::Vector3d& ref, int level){
	if (ray_find_sphere(r).full){
//		cout << "yes\n";
		Eigen::Vector3d N = r.best_pt - r.best_sph.location; N.normalize();
		Material mat = r.best_sph.mat;
		illumination(r, N, mat, rgb, ref);
		if (level > 0){
			Eigen::Vector3d Uinv = r.U, refR, flec = Eigen::Vector3d().Zero(); Uinv *= -1;
			refR = (2 * N.dot(Uinv) * N) - Uinv; refR.normalize();
			if (N.dot(refR) >= 0.0001){
				Eigen::Vector3d tem; Ray refl(r.best_pt, refR);
				if (mat.Kr.sum() != 0){
					for (int i = 0; i < 3; i++)
						tem[i] = mat.Kr[i] * ref[i];
					ray_trace(refl, flec, tem, (level - 1));
					for (int i = 0; i < 3; i++)
						rgb[i] += flec[i] * ref[i];
				}
				if (mat.Tr.sum() != 0){
					Eigen::Vector3d thru = Eigen::Vector3d().Zero(), W = r.U; W *= -1;
					Ray fraR = sphere_exit(r.best_sph, r.best_pt, W, mat.Ni, eta_outside);
					//cout << r.U.transpose() << ':' << fraR.U.transpose() << '\n';
					if (fraR.full){
						Eigen::Vector3d tem;
						for (int i = 0; i < 3; i++)
							tem[i] = mat.Tr[i] * ref[i];
						ray_trace(fraR, thru, tem, (level - 1));
						for (int i = 0; i < 3; i++) {
							rgb[i] += thru[i] * ref[i];
							if (rgb[i] > 1.0)
								rgb[i] = 1.0;
						}
					}
				}
			}
		}
	}
	//cout << "ray_find_model()\n";
	if (ray_find_model(r).full){
		//cout << r.best_mod.name << '\n';
		//cout << "hit\n";
		Eigen::Vector3d N = r.best_face.N; N.normalize();
		if (r.best_face.getNormal().dot(r.U) > 0.0)
			N *= -1;
		Material mat = r.best_face.mat;
		illumination(r, N, mat, rgb, ref);
		if (level > 0){
			Eigen::Vector3d flec = flec.Zero(),Uinv = r.U, refR; Uinv *= -1;
			refR = (2 * N.dot(Uinv) * N) - Uinv; refR.normalize();
			if (r.best_face.getNormal().dot(refR) >= 0.0001){
				Eigen::Vector3d tem;
				if (mat.Kr.sum() != 0){
					//if (!r.in){
//					if (mat.hasText){
//						flec += ;
//					}
						for (int i = 0; i < 3; i++)
							tem[i] = mat.Kr[i] * ref[i];
						Ray refl(r.best_pt, refR);
						ray_trace(refl, flec, tem, (level - 1));
						for (int i = 0; i < 3; i++)
							rgb[i] += flec[i] * ref[i];
					//}
				}
				//		Model refraction
				if (mat.Tr.sum() != 0){
					Eigen::Vector3d W = r.U, thru = Eigen::Vector3d().Zero(), x = r.best_face.getNormal(); W *= -1;
					if (x.dot(r.U) > 0.0)
						x *= -1;
					Ray refract = model_exit(r.best_mod, r.best_face, r.best_pt, W, x, mat.Ni, eta_outside);
					refract.refr = true; refract.current_mod = r.best_mod;
					//cout << r.U.transpose() << ':' << refract.U.transpose() << '\n';
					if (refract.full){
						for (int i = 0; i < 3; i++)
							tem[i] = mat.Tr[i] * ref[i];
						ray_trace(refract, thru, tem, (level - 1));
//						cout << refract.best_mod.name << '\n';
						for (int i = 0; i < 3; i++) {
							rgb[i] += thru[i] * ref[i];
							if (rgb[i] > 1.0)
								rgb[i] = 1.0;
						}
					}
				}
			}
		}
	}
}


int main(int argc, char **argv) {
	auto start_time = chrono::high_resolution_clock::now();
	if (argc != 3)
		cerr << "Invalid amount of arguments. Must be: ./raytracer driver_file.txt image_name.ppm\n";
	string driver = argv[1], image = argv[2], line;
	fstream in, out;
	amb.ambient = true;
	in.open(driver);
	while (getline(in, line)){
		string s = pop_front(line);
		stringstream ss(line);
		switch (string_switch(s)){
			case eye:{
				double x, y, z;
				ss >> x >> y >> z;
				cam.eye = Eigen::Vector3d(x, y, z);
				break;}
			case look:{
				double x, y, z;
				ss >> x >> y >> z;
				cam.look = Eigen::Vector3d(x, y, z);
				break;}
			case up:{
				double x, y, z;
				ss >> x >> y >> z;
				cam.up = Eigen::Vector3d(x, y, z);
				break;}
			case d:
				ss >> cam.d;
				break;
			case bounds:{
				double l, r, b, t;
				ss >> l >> r >> b >> t;
				cam.bounds = Eigen::Vector4d(l, r, b, t);
				break;}
			case res:{
				double w, h;
				ss >> w >> h;
				cam.res = Eigen::Vector2d(w, h);
				break;}
			case ambient:{
				double r, g, b;
				ss >> r >> g >> b;
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
			case model:{
				Model mod = make_model(line);
				models.push_back(mod);
				//	cout << "Model Made" <<'\n';
				break;}
			case depth:{
				dep = stoi(pop_front(line));
				break;}
			default:
				break;
		}
	}
	in.close();
	cam.setUVW();

	int width = cam.res[0], height = cam.res[1];

	out.open(image, fstream::out);
	while (out.is_open()){
		out << "P3\n" << width << ' ' << height << " 255\n";
		Eigen::MatrixXd img(width*3, height);
		for (double j = 0; j < height; j++){
			for (double i = 0; i < width*3; i++){
				img(i, j) = 0;
			}
		}
		float percent = float(height) / float(32);
		int progress = 0;
		for (double j = 0; j < height; j++){
			if(j >= percent * progress){
			 	auto current_time = chrono::high_resolution_clock::now();
				cout << "\rProgress:[" + string(progress, '#') + string(32-progress, ' ') + "] "
		        		<< (float(progress) / float(32)) * 100 << "%\n";
				cout << chrono::duration_cast<chrono::hours>(current_time-start_time).count() << ":" << (int)chrono::duration_cast<chrono::minutes>(current_time-start_time).count() % 60 << ":"
				     		<< (int)chrono::duration_cast<chrono::seconds>(current_time-start_time).count() % 60 << " Passed" << endl;
				progress += 1;
			}
			for (double i = 0; i < width; i++){
				Ray ray; ray = cam.pixel_ray(i, j);
				//cout << ray.best_t << '\n';
				Eigen::Vector3d rgb, reff; rgb.setZero(); reff << 1.0, 1.0, 1.0;
				ray_trace(ray, rgb, reff, dep);
				double r = rgb[0] * 255.0, g = rgb[1] * 255.0, b = rgb[2] * 255.0;
				Eigen::Vector3d RGB = Eigen::Vector3d(max(0.0,min(255.0,round(r))), max(0.0,min(255.0,round(g))), max(0.0,min(255.0,round(b))));
				img(i*3, j) = RGB[0], img(i*3+1, j) = RGB[1], img(i*3+2, j) = RGB[2];
				out << img(i*3, j) << ' ' << img(i*3+1, j) << ' ' << img(i*3+2, j) << ' ';
			}
			out << '\n';
		}
		out.close();
	}

 	auto current_time = chrono::high_resolution_clock::now();
			
	cout << chrono::duration_cast<chrono::hours>(current_time-start_time).count() << ":" << (int)chrono::duration_cast<chrono::minutes>(current_time-start_time).count() % 60 << ":"
	     << (int)chrono::duration_cast<chrono::seconds>(current_time-start_time).count() % 60 << " Passed" << endl;
	return 0;
}
