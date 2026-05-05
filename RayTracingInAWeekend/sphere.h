#pragma once

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable 
{
public:
    sphere(const vec3& center, double radius, std::shared_ptr<material> material) 
        : center(center), radius(std::fmax(0, radius)) , mat(material)
    {
    
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override 
    {
        vec3 oc = center - r.origin();
        double a = r.direction().length_squared();
        double h = dot(r.direction(), oc);
        double c = oc.length_squared() - radius * radius;

        double discriminant = h * h - a * c;
        if (discriminant < 0) { return false; }

        double sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        double root = (h - sqrtd) / a;
        if (root <= ray_t.min || ray_t.max <= root) 
        {
            root = (h + sqrtd) / a;
            if (root <= ray_t.min || ray_t.max <= root) { return false; }
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }

private:
    vec3 center;
    double radius;
    std::shared_ptr<material> mat;
};