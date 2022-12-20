#pragma once
#include "utils.h"
#include "scene.h"
#include "parser.h"

inline bool intersect(Ray& r, double& dist, int& id, Parser* p) {
    int n = p->scene.size();
    dist = INF; id = -1;
    for(int i = 0; i < n; ++i) {
        if(p->scene[i]->type == SPHERE){
            Sphere* sph = (Sphere*)p->scene[i];
            double t = sph->intersect(r);
            if(t!=0 && t<dist) { // if hit, find nearest object
                dist = t; id = i;
            }
        }
    }
    return dist < INF;
}

Vec3d path_tracing(Ray& r, int depth, unsigned short* Xi, Parser* p, int off) {
    // Monte Carlo Path Tracing with Russian Roulette
    double dist; int id;
    Vec3d emission;
    Material material;
    Vec3d n, nl; // normal
    Vec3d pos;
    Vec3d diff_c; // diffuse color (BRDF modulator)

    if (!intersect(r, dist, id, p)) return Vec3d(0.0, 0.0, 0.0); // no hit, return black
    else { // hit
        if(p->scene[id]->type == SPHERE){
            Sphere* sph = (Sphere*)p->scene[id];
            pos = r.ori + r.dir * dist; // hit position
            n = normalize(pos - sph->center);
            diff_c = sph->color;
            material = sph->material;
            emission = sph->emission;
        }
    }

    bool into = dot(n, r.dir) < 0; // ray from outside going in
    nl = into ? n : n * (-1); // actual normal (consider into or out)
    depth += 1; // path tracing depth
    double prob = std::max(std::max(diff_c.x[0], diff_c.x[1]), diff_c.x[2]); // max rgb (as probability for R.R)
    if (prob != 0) { // Russian Roulette
        if (erand48(Xi) < prob) diff_c = diff_c / prob; // R.R expectation
        else return emission * (1 - off); // terminate
    }
    if (depth > 10) return Vec3d(0.0, 0.0, 0.0); // exceed max depth, path tracing terminate

    // select a direction and carry on path tracing
    if (material == SPECULAR) {
        // ideal mirror reflection, determined direction
        Vec3d reflect_dir = r.dir - n * 2 * dot(n, r.dir); // D - 2*(N.D) N
        Ray reflect_r = Ray(pos, reflect_dir);
        return emission + multiple(diff_c, path_tracing(reflect_r, depth, Xi, p, 0)); // recursion
    }
    else if (material == DIFFUSE) {
        // Next Event Estimation
        Vec3d nee(0.0, 0.0, 0.0);
        for (int i = 0; i < p->scene.size(); ++i) {
            // loop over every light sources
            if (p->scene[i]->emission.x[0] <= 0.0 && p->scene[i]->emission.x[1] <= 0.0 
                && p->scene[i]->emission.x[2] <= 0.0) continue;
            if (p->scene[i]->type == SPHERE) {
                Sphere* sph = (Sphere*)p->scene[i];
                // set up coordinate (u, v, w)
                Vec3d w = sph->center - pos;
                Vec3d tmp = fabs(w.x[0]) > 0.1 ? Vec3d(0.0, 1.0, 0.0) : Vec3d(1.0, 0.0, 0.0);
                Vec3d u = normalize(cross(tmp, w));
                Vec3d v = cross(w, u);

                double cos_a_max = sqrt(1 - (sph->R * sph->R) / dot(pos - sph->center, pos - sph->center));
                double lamda = erand48(Xi);
                double cos_a = lamda * cos_a_max + (1.0 - lamda) * 1.0;
                double sin_a = sqrt(1.0 - cos_a * cos_a);
                double phi = 2 * M_PI * erand48(Xi);
                // sample from light
                Vec3d light_dir = normalize(u * cos(phi) * sin_a + v * sin(phi) * sin_a + w * cos_a);
                Ray light_r = Ray(pos, light_dir);
                if (intersect(light_r, dist, id, p) && id==i) { // shadow ray
                    // render equation
                    nee = nee + multiple(sph->emission * dot(light_dir, nl), diff_c / M_PI) * 2 * M_PI * (1 - cos_a_max);
                }
            }
        }
        // Indirect Illumination (from other object)
        double theta = 2 * M_PI * erand48(Xi);
        double r2 = sqrt(erand48(Xi));
        // set up coordinate (u, v, w)
        Vec3d w = nl;
        Vec3d tmp = fabs(w.x[0]) > 0.1 ? Vec3d(0.0, 1.0, 0.0) : Vec3d(1.0, 0.0, 0.0);
        Vec3d u = normalize(cross(tmp, w));
        Vec3d v = cross(w, u);
        // randomly sample a direction
        Vec3d indirect_dir = normalize(u * cos(theta) * r2 + v * sin(theta) * r2 + w * sqrt(1 - r2 * r2));
        Ray indirect_r = Ray(pos, indirect_dir);
        // render equation
        return emission * (1 - off) + nee + multiple(path_tracing(indirect_r, depth, Xi, p, 1), diff_c);
    }
    else {
        // refraction
        double n1 = 1.0, n2 = 1.5; // air and glass refractivity
        double nnt = into ? n1 / n2 : n2 / n1; // relative refractivity
        double cos_theta = -dot(r.dir, nl);
        double inter_reflec_cond = 1 + nnt * nnt * cos_theta * cos_theta - nnt * nnt;
        Vec3d reflect_d = r.dir - n * 2 * dot(n, r.dir); // D - 2*(N.D) N
        Ray reflect_r = Ray(pos, reflect_d);
        if (inter_reflec_cond < 0) { // total internal reflection
            return emission + multiple(path_tracing(reflect_r, depth, Xi, p, 0), diff_c); // recursion
        }
        double tmp = -cos_theta * nnt + sqrt(inter_reflec_cond);
        tmp = into ? tmp : tmp * (-1);
        Vec3d refrac_d = normalize(r.dir * nnt - n * tmp); // refraction ray
        Ray refrac_r = Ray(pos, refrac_d);
        double R_0 = ((n2 - n1) * (n2 - n1)) / ((n2 + n1) * (n2 + n1)); 
        double c = into ? 1 - cos_theta : 1 - dot(n, refrac_d);
        double R_theta = R_0 + (1 - R_0) * pow(c, 5); // reflect ratio
        double T_theta = 1 - R_theta;                 // refract ratio
        double P = 0.25 + 0.5 * R_theta;
        double RP = R_theta / P, TP = T_theta / (1 - P); // R.R expectation
        if (depth <= 2) {
            return emission + multiple(path_tracing(reflect_r, depth, Xi, p, 0) * R_theta + 
                path_tracing(refrac_r, depth, Xi, p, 0) * T_theta, diff_c);
        }
        // Russian Roulette
        if (erand48(Xi) < P) return emission + multiple(path_tracing(reflect_r, depth, Xi, p, 0) * RP, diff_c);
        else return emission + multiple(path_tracing(refrac_r, depth, Xi, p, 0) * TP, diff_c);
    }
}