    #include <iostream>
    #include <memory>
    #include <chrono>

    #include "bvh_node.h"
    #include "camera.h"
    #include "hittable.h"
    #include "hittable_list.h"
    #include "material.h"
    #include "sphere.h"


    int main() 
    {
        hittable_list world;

        std::shared_ptr<lambertian> ground_material = std::make_shared<lambertian>(vec3(0.5f, 0.5f, 0.5f));
        world.add(std::make_shared<sphere>(vec3(0, -1000, 0), 1000.f, ground_material));

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
                        vec3 albedo = vec3::random() * vec3::random();
                        sphere_material = std::make_shared<lambertian>(albedo);
                        world.add(std::make_shared<sphere>(center,0.2f, sphere_material));
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
        world.add(std::make_shared<sphere>(vec3(0, 1, 0), 1.0f, material1));

        std::shared_ptr<lambertian> material2 = std::make_shared<lambertian>(vec3(0.4f, 0.2f, 0.1f));
        world.add(std::make_shared<sphere>(vec3(-4, 1, 0), 1.0f, material2));

        std::shared_ptr<metal> material3 = std::make_shared<metal>(vec3(0.7f, 0.6f, 0.5f), 0.0f);
        world.add(std::make_shared<sphere>(vec3(4, 1, 0), 1.0f, material3));

        world = hittable_list(std::make_shared<bvh_node>(world));

        camera cam;

        cam.aspect_ratio = 16.0f / 9.0f;
        cam.image_width = 1000;
        cam.samples_per_pixel = 100;
        cam.max_depth = 50;

        cam.vfov = 20;
        cam.lookfrom = vec3(13, 3, 3);
        cam.lookat = vec3(0, 0, 0);
        cam.vup = vec3(0, 1, 0);

        cam.defocus_angle = 0.6f;
        cam.focus_dist = 10.0f;

        cam.render(world);
    }