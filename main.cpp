#include "camera.h"
#include "parser.h"
#include "pt.h"
#include "scene.h"
#include "utils.h"

int main(int argc, char *argv[]) {
    Parser parser;
    parser.build();
    Camera camera(parser.W, parser.H, parser.fov, parser.pos, parser.lookat);
    
    int w = parser.W, h = parser.H;
    int spp = parser.spp / 4; // 2x2 subpixel
    Vec3d c;
    Vec3d **rgb_map; // image
    rgb_map = new Vec3d*[h];
    for(int i = 0; i < h; ++i) rgb_map[i] = new Vec3d[w];

    std::cout << "Rendering spp " << spp * 4 << ", [" << h << " " << w << "]" << std::endl;
    #pragma omp parallel for schedule(dynamic) private(c) // OpenMP compile
    for(int y = 0; y < h; ++y) { // rows
        unsigned short Xi[3] = {0, 0, y*y*y};
        for(int x = 0; x < w; ++x) { // cols
            for(int sy = 0; sy < 2; ++sy) { // 2x2 subpixel rows
                for (int sx = 0; sx < 2; ++sx) { // 2x2 subpixel cols
                    c = Vec3d(0.0, 0.0, 0.0);
                    for (int s = 0; s < spp; ++s) { // spp
                        double dx = erand48(Xi) - 0.5; // [-0.5, 0.5]
                        double dy = erand48(Xi) - 0.5; // [-0.5, 0.5]
                        double x_in_subpixel = x + (sx + 0.5) * 0.5 + dx * 0.5;
                        double y_in_subpixel = y + (sy + 0.5) * 0.5 + dy * 0.5;
                        double new_x = x_in_subpixel / w - 0.5;
                        double new_y = y_in_subpixel / h - 0.5;
                        Vec3d offset_x = camera.cam_x_axis * new_x;
                        Vec3d offset_y = camera.cam_y_axis * new_y;
                        Vec3d d = normalize(camera.lookat + offset_x + offset_y);
                        Ray r = Ray(camera.pos, d);
                        c = c + path_tracing(r, 0, Xi, &parser, 0) / spp;
                    }
                    rgb_map[y][x] = rgb_map[y][x] + Vec3d(clamp(c.x[0]), clamp(c.x[1]), clamp(c.x[2])) / 4;
                }
            }
        }
    }

    // Write image to PPM file.
    FILE *f = fopen("image.ppm", "w");
    fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
    for (int i = h - 1; i >= 0; --i) {
        for (int j = 0; j < w; ++j) {
            fprintf(f,"%d %d %d ", gamma_corr(rgb_map[i][j].x[0]), 
                gamma_corr(rgb_map[i][j].x[1]), gamma_corr(rgb_map[i][j].x[2]));
        }
    }
    fclose(f); // finish writing

    return 0;
}