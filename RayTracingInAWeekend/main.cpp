#include <iostream>
#include <memory>
#include <chrono>

#include "bvh_node.h"
#include "camera.h"
#include "hittable.h"
#include "constant_medium.h"
#include "hittable_list.h"
#include "material.h"
#include "quad.h"
#include "sphere.h"
#include "texture.h"


void final_scene(int image_width, int samples_per_pixel, int max_depth);

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
            cam.background_color = vec3(0.70f, 0.80f, 1.00f);

            cam.vfov = 20;
            cam.lookfrom = vec3(13, 3, 3);
            cam.lookat = vec3(0, 0, 0);
            cam.vup = vec3(0, 1, 0);

            cam.defocus_angle = 0.6f;
            cam.focus_dist = 10.0f;

            cam.render(world, world);
        }
    else if (scene_num == 1) 
    {
        std::shared_ptr<lambertian> red =   std::make_shared<lambertian>(vec3(.65f, .05f, .05f));
        std::shared_ptr<lambertian> white = std::make_shared<lambertian>(vec3(.73f, .73f, .73f));
        std::shared_ptr<lambertian> green = std::make_shared<lambertian>(vec3(.12f, .45f, .15f));
        std::shared_ptr<diffuse_light> light = std::make_shared<diffuse_light>(vec3(7.f, 7.f, 7.f));

        world.add(std::make_shared<quad>(vec3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
        world.add(std::make_shared<quad>(vec3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
        world.add(std::make_shared<quad>(vec3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));
        world.add(std::make_shared<quad>(vec3(0, 555, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
        world.add(std::make_shared<quad>(vec3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
        world.add(std::make_shared<quad>(vec3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

        std::shared_ptr<hittable> box1 = box(vec3(0, 0, 0), vec3(165, 330, 165), white);
        box1 = std::make_shared<rotate_y>(box1, 15.f);
        box1 = std::make_shared<translate>(box1, vec3(265, 0, 295));
        world.add(box1);

        std::shared_ptr<hittable> box2 = box(vec3(0, 0, 0), vec3(165, 165, 165), white);
        box2 = std::make_shared<rotate_y>(box2, -18.f);
        box2 = std::make_shared<translate>(box2, vec3(130, 0, 65));
        world.add(box2);

        // Light Sources
        auto empty_material = std::shared_ptr<material>();
        quad lights(vec3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), empty_material);

        /*world.add(std::make_shared<constant_medium>(box1, 0.01f, vec3(0, 0, 0)));
        world.add(std::make_shared<constant_medium>(box2, 0.01f, vec3(1, 1, 1)));*/

        camera cam;

        cam.aspect_ratio = 1.0f;
        cam.image_width = 600;
        cam.samples_per_pixel = 500;
        cam.max_depth = 50;
        cam.background_color = vec3(0, 0, 0);

        cam.vfov = 40;
        cam.lookfrom = vec3(278, 278, -800);
        cam.lookat = vec3(278, 278, 0);
        cam.vup = vec3(0, 1, 0);

        cam.defocus_angle = 0;

        cam.render(world,lights);
    }
    else if (scene_num == 2) 
    {
        final_scene(600, 4000, 40);
    }
}

void final_scene(int image_width, int samples_per_pixel, int max_depth) 
{
    hittable_list boxes1;
    auto ground = std::make_shared<lambertian>(vec3(0.48f, 0.83f, 0.53f));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) 
    {
        for (int j = 0; j < boxes_per_side; j++) 
        {
            float w = 100.0f;
            float x0 = -1000.0f + i * w;
            float z0 = -1000.0f + j * w;
            float y0 = 0.0f;
            float x1 = x0 + w;
            float y1 = random_float(1, 101);
            float z1 = z0 + w;

            boxes1.add(box(vec3(x0, y0, z0), vec3(x1, y1, z1), ground));
        }
    }

    hittable_list world;

    world.add(std::make_shared<bvh_node>(boxes1));

    std::shared_ptr<diffuse_light> light = std::make_shared<diffuse_light>(vec3(7, 7, 7));
    world.add(std::make_shared<quad>(vec3(123, 554, 147), vec3(300, 0, 0), vec3(0, 0, 265), light));

    vec3 center1 = vec3(400, 400, 200);
    vec3 center2 = center1 + vec3(30, 0, 0);
    std::shared_ptr<lambertian> sphere_material = std::make_shared<lambertian>(vec3(0.7f, 0.3f, 0.1f));
    world.add(std::make_shared<sphere>(center1, center2, 50.f, sphere_material));

    world.add(std::make_shared<sphere>(vec3(260, 150, 45), 50.f, std::make_shared<dielectric>(1.5f)));
    world.add(std::make_shared<sphere>(
        vec3(0, 150, 145), 50.f, std::make_shared<metal>(vec3(0.8f, 0.8f, 0.9f), 1.0f)
    ));

    std::shared_ptr<sphere> boundary = std::make_shared<sphere>(vec3(360, 150, 145), 70.f, std::make_shared<dielectric>(1.5f));
    world.add(boundary);
    world.add(std::make_shared<constant_medium>(boundary, 0.2f, vec3(0.2f, 0.4f, 0.9f)));
    boundary = std::make_shared<sphere>(vec3(0, 0, 0), 5000.f, std::make_shared<dielectric>(1.5f));
    world.add(std::make_shared<constant_medium>(boundary, 0.0001f, vec3(1, 1, 1)));

    std::shared_ptr<lambertian> emat = std::make_shared<lambertian>(std::make_shared<image_texture>("earthmap.jpg"));
    world.add(std::make_shared<sphere>(vec3(400, 200, 400), 100.f, emat));
    std::shared_ptr<noise_texture> pertext = std::make_shared<noise_texture>(0.2f);
    world.add(std::make_shared<sphere>(vec3(220, 280, 300), 80.f, std::make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    std::shared_ptr<lambertian> white = std::make_shared<lambertian>(vec3(0.73f, 0.73f, 0.73f));
    int ns = 1000;
    for (int j = 0; j < ns; j++) 
    {
        boxes2.add(std::make_shared<sphere>(vec3::random(0, 165), 10.f, white));
    }

    world.add(std::make_shared<translate>(
        std::make_shared<rotate_y>(
        std::make_shared<bvh_node>(boxes2), 15.f),
        vec3(-100, 270, 395)));

    camera cam;

    cam.aspect_ratio = 1.0f;
    cam.image_width = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth = max_depth;
    cam.background_color = vec3(0, 0, 0);

    cam.vfov = 40;
    cam.lookfrom = vec3(478, 278, -600);
    cam.lookat = vec3(278, 278, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world, world);
}