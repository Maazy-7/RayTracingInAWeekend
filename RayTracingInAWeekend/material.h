#pragma once

#include "hittable.h"
#include "onb.h"
#include "texture.h"

class material 
{
public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, float& pdf) const 
    {
        return false;
    }

    virtual vec3 emitted(const ray& r_in, const hit_record& rec, float u, float v, const vec3& p) const
    {
        return vec3(0, 0, 0);
    }

    virtual float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const 
    {
        return 0;
    }
};

class lambertian : public material 
{
public:
    lambertian(const vec3& albedo) 
        : tex(std::make_shared<solid_color>(albedo)) 
    {
    
    }

    lambertian(std::shared_ptr<texture> tex) 
        : tex(tex) 
    {
    
    }

    bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, float& pdf) const override
    {
        //vec3 scatter_direction = random_on_hemisphere(rec.normal); //rec.normal + random_unit_vector();
        
        onb uvw(rec.normal);
        vec3 scatter_direction = uvw.transform(random_cosine_direction());
        scattered = ray(rec.p, unit_vector(scatter_direction), r_in.time());
        // Catch degenerate scatter direction
        if (scatter_direction.near_zero()) { scatter_direction = rec.normal; }
        scattered = ray(rec.p, scatter_direction, r_in.time());
        attenuation = tex->value(rec.u, rec.v, rec.p);
        pdf = dot(uvw.w(), scattered.direction()) / pi;
        return true;
    }

    float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override 
    {
        /*float cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
        return cos_theta < 0 ? 0 : cos_theta / pi;*/
        return 1.f / (2.f * pi);
    }

private:
    std::shared_ptr<texture> tex;
};

class metal : public material 
{
public:
    metal(const vec3& albedo, float fuzz) 
        : albedo(albedo), fuzziness(fuzz < 1 ? fuzz : 1)
    {
    
    }

    bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, float& pdf) const override
    {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + (fuzziness * random_unit_vector());
        scattered = ray(rec.p, reflected, r_in.time());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

private:
    vec3 albedo;
    float fuzziness;
};

class dielectric : public material 
{
public:
    dielectric(float refraction_index) 
        : refraction_index(refraction_index) 
    {
    
    }

    bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, float& pdf) const override
    {
        attenuation = vec3(1.0f, 1.0f, 1.0f);
        float ri = rec.front_face ? (1.0f / refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        float cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0f);
        float sin_theta = std::sqrt(1.0f - cos_theta * cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0f;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_float())
        {
            direction = reflect(unit_direction, rec.normal);
        }
        else
        {
            direction = refract(unit_direction, rec.normal, ri);
        }

        scattered = ray(rec.p, direction, r_in.time());
        return true;
    }

private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    float refraction_index;

    static float reflectance(float cosine, float refraction_index) 
    {
        // Use Schlick's approximation for reflectance.
        float r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1.f - r0) * std::powf((1.f - cosine), 5);
    }
};

class diffuse_light : public material 
{
public:
    diffuse_light(std::shared_ptr<texture> tex) 
        : tex(tex) 
    {
    
    }
    diffuse_light(const vec3& emit) 
        : tex(std::make_shared<solid_color>(emit)) 
    {
    
    }

    vec3 emitted(const ray& r_in, const hit_record& rec, float u, float v, const vec3& p) const override
    {
        if (!rec.front_face) 
        {
            return vec3(0, 0, 0);
        }
        return tex->value(u, v, p);
    }

private:
    std::shared_ptr<texture> tex;
};

class isotropic : public material 
{
public:
    isotropic(const vec3& albedo) 
        : tex(std::make_shared<solid_color>(albedo)) 
    {
    
    }

    isotropic(std::shared_ptr<texture> tex) 
        : tex(tex) 
    {
    
    }

    bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, float& pdf) const override
    {
        scattered = ray(rec.p, random_unit_vector(), r_in.time());
        attenuation = tex->value(rec.u, rec.v, rec.p);
        pdf = 1 / (4 * pi);
        return true;
    }

    float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override 
    {
        return 1 / (4 * pi);
    }

private:
    std::shared_ptr<texture> tex;
};