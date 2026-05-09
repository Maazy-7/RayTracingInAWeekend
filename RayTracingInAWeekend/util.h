#pragma once

#include <iostream>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <random>

const float pi = 3.141593f;
const float infinity = std::numeric_limits<float>::infinity();


inline float degrees_to_radians(float degrees) 
{
    return degrees * pi / 180.0f;
}

inline float radians_to_degrees(float radians) 
{
    return radians * 180.0f / pi;
}

inline float random_float() 
{
    // Returns a random real in [0,1).
    thread_local static std::mt19937 generator(std::random_device{}());
    thread_local static std::uniform_real_distribution<float>distribution(0.0f, 1.0f);
    return distribution(generator);
}

inline float random_float(float min, float max) 
{
    // Returns a random real in [min,max).
    return min + (max - min) * random_float();
}


inline int random_int(int min, int max)
{
    // Returns a random integer in [min,max].
    return int(random_float((float)min, (float)max + 1));
}