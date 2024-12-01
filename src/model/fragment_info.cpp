#include "model/fragment_info.h"

const std::uint8_t FragmentInfo::kMaxSupportedChanels = 4;

FragmentInfo::FragmentInfo()
    : FragmentInfo(0, 0)
{ }

FragmentInfo::FragmentInfo(
    std::uint8_t fragments_count,
    std::uint8_t fragment_number
)
    : FragmentInfo(fragments_count, fragment_number, ChannelsMask::kRGB, 255)
{ }

FragmentInfo::FragmentInfo(
    std::uint8_t fragments_count,
    std::uint8_t fragment_number,
    ChannelsMask::T channels_mask,
    std::uint8_t nonfragment_pixel_value
)
    : fragments_count(fragments_count)
    , fragment_number(fragment_number)
    , channels_mask(channels_mask)
    , nonfragment_pixel_value(nonfragment_pixel_value)
{ }

std::vector<std::uint8_t> FragmentInfo::GetChannelsToFragmentize()
{
    std::vector<std::uint8_t> channels_to_fragmentize;
    channels_to_fragmentize.reserve(kMaxSupportedChanels);

    for(std::uint8_t i = 0; i < kMaxSupportedChanels; i++)
    {
        if(channels_mask & 1 << i)
        {
            channels_to_fragmentize.push_back(i);
        }
    }

    return channels_to_fragmentize;
}

bool FragmentInfo::Comparator::operator()(
    const FragmentInfo &a,
    const FragmentInfo &b
) const
{
    return (
        (
            a.fragments_count                           << 24   |
            a.nonfragment_pixel_value                   << 16   |
            static_cast<std::uint8_t>(a.channels_mask)  << 8    |
            a.fragment_number
        ) < (
            b.fragments_count                           << 24   |
            b.nonfragment_pixel_value                   << 16   |
            static_cast<std::uint8_t>(b.channels_mask)  << 8    |
            b.fragment_number
        )
    );
}