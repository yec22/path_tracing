#pragma once
#include "utils.h"

enum Material {DIFFUSE, SPECULAR, REFRACTIVE};
enum Element {SPHERE, PLANE};

struct Ray {
    Vec3d ori; // origin
    Vec3d dir; // direction

    Ray(Vec3d ori_, Vec3d dir_){
        ori = ori_; dir = dir_;
    }
    Ray(const Ray& r){
        ori = r.ori; dir = r.dir;
    }
};

class Base {
    public:
        Vec3d emission;
        Vec3d color;
        Material material;
        Element type;

        Base() {}
        Base(Vec3d emi_, Vec3d col_, Material mat_, Element typ_){
            emission = emi_; color = col_; material = mat_; type = typ_;
        }
};

class Sphere: public Base {
    public:
        Vec3d center;
        double R; // radius

        Sphere(): Base() {}
        Sphere(Vec3d cen_, double R_, Vec3d emi_, Vec3d col_, Material mat_): Base(emi_, col_, mat_, SPHERE) {
            center = cen_; R = R_;
        } 
        double intersect(Ray& r) {
            // D.D*t^2 + 2D.(O-C)t + (O-C).(O-C) - R^2 = 0
            Vec3d OC = r.ori - center;
            double a = dot(r.dir, r.dir);
            double b = 2 * dot(r.dir, OC);
            double c = dot(OC, OC) - R * R;
            double det = b * b - 4 * a * c;
            if (det < 0.0) return 0.0; // no hit
            double t1 = (-b - sqrt(det)) / (2 * a);
            double t2 = (-b + sqrt(det)) / (2 * a);
            if (t1 > EPS) return t1;
            else if (t2 > EPS) return t2;
            else return 0.0;
        }
};