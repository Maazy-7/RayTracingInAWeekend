# Ray Tracing in a Weekend Ray Tracer

A CPU based offline Ray Tracer implemented from Peter Shirley's 3 Book Ray Tracing in a Weekend series. The ray tracer implements the techniques from all 3 books along with a few of my own optimizations. The ray tracer uses RGB color space for rendering and writes the RGB color data (in binary) to a ppm file with the P6 format.

# Ray Tracer Features  
- Ray Tracer utilizes multithreading to speed up rendering
  - Performed on a Ryzen 9 7940HS with 8 cores 16 threads
  - Thread count is adjustable but I usually use about 14-16 threads
  - An interleaved scanline approach is used to render the image where main thread schedules worker threads to render the next available scanline
  - A tiled rendering approach is also implemented but not used as my tests showed that interleaved was still faster in most cases
- Ray Tracer writes to PPM files
  - Unlike the ray tracer from the books mine uses PPM files with P6 format as it allows for very fast write time
  - Also unlike the book's ray tracer mine uses filestreams to write to the ppm file instead of redirecting cout which also results in faster write time
  - To view images the IrfanView application is used, though any application or website that can read P6 format PPM files will suffice
- Importance Sampling and BVH structure
  - In the 3rd book the importance sampling technique is implemented to achieve higher quality (less noise) images with fewer samples per pixel
  - From the 2nd book a Bounding Volume Hierarchy (BVH) structure was also implemented, this drastically reduces the number of ray checks required (O(n) -> O(log(n))) to find the object hit by the ray
  - Both of these features are extremely helpful as they greatly increase performance, although they aren't implemented perfectly and do need some modifications to perform better and support more primitives and materials

# Rendered Scenes

### 1000 SPP Importance Sampling &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; &ensp; 8000 SPP Random Sampling
<p float="left">
<img src="https://github.com/user-attachments/assets/f02bced0-631a-4718-af6a-ca8fcd652f9d" width = "49%"/>
<img src="https://github.com/user-attachments/assets/95f6871b-8b79-4373-b255-47eb762838ec" width = "49%"/>
</p>

### Specular reflections 1000 SPP Imp. Sampling &emsp; &emsp; 1500 SPP Multiple Materials
<p float="left">
<img src="https://github.com/user-attachments/assets/b3878226-70b4-4411-b26d-ac992ba172e5" width = "49%"/>
<img src="https://github.com/user-attachments/assets/ab90ea9e-e8c3-436c-8093-a519c35d49db" width = "49%"/>
</p>

# Performance
Since the ray tracer is CPU based it does take a while to render ideally a few minutes for a good quality image at HD. Performance however is sped up through multithreading, BVH structures and importance sampling. The Cornell room with 1000 SPP takes about 220s to render whereas the Cornell room without importance sampling and 8000 SPP took about 1300s to render, the Cornell room with the fully metallic box 1000 SPP took about 230s and the Cornell room with multiple objects 1500 SPP took about 2000s however this was probably due to a bug with the BVH structure.

# Next Steps
- Bug Fixes
  - Currently there exist a few bugs with the BVH structure that result in black screens or incorrect renders
- Optimizations
  - There are multiple unnecessary uses of shared pointers which could be easily replaced with unique pointers
  - Many cases of virtual functions which are accessed by each ray everytime a primitive is hit, with a large number of rays being traced the virtual function overhead begins to add up
  - The BVH implementation is done through pointers to nodes for simplicity but could be done through a contiguously allocated vector resulting in a cache coherent BVH
  - There are also other cases of pointer chases which could be replaced with contiguous allocations
  - Utilizing the CPU's SIMD capabilites (AVX, AVX2, and AVX-512) to accelerate certain ray operations  
