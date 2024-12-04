#include "model/fragment_cutter.h"

#include <limits>

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
