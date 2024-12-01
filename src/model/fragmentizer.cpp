#include "model/fragmentizer.h"

#include <iostream>
#include <utility>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <string>
#include <tuple>
#include <vector>
#include <limits>
#include "model/image.h"

const std::uint8_t kROffset = 0;
const std::uint8_t kGOffset = 1;
const std::uint8_t kBOffset = 2;
const std::uint8_t kAOffset = 3;
constexpr const std::size_t kCacheLimit = 1024 * 1024 * 1024; // 1 Gb

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

Fragmentizer::Fragmentizer(Image &&image)
    : image(std::move(image))
{ }

void Fragmentizer::SetNewImage(const Image &image)
{
    fragments_cache.clear();
    this->image = image;
}

void Fragmentizer::SetNewImage(Image &&image)
{
    fragments_cache.clear();
    this->image = std::move(image);
}

void Fragmentizer::CutImageToFragment(FragmentInfo fragment_info, Image& image)
{
    auto channels_to_fragmentize = fragment_info.GetChannelsToFragmentize();
    if(channels_to_fragmentize.size() == 0) return;

    // Calculating fragments size by dividing 256 on fragments count
    std::uint8_t fragment_size =
        (std::numeric_limits<std::uint8_t>::max() + 1)
        / fragment_info.fragments_count;

    std::uint8_t lower_bound = fragment_size * fragment_info.fragment_number;
    std::uint8_t upper_bound = lower_bound + fragment_size;

    if(fragment_info.fragments_count - 1 == fragment_info.fragment_number)
        upper_bound = std::numeric_limits<std::uint8_t>::max();

    for (std::uint32_t x = 0; x < image.width; x++)
    {
        for (std::uint32_t y = 0; y < image.height; y++)
        {
            for (auto channel : channels_to_fragmentize)
            {
                int pixel_index =
                    (y * image.width + x) * image.channels + channel;

                if (!IsInBounds(
                        image.raw_data[pixel_index],
                        lower_bound,
                        upper_bound
                    )
                )
                {
                    image.raw_data[pixel_index]
                        = fragment_info.nonfragment_pixel_value;
                }
            }
        }
    }
}

bool Fragmentizer::IsCacheFull()
{
    return (
        (fragments_cache.size() + 1)
        * image.width
        * image.height
        * image.channels
         > kCacheLimit
    );
}

std::pair<Image&, bool> Fragmentizer::TryFindInCache(FragmentInfo fragment_info)
{
    if(fragments_cache.find(fragment_info) != fragments_cache.end())
    {
        return std::pair<Image&, bool>(fragments_cache[fragment_info], true);
    }
    else if (!IsCacheFull())
    {
        return std::pair<Image&, bool>(
            fragments_cache.emplace(fragment_info, this->image).first->second,
            false
        );
    }
    else
    {
        return std::pair<Image&, bool>(reserved_fragment, false);
    }
}

const Image &Fragmentizer::GetFragment(FragmentInfo fragment_info)
{
    auto fragment = TryFindInCache(fragment_info);

    if(!fragment.second)
    {
        CutImageToFragment(
            fragment_info,
            fragment.first
            );
    }

    return fragment.first;
}

const Image& Fragmentizer::GetImage()
{
    return image;
}
