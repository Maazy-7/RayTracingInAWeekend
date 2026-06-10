#pragma once

#include "hittable.h"

#include <memory>
#include <vector>


class hittable_list : public hittable 
{
public:
    std::vector<std::shared_ptr<hittable>> hittable_objects;

    hittable_list() {}
    hittable_list(std::shared_ptr<hittable> object) { add(object); }

    void clear() { hittable_objects.clear(); }

    void add(std::shared_ptr<hittable> object) 
    {
        hittable_objects.push_back(object);
        bbox = aabb(bbox, object->bounding_box());
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override 
    {
        hit_record temp_rec;
        bool hit_anything = false;
        float closest_so_far = ray_t.max;

        for (const auto& object : hittable_objects) 
        {
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec))
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    aabb bounding_box() const override { return bbox; }

    float pdf_value(const vec3& origin, const vec3& direction) const override 
    {
        float weight = 1.0f / hittable_objects .size();
        float sum = 0.0f;

        for (const auto& object : hittable_objects)
        {
            sum += weight * object->pdf_value(origin, direction);
        }
        return sum;
    }

    vec3 random(const vec3& origin) const override 
    {
        auto int_size = int(hittable_objects.size());
        return hittable_objects[random_int(0, int_size - 1)]->random(origin);
    }

private:
    aabb bbox;
};