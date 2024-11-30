#include "model/fragmentizer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <string>
#include "model/image.h"

#define UINT8_T_MAX 256
#define R_OFFSET 0
#define G_OFFSET 1
#define B_OFFSET 2
#define A_OFFSET 3

#define IS_IN_BOUNDS(VALUE, LOWER_BOUND, UPPER_BOUND) \
            (VALUE >= LOWER_BOUND && VALUE < UPPER_BOUND)


Fragmentizer::Fragmentizer(const Image &image)
    : image(image){}

Image Fragmentizer::GetSegment(FragmentInfo segment_info)
{
    Image fragment(this->image);

    uint8_t fragment_size = UINT8_T_MAX / segment_info.fragments_count;
    uint8_t lower_bound = fragment_size * segment_info.fragment_number;
    uint8_t upper_bound = lower_bound + fragment_size;

    for (uint32_t x = 0; x < fragment.width; x++)
    {
        for (uint32_t y = 0; y < fragment.height; y++)
        {
            for (uint32_t c = 0; c < fragment.channels; c++)
            {
                int pixel_index = (y * fragment.width + x) * fragment.channels + c;

                if (!IS_IN_BOUNDS(fragment.raw_data[pixel_index + c],
                                  lower_bound,
                                  upper_bound))
                {
                    fragment.raw_data[pixel_index + c] = 0;
                }
            }
        }
    }

    return fragment;
}

const Image& Fragmentizer::GetOrigin()
{
    return image;
}
