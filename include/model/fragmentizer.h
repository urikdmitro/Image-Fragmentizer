#ifndef SEGMENTIZER_H_
#define SEGMENTIZER_H_

#include <stdint.h>
#include "image.h"

struct FragmentInfo
{
    uint8_t fragments_count;
    uint8_t fragment_number;

    FragmentInfo() : FragmentInfo(0, 0) {}
    FragmentInfo(uint8_t fragments_count, uint8_t fragment_number)
    {
        this->fragment_number = fragment_number;
        this->fragments_count = fragments_count;
    }
};

class Fragmentizer
{
private:
    const Image &image;

public:
    Fragmentizer(const Image &image);

    Image GetSegment(FragmentInfo segment_info);
    const Image &GetOrigin();
};

#endif // SEGMENTIZER_H_