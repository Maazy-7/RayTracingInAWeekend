#pragma once

// Disable strict warnings for this header from the Microsoft Visual C++ compiler.
#ifdef _MSC_VER
#pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

#include <iostream>
#include <cstdlib>

#include "stb_image.h"

class image 
{
public:
	image() {}

	image(const image&) = delete;
	image& operator=(const image&) = delete;

	image(const char* filename) 
	{
		std::string path = "Assets/" + std::string(filename);

		if (!load(path))
		{
			std::cerr << "ERROR: could not load image: " << filename << "\n";
		}
	}

	~image() 
	{
		delete[] bdata;
		STBI_FREE(fdata);
	}

	bool load(std::string& filename) 
	{
		// Loads the linear (gamma=1) image data from the given file name. Returns true if the
		// load succeeded. The resulting data buffer contains the three [0.0, 1.0]
		// floating-point values for the first pixel (red, then green, then blue). Pixels are
		// contiguous, going left to right for the width of the image, followed by the next row
		// below, for the full height of the image.

		int dummy = bytes_per_pixel;
		fdata = stbi_loadf(filename.c_str(), &image_width, &image_height, &dummy, bytes_per_pixel);
		if (fdata == nullptr) { return false; }

		bytes_per_scanline = image_width * bytes_per_pixel;
		convert_to_bytes();
		return true;
	}

	int width()  const { return (fdata == nullptr) ? 0 : image_width; }
	int height() const { return (fdata == nullptr) ? 0 : image_height; }

	const uint8_t* pixel_data(int x, int y) const 
	{
		// Return the address of the three RGB bytes of the pixel at x,y. If there is no image
		// data, returns magenta.
		static uint8_t magenta[] = { 255, 0, 255 };
		if (bdata == nullptr) return magenta;

		x = clamp(x, 0, image_width);
		y = clamp(y, 0, image_height);

		return bdata + y * bytes_per_scanline + x * bytes_per_pixel;
	}

private:
	const int	   bytes_per_pixel = 3;
	float*		   fdata = nullptr;         // Linear floating point pixel data
	uint8_t*	   bdata = nullptr;         // Linear 8-bit pixel data
	int            image_width = 0;         // Loaded image width
	int            image_height = 0;        // Loaded image height
	int            bytes_per_scanline = 0;


	static int clamp(int x, int low, int high) 
	{
		// Return the value clamped to the range [low, high).
		if (x < low) { return low; }
		if (x < high) { return x; }
		return high - 1;
	}

	static uint8_t float_to_byte(float value)
	{
		if (value <= 0.0f) { return 0; }
		if (1.0f <= value) { return 255; }
		return static_cast<uint8_t>(256.0f * value);
	}

	void convert_to_bytes() 
	{
		// Convert the linear floating point pixel data to bytes, storing the resulting byte
		// data in the `bdata` member.

		int total_bytes = image_width * image_height * bytes_per_pixel;
		bdata = new uint8_t[total_bytes];

		// Iterate through all pixel components, converting from [0.0, 1.0] float values to
		// unsigned [0, 255] byte values.

		uint8_t* bptr = bdata;
		float* fptr = fdata;
		for (int i = 0; i < total_bytes; i++, fptr++, bptr++)
		{
			*bptr = float_to_byte(*fptr);
		}
	}
};

// Restore MSVC compiler warnings
#ifdef _MSC_VER
#pragma warning (pop)
#endif