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

Fragmentizer::Fragmentizer(const IFragmentCutter& fragment_cutter)
    : fragment_cutter(fragment_cutter)
{ }

Fragmentizer::Fragmentizer(
    const IFragmentCutter& fragment_cutter,
    const Image &image
)
    : image(image)
    , fragment_cutter(fragment_cutter)
{ }

Fragmentizer::Fragmentizer(
    const IFragmentCutter& fragment_cutter,
    Image &&image
)
    : image(std::move(image))
    , fragment_cutter(fragment_cutter)
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
        fragment_cutter.CutImageToFragment(
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
