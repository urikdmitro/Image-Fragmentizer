#include "model/fragment_plotter.h"
#include "fragment_plotter.h"
#include <numeric>
#include <cmath>


FragmentPlotter::FragmentPlotter(Fragmentizer& fragmentizer)
    : fragmentizer(fragmentizer)
{ }

IntensityGraph FragmentPlotter::GetIntensity(
    const Image &image,
    int nonfragment_value
) {
    IntensityGraph result;
    result.main.reserve(256);
    result.r.reserve(256);
    result.g.reserve(256);
    result.b.reserve(256);
    std::fill(result.main.begin(), result.main.end(), 0);
    std::fill(result.r.begin(), result.r.end(), 0);
    std::fill(result.g.begin(), result.g.end(), 0);
    std::fill(result.b.begin(), result.b.end(), 0);
    result.x = std::vector<double>(256);
    std::iota(result.x.begin(), result.x.end(), 0);

    for(int i = 0; i < image.width * image.height; ++i)
    {
        int pixel_index = i * image.channels;
        int r = static_cast<int>(image.raw_data[pixel_index]);
        int g = static_cast<int>(image.raw_data[pixel_index + 1]);
        int b = static_cast<int>(image.raw_data[pixel_index + 2]);
        int absolute_pixel_value = (r + g + b) / 3;

        if(absolute_pixel_value != nonfragment_value)
            result.main[absolute_pixel_value]++;

        if(r != nonfragment_value)
            result.r[r]++;

        if(g != nonfragment_value)
            result.g[g]++;

        if(b != nonfragment_value)
            result.b[b]++;
    }

    // for(int i = 0; i < 256; i++)
    // {
    //     result.main[i] /= static_cast<double>(image.GetSize());
    //     result.r[i] /= static_cast<double>(image.GetSize());
    //     result.g[i] /= static_cast<double>(image.GetSize());
    //     result.b[i] /= static_cast<double>(image.GetSize());
    // }


    return result;
}

IntensityGraph FragmentPlotter::GetDensity(
    const Image &image,
    int nonfragment_value,
    const IntensityGraph &image_intensity
) {
    IntensityGraph result;
    result.main.reserve(256);
    result.r.reserve(256);
    result.g.reserve(256);
    result.b.reserve(256);
    std::fill(result.main.begin(), result.main.end(), 0);
    std::fill(result.r.begin(), result.r.end(), 0);
    std::fill(result.g.begin(), result.g.end(), 0);
    std::fill(result.b.begin(), result.b.end(), 0);
    result.x = std::vector<double>(256);
    std::iota(result.x.begin(), result.x.end(), 0);

    for(int i = 0; i < image.width * image.height; ++i)
    {
        int pixel_index = i * image.channels;
        int r = static_cast<int>(image.raw_data[pixel_index]);
        int g = static_cast<int>(image.raw_data[pixel_index + 1]);
        int b = static_cast<int>(image.raw_data[pixel_index + 2]);
        int absolute_pixel_value = (r + g + b) / 3;

        if(absolute_pixel_value != nonfragment_value)
            result.main[absolute_pixel_value]++;

        if(r != nonfragment_value)
            result.r[r]++;

        if(g != nonfragment_value)
            result.g[g]++;

        if(b != nonfragment_value)
            result.b[b]++;
    }

    for(int i = 0; i < 256; i++)
    {
        result.main[i] = pow(result.main[i] - image_intensity.main[i] * static_cast<double>(image.GetSize()), 2) / static_cast<double>(image.GetSize());
        result.r[i] = pow(result.r[i] - image_intensity.r[i] * static_cast<double>(image.GetSize()), 2) / static_cast<double>(image.GetSize());
        result.g[i] = pow(result.g[i] - image_intensity.g[i] * static_cast<double>(image.GetSize()), 2) / static_cast<double>(image.GetSize());
        result.b[i] = pow(result.b[i] - image_intensity.b[i] * static_cast<double>(image.GetSize()), 2) / static_cast<double>(image.GetSize());
    }


    return result;
}

std::vector<double> FragmentPlotter::GetDensity(int fragments_count)
{
    return std::vector<double>();
}