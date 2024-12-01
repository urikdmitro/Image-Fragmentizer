#ifndef CONTROLLER_FRAGMENTIZER_CONTROLLER_H_
#define CONTROLLER_FRAGMENTIZER_CONTROLLER_H_

#include "model/fragmentizer.h"
#include "model/image.h"
#include "view/texture.h"

class FragmentizerController
{
private:
    Fragmentizer &fragmentizer;

public:
    FragmentizerController(Fragmentizer &fragmentizer);

    void SetNewImage(std::string path);
    Texture GetImage() const;
    Texture GetFragment(
        int fragments_count,
        int fragment_number,
        ChannelsMask::T channels_to_fragmentize = ChannelsMask::kRGB,
        std::uint8_t nonfragment_value = 255
    ) const;
};

#endif // CONTROLLER_FRAGMENTIZER_CONTROLLER_H_
