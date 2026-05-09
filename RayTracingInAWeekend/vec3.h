#pragma once

#include <iostream>
#include <cmath>

#include "util.h"

class vec3 
{
public:
    float x, y, z;

    vec3() 
        : x(0), y(0), z(0)
    {
    
    }
    vec3(float v) 
        : x(v), y(v), z(v)
    {
    
    }
    vec3(float vx, float vy, float vz) 
        : x(vx), y(vy), z(vz)
    {
    
    }

    const float& operator[](int i) const
    {
        return *(&x + i);
    }

    vec3 operator-() const { return vec3(-x, -y, -z); }

    vec3& operator+=(const vec3& v) 
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    vec3& operator*=(float t) 
    {
        x *= t;
        y *= t;
        z *= t;
        return *this;
    }

    vec3& operator/=(float t) 
    {
        return *this *= 1 / t;
    }

    float length() const 
    {
        return std::sqrt(length_squared());
    }

    float length_squared() const 
    {
        return x * x + y * y + z * z;
    }

    bool near_zero() const 
    {
        // Return true if the vector is close to zero in all dimensions.
        auto s = 1e-8;
        return (std::fabs(x) < s) && (std::fabs(y) < s) && (std::fabs(z) < s);
    }

    static vec3 random() 
    {
        return vec3(random_float(), random_float(), random_float());
    }

    static vec3 random(float min, float max) 
    {
        return vec3(random_float(min, max), random_float(min, max), random_float(min, max));
    }
};

// Vector Utility Functions

inline std::ostream& operator<<(std::ostream& out, const vec3& v) 
{
    return out << v.x << ' ' << v.y << ' ' << v.z;
}

inline vec3 operator+(const vec3& u, const vec3& v) 
{
    return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline vec3 operator-(const vec3& u, const vec3& v) 
{
    return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

inline vec3 operator*(const vec3& u, const vec3& v) 
{
    return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}

inline vec3 operator*(float t, const vec3& v) 
{
    return vec3(t * v.x, t * v.y, t * v.z);
}

inline vec3 operator*(const vec3& v, float t) 
{
    return t * v;
}

inline vec3 operator/(const vec3& v, float t) 
{
    return (1 / t) * v;
}

inline float dot(const vec3& u, const vec3& v) 
{
    return u.x * v.x
        + u.y * v.y
        + u.z * v.z;
}

inline vec3 cross(const vec3& u, const vec3& v) 
{
    return vec3(u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x);
}

inline vec3 unit_vector(const vec3& v) 
{
    return v / v.length();
}

inline vec3 random_unit_vector() 
{
    while (true) 
    {
        vec3 p = vec3::random(-1, 1);
        float lensq = p.length_squared();
        if (1e-160 < lensq && lensq <= 1) { return p / sqrt(lensq); }
    }
}

inline vec3 random_on_hemisphere(const vec3& normal) 
{
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0f) // In the same hemisphere as the normal
    {
        return on_unit_sphere;
    }
    else
    {
        return -on_unit_sphere;
    }
    //return dot(on_unit_sphere, normal) > 0.0f ? on_unit_sphere : -on_unit_sphere;
}

inline vec3 random_in_unit_disk() 
{
    while (true) 
    {
        vec3 p = vec3(random_float(-1, 1), random_float(-1, 1), 0);
        if (p.length_squared() < 1) { return p; }
    }
}

inline vec3 reflect(const vec3& v, const vec3& n) 
{
    return v - 2 * dot(v, n) * n;
}

inline vec3 refract(const vec3& uv, const vec3& n, float etai_over_etat) 
{
    float cos_theta = std::fmin(dot(-uv, n), 1.0f);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -std::sqrt(std::fabs(1.0f - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}