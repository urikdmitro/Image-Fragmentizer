#ifndef CONTROLLER_FRAGMENTIZER_CONTROLLER_H_
#define CONTROLLER_FRAGMENTIZER_CONTROLLER_H_

#include <map>
#include <functional>
#include "model/fragmentizer.h"
#include "model/image.h"
#include "view/texture.h"

class FragmentizerController
{
private:
    Fragmentizer &fragmentizer;
    std::map<std::string, int> &fragment_cutters_indices_map;
    bool is_image_valid;

public:
    FragmentizerController(
        Fragmentizer &fragmentizer,
        std::map<std::string, int>& fragment_cutters_indices_map
    );

    int SetNewImage(std::string path);
    Texture GetImage() const;
    Texture GetFragment(
        int fragments_count,
        int fragment_number,
        ChannelsMask::T channels_to_fragmentize = ChannelsMask::kRGB,
        std::uint8_t nonfragment_value = 255
    ) const;


    std::vector<std::string> GetFragmentCuttersNames() const;

    void SetActiveFragmentCutter(const std::string &fragment_cutter_name);

    void ClearCache();
};

#endif // CONTROLLER_FRAGMENTIZER_CONTROLLER_H_
