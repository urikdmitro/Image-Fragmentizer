#ifndef MODEL_FRAGMENT_INFO_H_
#define MODEL_FRAGMENT_INFO_H_

#include <cstdint>
#include <vector>
#include "channels_mask.h"

struct FragmentInfo
{
private:
    static const std::uint8_t kMaxSupportedChanels;

public:
    struct Comparator
    {
        bool operator()(const FragmentInfo &a, const FragmentInfo &b) const;
    };

    std::uint8_t fragments_count;
    std::uint8_t fragment_number;
    ChannelsMask::T channels_mask;
    std::uint8_t nonfragment_pixel_value;

    FragmentInfo();
    FragmentInfo(
        std::uint8_t fragments_count,
        std::uint8_t fragment_number
    );
    FragmentInfo(
        std::uint8_t fragments_count,
        std::uint8_t fragment_number,
        ChannelsMask::T channels_mask,
        std::uint8_t nonfragment_pixel_value
    );

    std::vector<std::uint8_t> GetChannelsToFragmentize();
};


#endif // MODEL_FRAGMENT_INFO_H_
