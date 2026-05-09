#pragma once

#include "vec3.h"

#include <iostream>


inline float linear_to_gamma(float linear_component)
{
    if (linear_component > 0) { return std::sqrt(linear_component); }

    return 0;
}

void write_color(std::ostream& out, const vec3& pixel_color)
{
    float r = pixel_color.x;
    float g = pixel_color.y;
    float b = pixel_color.z;

    // Apply a linear to gamma transform for gamma 2
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Translate the [0,1] component values to the byte range [0,255].
    static const interval intensity(0.000f, 0.999f);
    
    unsigned char color[3]
    {
        static_cast<unsigned char>(256 * intensity.clamp(r)),
        static_cast<unsigned char>(256 * intensity.clamp(g)),
        static_cast<unsigned char>(256 * intensity.clamp(b))
    };
    
    out.write(reinterpret_cast<char*>(color), 3);
}