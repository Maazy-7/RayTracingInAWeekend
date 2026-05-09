#pragma once

#include <iostream>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <random>

const double pi = 3.1415926535897932385;
const double infinity = std::numeric_limits<double>::infinity();


inline double degrees_to_radians(double degrees) 
{
    return degrees * pi / 180.0;
}

inline double radians_to_degrees(double radians) 
{
    return radians * 180.0 / pi;
}

inline double random_double() 
{
    // Returns a random real in [0,1).
    thread_local static std::mt19937 generator(std::random_device{}());
    thread_local static std::uniform_real_distribution<double>distribution(0.0, 1.0);
    return distribution(generator);
}

inline double random_double(double min, double max) 
{
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

inline int random_int(int min, int max)
{
    // Returns a random integer in [min,max].
    return int(random_double(min, max + 1));
}