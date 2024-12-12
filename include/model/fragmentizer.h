#ifndef MODEL_FRAGMENTIZER_H_
#define MODEL_FRAGMENTIZER_H_

#include <map>
#include <cstdint>
#include <utility>
#include <vector>
#include <functional>
#include "fragment_cutter.h"
#include "fragment_info.h"
#include "image.h"

enum FragmentizerStatus
{
    FS_IN_PROGRESS,
    FS_DONE,
    FS_ERROR,
};

class Fragmentizer
{
private:
    std::vector<std::reference_wrapper<const IFragmentCutter>> fragment_cutters;
    int active_fragment_cutter;

    Image image;
    FragmentizerStatus status;

    Image reserved_fragment;
    std::map<FragmentInfo, Image, FragmentInfo::Comparator> fragments_cache;

    bool IsCacheFull();
    std::pair<Image&, bool> TryFindInCache(FragmentInfo fragment_info);

public:
    Fragmentizer();
    Fragmentizer(const Image &image);
    Fragmentizer(Image &&image);

    void SetNewImage(const Image &image);
    void SetNewImage(Image &&image);

    int AddFragmentCutter(const IFragmentCutter& fragment_cutter);
    void SetActiveFragmentCutter(int index);

    // FragmentizerStatus GetStatus();
    // FragmentizerStatus RunFragmentation(FragmentInfo fragment_info);
    // const Image &GetResult();

    const Image &GetFragment(FragmentInfo fragment_info);

    const Image &GetImage();

    void ClearCache();
};

#endif // MODEL_FRAGMENTIZER_H_
