#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : public hittable {
    public:
        sphere(const point3& center, double radius) : center(center), radius(std::fmax(0,radius)) {}

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // say b = -2h
            vec3 oc = center - r.origin();
            auto a = r.direction().length_squared();
            auto h = dot(r.direction(), oc);
            auto c = oc.length_squared() - (radius * radius);

            auto discriminant = (h * h) - (a * c);
            if (discriminant < 0) { // D > 0 two real soln, D = 0 one real soln, D < 0 no real solutions
                return false;
            }
            
            auto sqrtd = std::sqrt(discriminant);

            // Nearest root in acceptable range
            auto root = (h - sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                root = (h + sqrtd) / a;
                if (!ray_t.surrounds(root)) {
                    return false;
                }
            }

            rec.t = root;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);

            return true;
        }

    private:
        point3 center;
        double radius;
};

#endif
