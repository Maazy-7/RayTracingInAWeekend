#pragma once

#include "hittable.h"
#include "vec3.h"
#include "aabb.h"

class quad : public hittable 
{
public:
    quad(const vec3& Q, const vec3& u, const vec3& v, std::shared_ptr<material> mat)
        : Q(Q), u(u), v(v), mat(mat)
    {
        vec3 n = cross(u,v);
        normal = unit_vector(n);
        d = dot(normal, Q);
        w = n / dot(n,n);
        set_bounding_box();
    }

    virtual void set_bounding_box() 
    {
        // Compute the bounding box of all four vertices.
        aabb bbox_diagonal1 = aabb(Q, Q + u + v);
        aabb bbox_diagonal2 = aabb(Q + u, Q + v);
        bbox = aabb(bbox_diagonal1, bbox_diagonal2);
    }

    aabb bounding_box() const override { return bbox; }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override 
    {
        float denom = dot(normal, r.direction());
        //if ray parallel to plane then no intersection
        if (std::fabs(denom) < 1e-8) //almost zero dot product means ray and plane dont intersect
        {
            return false;
        }

        // Return false if the hit point parameter t is outside the ray interval.
        float t = (d - dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t))
        {
            return false;
        }
        
        // Determine if the hit point lies within the planar shape using its plane coordinates.
        vec3 intersection = r.at(t);
        vec3 planar_hitpt_vector = intersection - Q;
        float alpha = dot(w, cross(planar_hitpt_vector, v));
        float beta = dot(w, cross(u, planar_hitpt_vector));

        if (!is_interior(alpha, beta, rec))
        {
            return false;
        }
        // Ray hits the 2D shape; set the rest of the hit record and return true.


        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }

    virtual bool is_interior(float a, float b, hit_record& rec) const 
    {
        interval unit_interval = interval(0, 1);
        // Given the hit point in plane coordinates, return false if it is outside the
        // primitive, otherwise set the hit record UV coordinates and return true.

        if (!unit_interval.contains(a) || !unit_interval.contains(b))
        {
            return false;
        }
        rec.u = a;
        rec.v = b;
        return true;
    }

private:
    vec3 Q;
    vec3 u, v;
    vec3 w;
    std::shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    float d;
};

inline std::shared_ptr<hittable_list> box(const vec3& a, const vec3& b, std::shared_ptr<material> mat)
{
    // Returns the 3D box (six sides) that contains the two opposite vertices a & b.

    std::shared_ptr<hittable_list> sides = std::make_shared<hittable_list>();

    // Construct the two opposite vertices with the minimum and maximum coordinates.
    vec3 min = vec3(std::fmin(a.x, b.x), std::fmin(a.y, b.y), std::fmin(a.z, b.z));
    vec3 max = vec3(std::fmax(a.x, b.x), std::fmax(a.y, b.y), std::fmax(a.z, b.z));

    vec3 dx = vec3(max.x - min.x, 0, 0);
    vec3 dy = vec3(0, max.y - min.y, 0);
    vec3 dz = vec3(0, 0, max.z - min.z);

    sides->add(std::make_shared<quad>(vec3(min.x, min.y, max.z), dx, dy, mat)); // front
    sides->add(std::make_shared<quad>(vec3(max.x, min.y, max.z), -dz, dy, mat)); // right
    sides->add(std::make_shared<quad>(vec3(max.x, min.y, min.z), -dx, dy, mat)); // back
    sides->add(std::make_shared<quad>(vec3(min.x, min.y, min.z), dz, dy, mat)); // left
    sides->add(std::make_shared<quad>(vec3(min.x, max.y, max.z), dx, -dz, mat)); // top
    sides->add(std::make_shared<quad>(vec3(min.x, min.y, min.z), dx, dz, mat)); // bottom

    return sides;
}