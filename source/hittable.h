#ifndef HITTABLE_H
#define HITTABLE_H

class material;

class hit_record {
    public:
        point3 p;
        vec3 normal;
        shared_ptr<material> mat;
        double t;
        bool front_face;

        void set_face_normal(const ray& r, const vec3& outward_normal) {
            // outward_normal should have unit_length
            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }
};

class hittable {
    public:
        // use compiler-generated destructor
        virtual ~hittable() = default;

        // pure virtual function, making this class an abstract class (virtual, = 0)
        // abstract class: 
        // virtual keyword enables runtime polymorphism
        // “When calling this function on a pointer or reference to a base class, 
        // use the derived class’s version of the function — if it exists.”
        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif
