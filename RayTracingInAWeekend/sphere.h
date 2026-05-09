#pragma once

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable 
{
public:
    //Stationary sphere
    sphere(const vec3& static_center, double radius, std::shared_ptr<material> material) 
        : center(static_center, vec3(0,0,0)), radius(std::fmax(0, radius)) , mat(material)
    {
        vec3 rvec = vec3(radius, radius, radius);
        bbox = aabb(static_center - rvec, static_center + rvec);
    }

    //Moving sphere
    sphere(const vec3& center1, const vec3& center2, double radius, std::shared_ptr<material> material)
        : center(center1, center2 - center1), radius(std::fmax(0, radius)), mat(material)
    {
        vec3 rvec = vec3(radius, radius, radius);
        aabb box1(center.at(0) - rvec, center.at(0) + rvec);
        aabb box2(center.at(1) - rvec, center.at(1) + rvec);
        bbox = aabb(box1, box2);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override 
    {
        vec3 current_center = center.at(r.time());
        vec3 oc = current_center - r.origin();
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
        vec3 outward_normal = (rec.p - current_center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }

    aabb bounding_box() const override { return bbox; }

private:
    ray center;
    double radius;
    std::shared_ptr<material> mat;
    aabb bbox;
};