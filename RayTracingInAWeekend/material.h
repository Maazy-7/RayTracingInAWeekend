#pragma once

#include "hittable.h"
#include "onb.h"
#include "pdf.h"
#include "texture.h"

class scatter_record 
{
public:
    vec3 attenuation;
    std::shared_ptr<pdf> pdf_ptr;
    bool skip_pdf;
    ray skip_pdf_ray;
};

class material 
{
public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const 
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

    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override 
    {
        srec.attenuation = tex->value(rec.u, rec.v, rec.p);
        srec.pdf_ptr = std::make_shared<cosine_pdf>(rec.normal);
        srec.skip_pdf = false;
        return true;
    }

    float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override 
    {
        float cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
        return cos_theta < 0 ? 0 : cos_theta / pi;
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

    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override 
    {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + (fuzziness * random_unit_vector());

        srec.attenuation = albedo;
        srec.pdf_ptr = nullptr;
        srec.skip_pdf = true;
        srec.skip_pdf_ray = ray(rec.p, reflected, r_in.time());

        return true;
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

    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override 
    {
        srec.attenuation = vec3(1.0, 1.0, 1.0);
        srec.pdf_ptr = nullptr;
        srec.skip_pdf = true;
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

        srec.skip_pdf_ray = ray(rec.p, direction, r_in.time());
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

    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {

        srec.attenuation = tex->value(rec.u, rec.v, rec.p);
        srec.pdf_ptr = std::make_shared<sphere_pdf>();
        srec.skip_pdf = false;
        return true;
    }

    float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override 
    {
        return 1 / (4 * pi);
    }

private:
    std::shared_ptr<texture> tex;
};