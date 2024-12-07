#include "controller/fragmentizer_controller.h"

FragmentizerController::FragmentizerController(
    Fragmentizer &fragmentizer,
    std::map<std::string, int> &fragment_cutters_indices_map
)
    : fragmentizer(fragmentizer)
    , fragment_cutters_indices_map(fragment_cutters_indices_map)
{ }

void FragmentizerController::SetNewImage(std::string path_to_image)
{
    fragmentizer.SetNewImage(Image(path_to_image));
}

Texture FragmentizerController::GetFragment(
    int fragments_count,
    int fragment_number,
    ChannelsMask::T channels_to_fragmentize,
    std::uint8_t nonfragment_value
) const
{
    return Texture(
        fragmentizer.GetFragment(
            FragmentInfo(
                fragments_count,
                fragment_number,
                channels_to_fragmentize,
                nonfragment_value
            )
        )
    );
}

Texture FragmentizerController::GetImage() const
{
    return Texture(fragmentizer.GetImage());
}

std::vector<std::string>
FragmentizerController::GetFragmentCuttersNames() const
{
    std::vector<std::string> fragment_cutters_names_vector;
    fragment_cutters_names_vector.reserve(fragment_cutters_indices_map.size());

    for(const auto& i : fragment_cutters_indices_map)
    {
        fragment_cutters_names_vector.push_back(i.first);
    }

    return fragment_cutters_names_vector;
}

// TODO: handle out of bounds
void FragmentizerController::SetActiveFragmentCutter(
    const std::string &fragment_cutter_name
)
{
    fragmentizer.SetActiveFragmentCutter(
        fragment_cutters_indices_map.find(
            fragment_cutter_name
        )->second
    );
}

void FragmentizerController::ClearCache()
{
    fragmentizer.ClearCache();
}
