#pragma once

#include "hittable.h"
#include "onb.h"
#include "vec3.h"

class sphere : public hittable 
{
public:
    //Stationary sphere
    sphere(const vec3& static_center, float radius, std::shared_ptr<material> material) 
        : center(static_center, vec3(0,0,0)), radius(std::fmax((float)0, radius)) , mat(material)
    {
        vec3 rvec = vec3(radius, radius, radius);
        bbox = aabb(static_center - rvec, static_center + rvec);
    }

    //Moving sphere
    sphere(const vec3& center1, const vec3& center2, float radius, std::shared_ptr<material> material)
        : center(center1, center2 - center1), radius(std::fmax((float)0, radius)), mat(material)
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
        float a = r.direction().length_squared();
        float h = dot(r.direction(), oc);
        float c = oc.length_squared() - radius * radius;

        float discriminant = h * h - a * c;
        if (discriminant < 0) { return false; }

        float sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        float root = (h - sqrtd) / a;
        if (root <= ray_t.min || ray_t.max <= root) 
        {
            root = (h + sqrtd) / a;
            if (root <= ray_t.min || ray_t.max <= root) { return false; }
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - current_center) / radius;
        rec.set_face_normal(r, outward_normal);
        get_sphere_uv(outward_normal, rec.u, rec.v);
        rec.mat = mat;

        return true;
    }

    aabb bounding_box() const override { return bbox; }

    float pdf_value(const vec3& origin, const vec3& direction) const override 
    {
        // This method only works for stationary spheres.

        hit_record rec;
        if (!this->hit(ray(origin, direction), interval(0.001f, infinity), rec))
        {
            return 0;
        }

        float dist_squared = (center.at(0) - origin).length_squared();
        float cos_theta_max = std::sqrt(1 - radius * radius / dist_squared);
        float solid_angle = 2 * pi * (1 - cos_theta_max);

        return  1 / solid_angle;
    }

    vec3 random(const vec3& origin) const override 
    {
        vec3 direction = center.at(0) - origin;
        auto distance_squared = direction.length_squared();
        onb uvw(direction);
        return uvw.transform(random_to_sphere(radius, distance_squared));
    }

private:
    ray center;
    float radius;
    std::shared_ptr<material> mat;
    aabb bbox;

    static void get_sphere_uv(const vec3& p, float& u, float& v) 
    {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        float theta = std::acosf(-p.y);
        float phi = std::atan2f(-p.z, p.x) + pi;

        u = phi / (2 * pi);
        v = theta / pi;
    }

    static vec3 random_to_sphere(float radius, float distance_squared) 
    {
        float r1 = random_float();
        float r2 = random_float();
        float z = 1 + r2 * (std::sqrt(1 - radius * radius / distance_squared) - 1);

        float phi = 2 * pi * r1;
        float x = std::cosf(phi) * std::sqrt(1 - z * z);
        float y = std::sinf(phi) * std::sqrt(1 - z * z);

        return vec3(x, y, z);
    }
};