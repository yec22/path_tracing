#pragma once
#include "utils.h"
#include "scene.h"
#include <fstream>
#include <vector>
#include <string>

class Parser {
    public:
        std::vector<Base*> scene;
        std::string config_file;
        int W, H; // width, height
        int spp; // samples per pixel
        Vec3d pos; // position
        Vec3d lookat; // camera lookat
        double fov; // vertical fov

        Parser() {}
        Parser(std::string file) {config_file = file;}
        void build() {
            // build the scene from config file
            scene.push_back(new Sphere(Vec3d(1e5+1,40.8,81.6), 1e5, Vec3d(0.0,0.0,0.0), Vec3d(0.75,0.25,0.25), DIFFUSE));
            scene.push_back(new Sphere(Vec3d(-1e5+99,40.8,81.6), 1e5, Vec3d(0.0,0.0,0.0), Vec3d(0.25,0.25,0.75), DIFFUSE));
            scene.push_back(new Sphere(Vec3d(50,40.8,1e5), 1e5, Vec3d(0.0,0.0,0.0), Vec3d(0.75,0.75,0.75), DIFFUSE));
            scene.push_back(new Sphere(Vec3d(50,1e5,81.6), 1e5, Vec3d(0.0,0.0,0.0), Vec3d(0.75,0.75,0.75), DIFFUSE));
            scene.push_back(new Sphere(Vec3d(50,-1e5+81.6,81.6), 1e5, Vec3d(0.0,0.0,0.0), Vec3d(0.75,0.75,0.75), DIFFUSE));
            scene.push_back(new Sphere(Vec3d(27,16.5,47), 16.5, Vec3d(0.0,0.0,0.0), Vec3d(1,1,1)*0.999, SPECULAR));
            scene.push_back(new Sphere(Vec3d(73,16.5,78), 16.5, Vec3d(0.0,0.0,0.0), Vec3d(1,1,1)*0.999, REFRACTIVE));
            scene.push_back(new Sphere(Vec3d(50,81.6-16.5,81.6), 1.5, Vec3d(4,4,4)*100, Vec3d(0.0,0.0,0.0), DIFFUSE));

            W = 1024, H = 768;
            spp = 400;
            pos = Vec3d(50.0, 52.0, 300.0);
            lookat = Vec3d(0.0, -0.05, -1.0);
            fov = M_PI / 6;
        }
};