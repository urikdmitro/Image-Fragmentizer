#ifndef MODEL_FRAGMENTIZER_H_
#define MODEL_FRAGMENTIZER_H_

#include <stdint.h>
#include <map>
#include "image.h"

struct FragmentInfo
{
    struct Less
    {
        bool operator()(const FragmentInfo &a, const FragmentInfo &b) const;
    };

    uint8_t fragments_count;
    uint8_t fragment_number;

    FragmentInfo() : FragmentInfo(0, 0) {}
    FragmentInfo(uint8_t fragments_count, uint8_t fragment_number);
};

class Fragmentizer
{
private:
    Image image;
    std::map<FragmentInfo, Image, FragmentInfo::Less> fragments_cache;

public:
    Fragmentizer();
    Fragmentizer(const Image &image);
    void SetNewImage(const Image &image);

    const Image &GetFragment(FragmentInfo fragment_info);
    const Image &GetImage();
};

#endif // MODEL_FRAGMENTIZER_H_