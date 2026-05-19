#include <iostream>
#include <memory>
#include <chrono>

#include "bvh_node.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "quad.h"
#include "sphere.h"
#include "texture.h"


    int main() 
    {
        hittable_list world;

        int scene_num = 1;
        if (scene_num == 0)
        {

            std::shared_ptr<noise_texture> noise_tex = std::make_shared<noise_texture>(10.f);
            world.add(std::make_shared<sphere>(vec3(0, -1000, 0), 1000.f, std::make_shared<lambertian>(noise_tex/*vec3(0.5f, 0.5f, 0.5f)*/)));

            for (int a = -4; a < 4; a++)
            {
                for (int b = -4; b < 4; b++)
                {
                    float choose_mat = random_float();
                    vec3 center(a + 0.9f * random_float(), 0.2f, b + 0.9f * random_float());

                    if ((center - vec3(4, 0.2f, 0)).length() > 0.9f)
                    {
                        std::shared_ptr<material> sphere_material;

                        if (choose_mat < 0.8f)
                        {
                            // diffuse
                            int ran = random_int(0, 1);
                            vec3 albedo = vec3::random() * vec3::random();
                            sphere_material = (ran) ? std::make_shared<lambertian>(albedo) : std::make_shared<lambertian>(noise_tex);
                            world.add(std::make_shared<sphere>(center, 0.2f, sphere_material));
                        }
                        else if (choose_mat < 0.95f)
                        {
                            // metal
                            vec3 albedo = vec3::random(0.5f, 1);
                            float fuzz = random_float(0, 0.5f);
                            sphere_material = std::make_shared<metal>(albedo, fuzz);
                            world.add(std::make_shared<sphere>(center, 0.2f, sphere_material));
                        }
                        else
                        {
                            // glass
                            sphere_material = std::make_shared<dielectric>(1.5f);
                            world.add(std::make_shared<sphere>(center, 0.2f, sphere_material));
                        }
                    }
                }
            }

            std::shared_ptr<dielectric> material1 = std::make_shared<dielectric>(1.5f);
            world.add(std::make_shared<sphere>(vec3(0, 1, 0), 1.0f, std::make_shared<lambertian>(noise_tex)));

            std::shared_ptr<lambertian> material2 = std::make_shared<lambertian>(vec3(0.4f, 0.2f, 0.1f));
            world.add(std::make_shared<sphere>(vec3(-4, 1, 0), 1.0f, material2));

            std::shared_ptr<metal> material3 = std::make_shared<metal>(vec3(0.7f, 0.6f, 0.5f), 0.0f);
            world.add(std::make_shared<sphere>(vec3(4, 1, 0), 1.0f, material3));

            world = hittable_list(std::make_shared<bvh_node>(world));

            camera cam;

            cam.aspect_ratio = 16.0f / 9.0f;
            cam.image_width = 600;
            cam.samples_per_pixel = 100;
            cam.max_depth = 40;

            cam.vfov = 20;
            cam.lookfrom = vec3(13, 3, 3);
            cam.lookat = vec3(0, 0, 0);
            cam.vup = vec3(0, 1, 0);

            cam.defocus_angle = 0.6f;
            cam.focus_dist = 10.0f;

            cam.render(world);
        }
        else if (scene_num == 1) 
        {
            // Materials
            std::shared_ptr<lambertian> left_red = std::make_shared<lambertian>(vec3(1.0f, 0.2f, 0.2f));
            std::shared_ptr<lambertian> back_green = std::make_shared<lambertian>(vec3(0.2f, 1.0f, 0.2f));
            std::shared_ptr<lambertian> right_blue = std::make_shared<lambertian>(vec3(0.2f, 0.2f, 1.0f));
            std::shared_ptr<lambertian> upper_orange = std::make_shared<lambertian>(vec3(1.0f, 0.5f, 0.0f));
            std::shared_ptr<lambertian> lower_teal = std::make_shared<lambertian>(vec3(0.2f, 0.8f, 0.8f));

            // Quads
            world.add(std::make_shared<quad>(vec3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), left_red));
            world.add(std::make_shared<quad>(vec3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
            world.add(std::make_shared<quad>(vec3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
            world.add(std::make_shared<quad>(vec3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
            world.add(std::make_shared<quad>(vec3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4), lower_teal));

            camera cam;

            cam.aspect_ratio = 1.0f;
            cam.image_width = 400;
            cam.samples_per_pixel = 100;
            cam.max_depth = 50;

            cam.vfov = 80;
            cam.lookfrom = vec3(0, 0, 9);
            cam.lookat = vec3(0, 0, 0);
            cam.vup = vec3(0, 1, 0);

            cam.defocus_angle = 0;

            cam.render(world);
        }
    }