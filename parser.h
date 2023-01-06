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
        double translate; // camera translate

        Parser() {}
        Parser(std::string file) {config_file = file;}
        void build() {
            // build the scene from config file
            std::ifstream conf_f(config_file.c_str()); // pre-C++11.
            std::string tmp;
            
            // parse config file
            while(conf_f >> tmp) {
                if(tmp == std::string("END")) break;
                if(tmp == std::string("IMAGE")) { // image
                    conf_f >> tmp; conf_f >> H;
                    conf_f >> tmp; conf_f >> W;
                    conf_f >> tmp; conf_f >> spp;
                }
                if(tmp == std::string("CAMERA")) { // camera
                    double x, y, z;
                    conf_f >> tmp; conf_f >> x >> y >> z; pos = Vec3d(x, y, z);
                    double u, v, w;
                    conf_f >> tmp; conf_f >> u >> v >> w; lookat = Vec3d(u, v, w);
                    conf_f >> tmp; conf_f >> fov;
                    conf_f >> tmp; conf_f >> translate;
                }
                if(tmp == std::string("SCENE")) { // scene
                    int n; conf_f >> n;
                    for(int i = 0; i < n; ++i) {
                        conf_f >> tmp;
                        if(tmp == std::string("Plane")) {
                            double a, b, c; 
                            Vec3d p, n, emi, col;
                            Material m;
                            conf_f >> tmp >> a >> b >> c; p = Vec3d(a, b, c);
                            conf_f >> tmp >> a >> b >> c; n = Vec3d(a, b, c);
                            conf_f >> tmp >> a >> b >> c; emi = Vec3d(a, b, c);
                            conf_f >> tmp >> a >> b >> c; col = Vec3d(a, b, c);
                            conf_f >> tmp >> tmp;
                            if(tmp == std::string("DIFFUSE")) m = DIFFUSE;
                            else if(tmp == std::string("SPECULAR")) m = SPECULAR;
                            else if(tmp == std::string("REFRACTIVE")) m = REFRACTIVE;
                            scene.push_back(new Plane(p, n, emi, col, m));
                        }
                        else if(tmp == std::string("Sphere")) {
                            double a, b, c, r;
                            Vec3d p, emi, col;
                            Material m;
                            conf_f >> tmp >> a >> b >> c; p = Vec3d(a, b, c);
                            conf_f >> tmp >> r;
                            conf_f >> tmp >> a >> b >> c; emi = Vec3d(a, b, c);
                            conf_f >> tmp >> a >> b >> c; col = Vec3d(a, b, c);
                            conf_f >> tmp >> tmp;
                            if(tmp == std::string("DIFFUSE")) m = DIFFUSE;
                            else if(tmp == std::string("SPECULAR")) m = SPECULAR;
                            else if(tmp == std::string("REFRACTIVE")) m = REFRACTIVE;
                            scene.push_back(new Sphere(p, r, emi, col, m));
                        }
                    }
                }
            }
        }
};