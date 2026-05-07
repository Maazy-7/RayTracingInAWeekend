    #include <iostream>
    #include <memory>
    #include <chrono>

    #include "camera.h"
    #include "hittable.h"
    #include "hittable_list.h"
    #include "material.h"
    #include "sphere.h"


    int main() 
    {
        hittable_list world;

        std::shared_ptr<lambertian> ground_material = std::make_shared<lambertian>(vec3(0.5, 0.5, 0.5));
        world.add(std::make_shared<sphere>(vec3(0, -1000, 0), 1000, ground_material));

        for (int a = -2; a < 2; a++) 
        {
            for (int b = -2; b < 2; b++) 
            {
                double choose_mat = random_double();
                vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

                if ((center - vec3(4, 0.2, 0)).length() > 0.9) 
                {
                    std::shared_ptr<material> sphere_material;

                    if (choose_mat < 0.8) 
                    {
                        // diffuse
                        vec3 albedo = vec3::random() * vec3::random();
                        sphere_material = std::make_shared<lambertian>(albedo);
                        world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                    }
                    else if (choose_mat < 0.95) 
                    {
                        // metal
                        vec3 albedo = vec3::random(0.5, 1);
                        double fuzz = random_double(0, 0.5);
                        sphere_material = std::make_shared<metal>(albedo, fuzz);
                        world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                    }
                    else 
                    {
                        // glass
                        sphere_material = std::make_shared<dielectric>(1.5);
                        world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                    }
                }
            }
        }

        std::shared_ptr<dielectric> material1 = std::make_shared<dielectric>(1.5);
        world.add(std::make_shared<sphere>(vec3(0, 1, 0), 1.0, material1));

        std::shared_ptr<lambertian> material2 = std::make_shared<lambertian>(vec3(0.4, 0.2, 0.1));
        world.add(std::make_shared<sphere>(vec3(-4, 1, 0), 1.0, material2));

        std::shared_ptr<metal> material3 = std::make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0);
        world.add(std::make_shared<sphere>(vec3(4, 1, 0), 1.0, material3));

        camera cam;

        cam.aspect_ratio = 16.0 / 9.0;
        cam.image_width = 600;
        cam.samples_per_pixel = 70;
        cam.max_depth = 10;

        cam.vfov = 20;
        cam.lookfrom = vec3(13, 2, 3);
        cam.lookat = vec3(0, 0, 0);
        cam.vup = vec3(0, 1, 0);

        cam.defocus_angle = 0.6;
        cam.focus_dist = 10.0;

        cam.render(world);
    }