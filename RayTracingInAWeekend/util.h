#pragma once

#include <iostream>
#include <cmath>
#include <limits>

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