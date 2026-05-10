#pragma once

#include <chrono>
#include <vector>
#include <thread>
#include <atomic>
#include <fstream>

#include "hittable.h"
#include "material.h"
#include "color.h"

struct tile 
{
    int x0, y0, x1, y1;
};

class camera 
{
public:
    /* Public Camera Parameters Here */

    //default values
    float aspect_ratio = 1.0f;  // Ratio of image width over height
    int    image_width = 100;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;   // Count of random samples for each pixel
    int    max_depth = 10;   // Maximum number of ray bounces into scene

    float vfov = 90;//vertical field of view
    vec3 lookfrom = vec3(0, 0, 0);   // Point camera is looking from
    vec3  lookat = vec3(0, 0, -1);  // Point camera is looking at
    vec3      vup = vec3(0, 1, 0); // Camera-relative "up" direction

    float defocus_angle = 0;  // Variation angle of rays through each pixel
    float focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus


    void render(const hittable& world) 
    {
        initialize();

        world_list = &world;

        std::ofstream out("image.ppm", std::ios::binary);

        out << "P6\n" << image_width << ' ' << image_height << "\n255\n";

#if 0
        auto start = std::chrono::high_resolution_clock::now();
        auto p0 = start;
        float averagePixelTime = 0.0f;

        for (int j = 0; j < image_height; j++) 
        {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; i++) 
            {
                p0 = std::chrono::high_resolution_clock::now();

                vec3 pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; sample++) 
                {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(std::cout, pixel_samples_scale * pixel_color);
                
                auto p1 = std::chrono::high_resolution_clock::now();
                std::chrono::duration<float> elapsedPixelTime = p1 - p0;
                averagePixelTime += elapsedPixelTime.count() / (image_height*image_width);
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = end - start;
        std::clog << "\rDone, Frame time: " << elapsed.count() << "s, Average pixel write time: " << averagePixelTime*1000 << "ms \n";

#elif 0

        auto start = std::chrono::high_resolution_clock::now();

        constexpr unsigned int thread_amount = 16 - 2;

        std::thread arr[thread_amount];

        for (int i = 0; i < thread_amount; i++) 
        {
            arr[i] = std::thread(&camera::render_thread, this, i, thread_amount);
        }

        for (int i = 0; i < thread_amount; i++) 
        {
            arr[i].join();
        }

        auto render_finish = std::chrono::high_resolution_clock::now();

        int scan_lines_remaining = image_height-1;
        for (int i = 0; i < framebuffer.size(); i++) 
        {
            if (i % image_width == 0) 
            {
                std::clog << "\rScanlines remaining: " << scan_lines_remaining-- << ' ' << std::flush;
            }
            write_color(out, framebuffer[i]);
        }

        auto write_finish = std::chrono::high_resolution_clock::now();

        std::chrono::duration<float> render_time = render_finish - start;
        std::chrono::duration<float> write_time = write_finish - render_finish;

        std::clog << "\rDone, render time: " << render_time.count() << "s, write time: " << write_time.count() << ", total " << (render_time.count()+write_time.count()) << "\n";

#elif 1
        constexpr unsigned int thread_amount = 16 - 2;
        std::thread arr[thread_amount];

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < thread_amount; i++) 
        {
            arr[i] = std::thread(&camera::render_tile_thread, this);
        }

        for (int i = 0; i < thread_amount; i++)
        {
            arr[i].join();
        }

        auto render_finish = std::chrono::high_resolution_clock::now();

        int scan_lines_remaining = image_height - 1;
        for (int i = 0; i < framebuffer.size(); i++)
        {
            if (i % image_width == 0)
            {
                std::clog << "\rScanlines remaining: " << scan_lines_remaining-- << ' ' << std::flush;
            }
            write_color(out, framebuffer[i]);
        }

        auto write_finish = std::chrono::high_resolution_clock::now();

        std::chrono::duration<float> render_time = render_finish - start;
        std::chrono::duration<float> write_time = write_finish - render_finish;

        std::clog << "\rDone, render time: " << render_time.count() << "s, write time: " << write_time.count() << ", total " << (render_time.count() + write_time.count()) << "\n";


#endif
    }

    

private:
    int    image_height;   // Rendered image height
    float pixel_samples_scale;  // Color scale factor for a sum of pixel samples
    vec3   center;         // Camera center
    vec3   pixel00_loc;    // Location of pixel 0, 0
    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below
    vec3   u, v, w;        // Camera frame basis vectors
    vec3   defocus_disk_u; // Defocus disk horizontal radius
    vec3   defocus_disk_v; // Defocus disk vertical radius
    std::vector<vec3> framebuffer; //framebuffer - vector that stores all pixels of the image
    std::vector<tile> screen_tiles; //list storing all tiles that make up the screen
    int tile_size = 16; //side length of tile size, tile area = tile size squared
    const hittable* world_list;
    std::atomic<int> scan_lines;
    std::atomic<int> tiles_rendered = -1;


