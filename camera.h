#pragma once
#include "utils.h"

class Camera {
    public:
        Vec3d pos; // position
        Vec3d lookat; // camera lookat
        double fov; // vertical fov
        int W, H; // width, height
        Vec3d cam_x_axis; // cx
        Vec3d cam_y_axis; // cy

        Camera() {}
        Camera(int w_, int h_, double fov_, Vec3d pos_, Vec3d lookat_) {
            pos = pos_;
            lookat = normalize(lookat_);
            W = w_; H = h_;
            fov = fov_;
            double fov_scale = 2 * tan(fov / 2);
            cam_x_axis = Vec3d(W * fov_scale / H, 0.0, 0.0);
            cam_y_axis = normalize(cross(cam_x_axis, lookat)) * fov_scale;
        }
};