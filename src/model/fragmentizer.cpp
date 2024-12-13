#include <iostream>
#include <utility>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <string>
#include <tuple>
#include <vector>
#include <limits>
#include <stdexcept>

#include "model/fragmentizer.h"
#include "model/image.h"

const std::uint8_t kROffset = 0;
const std::uint8_t kGOffset = 1;
const std::uint8_t kBOffset = 2;
const std::uint8_t kAOffset = 3;
constexpr const std::size_t kCacheLimit = 1024 * 1024 * 1024; // 1 Gb

Fragmentizer::Fragmentizer()
    : active_fragment_cutter(0)
    , is_cache_enable(true)
{ }

Fragmentizer::Fragmentizer(const Image &image)
    : active_fragment_cutter(0)
    , image(image)
    , is_cache_enable(true)
{ }

Fragmentizer::Fragmentizer(Image &&image)
    : active_fragment_cutter(0)
    , image(std::move(image))
    , is_cache_enable(true)
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
    if (
        is_cache_enable &&
        fragments_cache.find(fragment_info) != fragments_cache.end()
    ) {
        return std::pair<Image&, bool>(fragments_cache[fragment_info], true);
    }
    else if (is_cache_enable && !IsCacheFull())
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
        fragment_cutters[active_fragment_cutter].get().CutImageToFragment(
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

int Fragmentizer::AddFragmentCutter(const IFragmentCutter& fragment_cutter)
{
    fragment_cutters.push_back(fragment_cutter);
    return fragment_cutters.size() - 1;
}

void Fragmentizer::SetActiveFragmentCutter(int index)
{
    if(index < 0 && index >= fragment_cutters.size())
    {
        throw(std::out_of_range("Wrong fragment cutter index"));
    }

    active_fragment_cutter = index;
}

void Fragmentizer::ClearCache()
{
    fragments_cache.clear();
}

void Fragmentizer::EnableCache()
{
    is_cache_enable = true;
}

void Fragmentizer::DisableCache()
{
    is_cache_enable = false;
}

bool Fragmentizer::IsCacheEnable()
{
    return is_cache_enable;
}
