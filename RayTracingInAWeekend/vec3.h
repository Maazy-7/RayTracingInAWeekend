#pragma once

#include <iostream>
#include <cmath>

#include "util.h"

class vec3 
{
public:
    double x, y, z;

    vec3() 
        : x(0), y(0), z(0)
    {
    
    }
    vec3(double v) 
        : x(v), y(v), z(v)
    {
    
    }
    vec3(double vx, double vy, double vz) 
        : x(vx), y(vy), z(vz)
    {
    
    }

    vec3 operator-() const { return vec3(-x, -y, -z); }

    vec3& operator+=(const vec3& v) 
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    vec3& operator*=(double t) 
    {
        x *= t;
        y *= t;
        z *= t;
        return *this;
    }

    vec3& operator/=(double t) 
    {
        return *this *= 1 / t;
    }

    double length() const 
    {
        return std::sqrt(length_squared());
    }

    double length_squared() const 
    {
        return x * x + y * y + z * z;
    }

    static vec3 random() 
    {
        return vec3(random_double(), random_double(), random_double());
    }

    static vec3 random(double min, double max) 
    {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
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

inline vec3 operator*(double t, const vec3& v) 
{
    return vec3(t * v.x, t * v.y, t * v.z);
}

inline vec3 operator*(const vec3& v, double t) 
{
    return t * v;
}

inline vec3 operator/(const vec3& v, double t) 
{
    return (1 / t) * v;
}

inline double dot(const vec3& u, const vec3& v) 
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
        double lensq = p.length_squared();
        if (1e-160 < lensq && lensq <= 1) { return p / sqrt(lensq); }
    }
}

inline vec3 random_on_hemisphere(const vec3& normal) 
{
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
    {
        return on_unit_sphere;
    }
    else
    {
        return -on_unit_sphere;
    }
    //return dot(on_unit_sphere, normal) > 0.0 ? on_unit_sphere : -on_unit_sphere;
}