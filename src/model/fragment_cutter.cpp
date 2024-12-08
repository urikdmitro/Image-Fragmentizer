#include "model/fragment_cutter.h"

template <typename T>
inline bool IsInBounds(T value, T lower_bound, T upper_bound)
{
    return value >= lower_bound && value < upper_bound;
}

void FragmentCutter::CutImageToFragment(
    FragmentInfo fragment_info,
    Image& image
) const
{
    auto channels_to_fragmentize = fragment_info.GetChannelsToFragmentize();
    if(channels_to_fragmentize.size() == 0) return;

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
                        fragment_info.GetBounds().first,
                        fragment_info.GetBounds().second
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
