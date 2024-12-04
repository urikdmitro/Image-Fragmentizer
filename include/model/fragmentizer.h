#ifndef MODEL_FRAGMENTIZER_H_
#define MODEL_FRAGMENTIZER_H_

#include <map>
#include <cstdint>
#include <utility>
#include <vector>
#include "fragment_cutter.h"
#include "fragment_info.h"
#include "image.h"

class Fragmentizer
{
private:
    const IFragmentCutter &fragment_cutter;
    Image image;
    Image reserved_fragment;
    std::map<FragmentInfo, Image, FragmentInfo::Comparator> fragments_cache;

    bool IsCacheFull();
    std::pair<Image&, bool> TryFindInCache(FragmentInfo fragment_info);

public:
    Fragmentizer(const IFragmentCutter& fragment_cutter);
    Fragmentizer(const IFragmentCutter& fragment_cutter, const Image &image);
    Fragmentizer(const IFragmentCutter& fragment_cutter, Image &&image);

    void SetNewImage(const Image &image);
    void SetNewImage(Image &&image);

    const Image &GetFragment(FragmentInfo fragment_info);

    const Image &GetImage();
};

#endif // MODEL_FRAGMENTIZER_H_
