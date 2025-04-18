#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"

class camera {
    public:
        // Image properties
        double aspect_ratio = 1.0;
        int image_width = 100;
        int samples_per_pixel = 10;
        int max_depth = 10;                     // max recursion depth for bouncing rays
        
        double vfov = 90;                       // vertical FOV in degrees
        point3 lookfrom = point3(0, 0, 0);      // point cam is looking from
        point3 lookat = point3(0, 0, -1);       // point camera is looking at
        vec3 vup = vec3(0, 1, 0);               // camera-relative up direction
        
        double defocus_angle = 0;               // variation angle of rays thru each pixel
        double focus_dist = 10;                 // distance from lookform to plane of perfect focus

        void render(const hittable& world, std::ostream& out = std::cout) {
            initialize();

            out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

            for (int j = 0; j < image_height; j++) {
                // \r is carriage return, which goes back to the beginning of current line
                // this ensures that the message is overwritten, not on a newline each time
                // std::flush forces the output buffer to be written to terminal immediately
                std::clog << "\rProgress: " << int(100.0 * j / image_height) << "%   " << std::flush;
                int bar_width = 50; // Width of the progress bar
                float progress = float(j) / image_height;
                int pos = int(bar_width * progress);

                std::clog << "\r[";
                for (int i = 0; i < bar_width; ++i) {
                    if (i < pos) std::clog << "=";
                    else if (i == pos) std::clog << ">";
                    else std::clog << " ";
                }
                std::clog << "] " << int(progress * 100.0) << "%   " << std::flush;

                for (int i = 0; i < image_width; i++) {
                    color pixel_color(0, 0, 0);
                    for (int sample = 0; sample < samples_per_pixel; sample++) {
                        ray r = get_ray(i, j);
                        pixel_color += ray_color(r, max_depth, world);
                    }
                    write_color(out, pixel_samples_scale * pixel_color);
                }
            }

            std::clog << "\rDone.                                                               \n";

        }
    
    private:
        int image_height;
        double pixel_samples_scale;
        point3 center;
        point3 pixel00_loc;
        vec3 pixel_delta_u;
        vec3 pixel_delta_v;
        vec3 u, v, w;                           // u is rightwards, v is upwards, w is backwards (oppo of lookat)
        vec3 defocus_disk_u;
        vec3 defocus_disk_v;

        void initialize() {
            // Calculate image properties
            image_height = int(image_width / aspect_ratio);
            image_height = std::max(1, static_cast<int>(std::round(image_width / aspect_ratio)));

            pixel_samples_scale = 1.0 / samples_per_pixel;

            center = lookfrom;

            // Specify and calculate the camera properties
            auto theta = degrees_to_radians(vfov);
            auto h = std::tan(theta / 2);
            auto viewport_height = 2 * h * focus_dist;
            auto viewport_width = viewport_height * (double(image_width) / image_height);

            // Calculate u,v,w unit vectors for camera coordinate frame
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            // Vectors along viewport edges
            auto viewport_u = viewport_width * u;
            auto viewport_v = viewport_height * -v;

            // Delta vectors between pixels
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // Upper left pixel calculation
            auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

            // Calculate camera defocus disk vectors
            auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_u = u * defocus_radius;
            defocus_disk_v = v * defocus_radius;
        }

        ray get_ray(int i, int j) const {
            // makes camera ray from defocus disk and directed at randomly sampled point around pixel loc i,j
            auto offset = sample_square();
            auto pixel_sample = pixel00_loc
                                + ((i + offset.x()) * pixel_delta_u)
                                + ((j + offset.y()) * pixel_delta_v);
            
            auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
            auto ray_direction = pixel_sample - ray_origin;

            return ray(ray_origin, ray_direction);
        }

        vec3 sample_square() const {
            // returns vector to random point within [-.5, -.5]-[+.5, +.5] square
            return vec3(random_double(-0.5, 0.5), random_double(-0.5, 0.5), 0);
        }

        point3 defocus_disk_sample() const {
            auto p = random_in_unit_disk();
            return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }

        color ray_color(const ray& r, int depth, const hittable& world) const {
            if (depth <= 0) return color(0, 0, 0);

            hit_record rec;

            if (world.hit(r, interval(0.001, infinity), rec)) {
                ray scattered;
                color attenuation;
                if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                    return attenuation * ray_color(scattered, depth - 1, world);
                }
                return color(0, 0, 0);
            }

            vec3 unit_direction = unit_vector(r.direction());
            auto a = 0.5 * (unit_direction.y() + 1.0);
            return ((1.0 - a) * color(1.0, 1.0, 1.0)) + (a * color(0.5, 0.7, 1.0));
        }
};

#endif
