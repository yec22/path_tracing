#pragma once
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>

const double EPS = 1e-5;
const double INF = 1e10;

struct Vec3d {
	double x[3]; // data
    
    Vec3d() {}
	Vec3d(const Vec3d& v){
        x[0] = v.x[0]; x[1] = v.x[1]; x[2] = v.x[2];
    }
    Vec3d(double x_, double y_, double z_){
        x[0] = x_; x[1] = y_; x[2] = z_;
    }

	// arithmetic
    Vec3d operator+(const Vec3d& v) const{
        return Vec3d(x[0]+v.x[0], x[1]+v.x[1], x[2]+v.x[2]);
    } 
    Vec3d operator-(const Vec3d& v) const{
        return Vec3d(x[0]-v.x[0], x[1]-v.x[1], x[2]-v.x[2]);
    } 
    Vec3d operator*(double d) {
        return Vec3d(x[0]*d, x[1]*d, x[2]*d);
    } 
    Vec3d operator/(double d) {
        return Vec3d(x[0]/d, x[1]/d, x[2]/d);
    } 
}; 

inline double dot(const Vec3d& v1, const Vec3d& v2) {return v1.x[0]*v2.x[0]+v1.x[1]*v2.x[1]+v1.x[2]*v2.x[2];}

inline double len(const Vec3d& v) {return sqrt(dot(v, v));}

inline Vec3d normalize(const Vec3d& v) {return Vec3d(v.x[0]/len(v), v.x[1]/len(v), v.x[2]/len(v));}

inline Vec3d cross(const Vec3d& v1, const Vec3d& v2) {
    return Vec3d(v1.x[1] * v2.x[2] - v1.x[2] * v2.x[1],
	             v1.x[2] * v2.x[0] - v1.x[0] * v2.x[2],
	             v1.x[0] * v2.x[1] - v1.x[1] * v2.x[0]);
}

inline Vec3d multiple(const Vec3d& v1, const Vec3d& v2) {
    return Vec3d(v1.x[0]*v2.x[0], v1.x[1]*v2.x[1], v1.x[2]*v2.x[2]);
}

inline double clamp(double x) {
    // clamp x into [0, 1]
    if (x < 0.0) return 0.0;
    if (x > 1.0) return 1.0;
    return x;
}

inline int gamma_corr(double x) {
    // 2.2 gamma correction
    return int(pow(clamp(x), 1.0/2.2) * 255.0 + 0.5);
} 