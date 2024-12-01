#include "model/fragmentizer.h"

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <string>
#include "model/image.h"

const std::uint32_t kUInt8Max = 256;
const std::uint8_t kROffset = 0;
const std::uint8_t kGOffset = 1;
const std::uint8_t kBOffset = 2;
const std::uint8_t kAOffset = 3;

bool FragmentInfo::Less::operator()(
    const FragmentInfo &a,
    const FragmentInfo &b
) const
{
    return (a.fragments_count << 8 | a.fragment_number)
            < (b.fragments_count << 8 | b.fragment_number);
}

FragmentInfo::FragmentInfo(uint8_t fragments_count, uint8_t fragment_number)
{
    this->fragment_number = fragment_number;
    this->fragments_count = fragments_count;
}

template <typename T>
inline bool IsInBounds(T value, T lower_bound, T upper_bound)
{
    return value >= lower_bound && value < upper_bound;
}

Fragmentizer::Fragmentizer()
{ }

Fragmentizer::Fragmentizer(const Image &image)
    : image(image)
{ }

void Fragmentizer::SetNewImage(const Image &image)
{
    fragments_cache.clear();
    this->image = image;
}

const Image &Fragmentizer::GetFragment(FragmentInfo fragment_info)
{
    if(fragments_cache.find(fragment_info) != fragments_cache.end())
    {
        std::cout << "Find\n";
        return fragments_cache.at(fragment_info);
    }
    std::cout << "Not find\n";

    Image& fragment =
        fragments_cache.emplace(fragment_info, this->image).first->second;

    std::uint8_t fragment_size = kUInt8Max / fragment_info.fragments_count;
    std::uint8_t lower_bound = fragment_size * fragment_info.fragment_number;
    std::uint8_t upper_bound = lower_bound + fragment_size;

    for (std::uint32_t x = 0; x < fragment.width; x++)
    {
        for (std::uint32_t y = 0; y < fragment.height; y++)
        {
            for (std::uint32_t c = 0; c < fragment.channels; c++)
            {
                int pixel_index =
                    (y * fragment.width + x)* fragment.channels + c;

                if (!IsInBounds(
                        fragment.raw_data[pixel_index + c],
                        lower_bound,
                        upper_bound
                    )
                    && c != 3
                )
                {
                    fragment.raw_data[pixel_index + c] = 255;
                }
            }
        }
    }

    return fragment;
}

const Image& Fragmentizer::GetImage()
{
    return image;
}
