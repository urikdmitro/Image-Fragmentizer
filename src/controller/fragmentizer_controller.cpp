#include "controller/fragmentizer_controller.h"

FragmentizerController::FragmentizerController(Fragmentizer &fragmentizer)
    : fragmentizer(fragmentizer)
{ }

void FragmentizerController::SetNewImage(std::string path_to_image)
{
    fragmentizer.SetNewImage(Image(path_to_image));
}

Texture FragmentizerController::GetFragment(
    int fragments_count,
    int fragment_number
) const
{
    return Texture(fragmentizer.GetFragment(
        FragmentInfo(fragments_count, fragment_number)));
}

Texture FragmentizerController::GetImage() const
{
    return Texture(fragmentizer.GetImage());
}
