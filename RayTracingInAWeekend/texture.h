#pragma once

#include "vec3.h"
#include "image.h"

class texture 
{
public:
    virtual ~texture() = default;

    virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class solid_color : public texture 
{
public:
    solid_color(const vec3& albedo) 
        : albedo(albedo) 
    {
    
    }

    solid_color(float red, float green, float blue) 
        : solid_color(vec3(red, green, blue)) 
    {
    
    }

    vec3 value(float u, float v, const vec3& p) const override 
    {
        return albedo;
    }

private:
    vec3 albedo;
};

class checker_texture : public texture {
public:
    checker_texture(float scale, std::shared_ptr<texture> even, std::shared_ptr<texture> odd)
        : inv_scale(1.0f / scale), even(even), odd(odd) 
    {

    }

    checker_texture(float scale, const vec3& c1, const vec3& c2)
        : checker_texture(scale, std::make_shared<solid_color>(c1), std::make_shared<solid_color>(c2)) 
    {

    }

    vec3 value(float u, float v, const vec3& p) const override 
    {
        int xInteger = int(std::floor(inv_scale * p.x));
        int yInteger = int(std::floor(inv_scale * p.y));
        int zInteger = int(std::floor(inv_scale * p.z));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    float inv_scale;
    std::shared_ptr<texture> even;
    std::shared_ptr<texture> odd;
};

class image_texture : public texture
{
public:
    image_texture(const char* filename) 
        : image_tex(filename) 
    {
    
    }

   vec3 value(float u, float v, const vec3& p) const override
    {
        // If we have no texture data, then return solid cyan as a debugging aid.
       if (image_tex.height() <= 0) { return vec3(0, 1, 1); }

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = interval(0, 1).clamp(u);
        v = 1.0f - interval(0, 1).clamp(v);  // Flip V to image coordinates

        int i = int(u * image_tex.width());
        int j = int(v * image_tex.height());
        const uint8_t* pixel = image_tex.pixel_data(i, j);

        float color_scale = 1.0f / 255.0f;
        return vec3(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }

private:
    image image_tex;
};