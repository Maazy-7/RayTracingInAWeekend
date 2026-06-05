#pragma once

#include "ray.h"
#include "interval.h"
#include "aabb.h"

class material;

class hit_record 
{
public:
    vec3 p;
    vec3 normal;
    std::shared_ptr<material> mat;
    float t;
    float u, v; //tecture coordinates
    bool front_face;

    void set_face_normal(const ray& r, const vec3& outward_normal) 
    {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable 
{
public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
    virtual aabb bounding_box() const = 0;

    virtual float pdf_value(const vec3& origin, const vec3& direction) const 
    {
        return 0.0f;
    }

    virtual vec3 random(const vec3& origin) const 
    {
        return vec3(1, 0, 0);
    }

};

class translate : public hittable 
{
public:
    translate(std::shared_ptr<hittable> object, const vec3& offset)
        : object(object), offset(offset)
    {
        bbox = object->bounding_box() + offset;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override 
    {
        // Move the ray backwards .y the offset
        ray offset_r(r.origin() - offset, r.direction(), r.time());

        // Determine whether an intersection exists along the offset ray (and if so, where)
        if (!object->hit(offset_r, ray_t, rec))
        {
            return false;
        }
        // Move the intersection vec forwards .y the offset
        rec.p += offset;

        return true;
    }

    aabb bounding_box() const override { return bbox; }

private:
    std::shared_ptr<hittable> object;
    vec3 offset;
    aabb bbox;
};

class rotate_y : public hittable {
public:

    rotate_y(std::shared_ptr<hittable> object, float angle) : object(object) 
    {
        float radians = degrees_to_radians(angle);
        sin_theta = std::sinf(radians);
        cos_theta = std::cosf(radians);
        bbox = object->bounding_box();

        vec3 min(infinity, infinity, infinity);
        vec3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) 
        {
            for (int j = 0; j < 2; j++) 
            {
                for (int k = 0; k < 2; k++) 
                {
                    float x = i * bbox.x.max + (1 - i) * bbox.x.min;
                    float y = j * bbox.y.max + (1 - j) * bbox.y.min;
                    float z = k * bbox.z.max + (1 - k) * bbox.z.min;
                    float newx = cos_theta * x + sin_theta * z;
                    float newz = -sin_theta * x + cos_theta * z;

                    vec3 tester(newx, y, newz);

                    min.x = std::fmin(min.x, tester.x);
                    max.x = std::fmax(min.x, tester.x);

                    min.y = std::fmin(min.y, tester.y);
                    max.y = std::fmax(min.y, tester.y);

                    min.z = std::fmin(min.z, tester.z);
                    max.z = std::fmax(min.z, tester.z);
                }
            }
        }

        bbox = aabb(min, max);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override 
    {

        // Transform the ray from world space to object space.

        vec3 origin = vec3(
            (cos_theta * r.origin().x) - (sin_theta * r.origin().z),
            r.origin().y,
            (sin_theta * r.origin().x) + (cos_theta * r.origin().z)
        );

        vec3 direction = vec3(
            (cos_theta * r.direction().x) - (sin_theta * r.direction().z),
            r.direction().y,
            (sin_theta * r.direction().x) + (cos_theta * r.direction().z)
        );

        ray rotated_r(origin, direction, r.time());

        // Determine whether an intersection exists in object space (and if so, where).

        if (!object->hit(rotated_r, ray_t, rec))
        {
            return false;
        }

        // Transform the intersection from object space back to world space.

        rec.p = vec3(
            (cos_theta * rec.p.x) + (sin_theta * rec.p.z),
            rec.p.y,
            (-sin_theta * rec.p.x) + (cos_theta * rec.p.z)
        );

        rec.normal = vec3(
            (cos_theta * rec.normal.x) + (sin_theta * rec.normal.z),
            rec.normal.y,
            (-sin_theta * rec.normal.x) + (cos_theta * rec.normal.z)
        );

        return true;
    }

    aabb bounding_box() const override { return bbox; }

private:
    std::shared_ptr<hittable> object;
    float sin_theta;
    float cos_theta;
    aabb bbox;
};