    void initialize() 
    {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        scan_lines = image_height;
        framebuffer.resize(image_width*image_height);
        screen_tiles.reserve((image_width * image_height) / (tile_size*tile_size));
        create_tiles();

        pixel_samples_scale = 1.0f / samples_per_pixel;

        center = lookfrom;

        // Determine viewport dimensions.
        //float focal_length = (lookfrom - lookat).length();
        float theta = degrees_to_radians(vfov);
        float h = std::tan(theta / 2);
        float viewport_height = 2 * h * focus_dist;
        float viewport_width = viewport_height * (float(image_width) / image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vec3 viewport_u = viewport_width * u;
        vec3 viewport_v = viewport_height * -v;

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / (float)image_width;
        pixel_delta_v = viewport_v / (float)image_height;

        // Calculate the location of the upper left pixel.
        vec3 viewport_upper_left =
            center - focus_dist * w - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        float defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    void create_tiles() 
    {
        for (int i = 0; i < image_height; i+= tile_size) 
        {
            for (int j = 0; j < image_width; j+= tile_size) 
            {
                int x1 = std::min(j + tile_size, image_width);
                int y1 = std::min(i + tile_size, image_height);
                screen_tiles.push_back({j,i,x1, y1});
            }
        }
    }

    void render_tile_thread() 
    {
        while (true) 
        {
            int tile_index = tiles_rendered++;
            if (tile_index >= screen_tiles.size()) { break; }
            render_tile(screen_tiles[tile_index]);
            std::clog << "\rTiles remaining: " << (screen_tiles.size() - tiles_rendered) << ' ' << std::flush;
        }
    }

    void render_thread(int thread_id, int thread_count) 
    {
        for (int j = thread_id; j < image_height; j += thread_count) 
        {
            for (int i = 0; i < image_width; i++) 
            {
                renderPixel(i,j);
            }
            std::clog << "\rScanlines remaining: " << --scan_lines << ' ' << std::flush;
        }
    }

    void render_tile(const tile& t) 
    {
        for (int i = t.y0; i < t.y1; i++) 
        {
            for (int j = t.x0; j < t.x1; j++) 
            {
                renderPixel(j,i);
            }
        } 
    }

    void renderPixel(int i, int j)
    {
        vec3 pixel_color(0, 0, 0);
        for (int sample = 0; sample < samples_per_pixel; sample++)
        {
            ray r = get_ray(i, j);
            pixel_color += ray_color(r, max_depth, *world_list);
        }
        pixel_color *= pixel_samples_scale;
        framebuffer[j * image_width + i] = pixel_color;
    }

    ray get_ray(int i, int j) const 
    {
        // Construct a camera ray originating from the defocus disk and directed at a randomly
        // sampled point around the pixel location i, j.

        vec3 offset = sample_square();
        vec3 pixel_sample = pixel00_loc
            + ((i + offset.x) * pixel_delta_u)
            + ((j + offset.y) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        vec3 ray_direction = pixel_sample - ray_origin;

        auto ray_time = random_float();

        return ray(ray_origin, ray_direction, ray_time);
    }

    vec3 sample_square() const 
    {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return vec3(random_float() - 0.5f, random_float() - 0.5f, 0);
    }

    vec3 defocus_disk_sample() const 
    {
        // Returns a random point in the camera defocus disk.
        vec3 p = random_in_unit_disk();
        return center + (p.x * defocus_disk_u) + (p.y * defocus_disk_v);
    }

    vec3 ray_color(const ray& r, int depth, const hittable& world) const
    {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0) { return vec3(0, 0, 0); }

        hit_record rec;

        if (world.hit(r, interval(0.001f, infinity), rec)) 
        {
            ray scattered;
            vec3 attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
            {
                return attenuation * ray_color(scattered, depth - 1, world);
            }
            return vec3(0, 0, 0);
        }

        vec3 unit_direction = unit_vector(r.direction());
        vec3 a = 0.5f * (unit_direction.y + 1.0f);
        return (1.0f - a) * vec3(1.0f, 1.0f, 1.0f) + a * vec3(0.5f, 0.7f, 1.0f);
    }
};