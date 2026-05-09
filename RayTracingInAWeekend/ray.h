#pragma once

#include "vec3.h"

class ray 
{
public:
    ray() {}

    ray(const vec3& origin, const vec3& direction, float time)
        : orig(origin), dir(direction), tm(time)
    {

    }

    ray(const vec3& origin, const vec3& direction) 
        : orig(origin), dir(direction), tm(0)
    {
    
    }

    const vec3& origin() const { return orig; }
    const vec3& direction() const { return dir; }
    float time() const { return tm; }

    vec3 at(float t) const 
    {
        return orig + t * dir;
    }

private:
    vec3 orig;
    vec3 dir;
    float tm;
};