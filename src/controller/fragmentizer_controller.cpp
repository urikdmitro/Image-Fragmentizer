#include "controller/fragmentizer_controller.h"
#include "fragmentizer_controller.h"
#include "model/fragment_plotter.h"

FragmentizerController::FragmentizerController(
    Fragmentizer &fragmentizer,
    std::map<std::string, int> &fragment_cutters_indices_map
)
    : fragmentizer(fragmentizer)
    , fragment_cutters_indices_map(fragment_cutters_indices_map)
    , is_image_valid(false)
{ }

int FragmentizerController::SetNewImage(std::string path_to_image)
{
    Image image(path_to_image);

    if(image.GetSize() <= 0)
    {
        is_image_valid = false;
        return -1;
    }

    fragmentizer.SetNewImage(std::move(image));
    is_image_valid = true;

    return 0;
}

Texture FragmentizerController::GetFragment(
    int fragments_count,
    int fragment_number,
    ChannelsMask::T channels_to_fragmentize,
    std::uint8_t nonfragment_value
) const
{
    if(!is_image_valid) return Texture();

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

int FragmentizerController::SaveFragmentToFile(
    const std::string &path,
    int fragments_count,
    int fragment_number,
    ChannelsMask::T channels_to_fragmentize,
    std::uint8_t nonfragment_value
) const
{
    if(!is_image_valid) return -1;

    return fragmentizer.GetFragment(
        FragmentInfo(
            fragments_count,
            fragment_number,
            channels_to_fragmentize,
            nonfragment_value
        )
    ).SaveToFile(path);
}

std::chrono::milliseconds FragmentizerController::RunBenchmark(
    int fragments_count,
    bool use_cache,
    ChannelsMask::T channels_mask
)
{
    return Benchmarker(fragmentizer).RunFullBenchmark(
        fragments_count,
        use_cache,
        channels_mask
    );
}

Texture FragmentizerController::GetImage() const
{
    if(!is_image_valid) return Texture();

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

IntensityGraph FragmentizerController::GetFragmentIntensityGraph(int fragments_count, int fragment_number)
{
    FragmentPlotter plotter(fragmentizer);
    int nonfragment_value = fragment_number < fragments_count / 2 ? 255 : 0;

    return plotter.GetIntensity(
        fragmentizer.GetFragment(
            FragmentInfo(
                fragments_count,
                fragment_number,
                ChannelsMask::kRGB,
                nonfragment_value
            )
        ),
        nonfragment_value
    );
}

IntensityGraph FragmentizerController::GetFragmentDensityGraph(int fragments_count, int fragment_number)
{
    FragmentPlotter plotter(fragmentizer);
    int nonfragment_value = fragment_number < fragments_count / 2 ? 255 : 0;

    auto image_intensity = plotter.GetIntensity(fragmentizer.GetImage(), -1);

    return plotter.GetDensity(
        fragmentizer.GetFragment(
            FragmentInfo(
                fragments_count,
                fragment_number,
                ChannelsMask::kRGB,
                nonfragment_value
            )
        ),
        nonfragment_value,
        image_intensity
    );
}


void FragmentizerController::ClearCache()
{
    fragmentizer.ClearCache();
}
