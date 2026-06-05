#pragma once

#include "hittable_list.h"
#include "onb.h"

class pdf 
{
public:
    virtual ~pdf() {}

    virtual float value(const vec3& direction) const = 0;
    virtual vec3 generate() const = 0;
};

class sphere_pdf : public pdf 
{
public:
    sphere_pdf() {}

    float value(const vec3& direction) const override 
    {
        return 1 / (4 * pi);
    }

    vec3 generate() const override 
    {
        return random_unit_vector();
    }
};

class cosine_pdf : public pdf 
{
public:
    cosine_pdf(const vec3& w) : uvw(w) {}

    float value(const vec3& direction) const override 
    {
        float cosine_theta = dot(unit_vector(direction), uvw.w());
        return std::fmaxf(0, cosine_theta / pi);
    }

    vec3 generate() const override 
    {
        return uvw.transform(random_cosine_direction());
    }

private:
    onb uvw;
};

class hittable_pdf : public pdf 
{
public:
    hittable_pdf(const hittable& objects, const vec3& origin)
        : objects(objects), origin(origin)
    {

    }

    float value(const vec3& direction) const override 
    {
        return objects.pdf_value(origin, direction);
    }

    vec3 generate() const override 
    {
        return objects.random(origin);
    }

private:
    const hittable& objects;
    vec3 origin;
};

class mixture_pdf : public pdf 
{
public:
    mixture_pdf(std::shared_ptr<pdf> p0, std::shared_ptr<pdf> p1) 
    {
        p[0] = p0;
        p[1] = p1;
    }

    float value(const vec3& direction) const override 
    {
        return 0.5f * p[0]->value(direction) + 0.5f * p[1]->value(direction);
    }

    vec3 generate() const override 
    {
        if (random_float() < 0.5f)
        {
            return p[0]->generate();
        }
        else
        {
            return p[1]->generate();
        }
    }

private:
    std::shared_ptr<pdf> p[2];
};