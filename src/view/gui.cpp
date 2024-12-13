#include "view/gui.h"
#include <iostream>
#include <cmath>
#include <cstring>
#include <nfd.h>
#include <string>
#include "resources.h"
#include "view/color_style.h"
#include <algorithm>
#include <bitset>
#include <imgui_internal.h>
#include <numeric>
#include "implot.h"

const float kFloatEpsilon = 0.00001f;

Gui::Gui(
    const OpenGLContext &opengl_context,
    FragmentizerController &controller
)
    : opengl_context(opengl_context)
    , controller(controller)
    , accent_color(ColorStyle::Mauve)
    , splitter_ratio(0.33f)
    , input_data(
        InputData {
            .fragments_count = 10,
            .selected_fragment = 5,
            .path_to_image = std::string("none"),
            .fragmentize_red_channel   = true,
            .fragmentize_green_channel = true,
            .fragmentize_blue_channel  = true,
            .nonfragment_pixel_value  = 255,
            .origin_image_scale     = 100,
            .fragment_image_scale   = 100,
            .variance_graph_scale   = 100,
            .intensity_graph_scale  = 100,
            .selected_calculation_method = 0,
            .calculation_methods = controller.GetFragmentCuttersNames(),
        }
    )
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(opengl_context.GetWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();
    SetColorStyle();

    ImPlot::CreateContext();
}

std::string Gui::OpenFilePickerForSave()
{
    nfdchar_t *result_path = NULL;
    nfdresult_t result = NFD_SaveDialog("*.bmp;*.jpg;*.png", NULL, &result_path);

    if (result != NFD_OKAY)
    {
        std::cerr << "No file selected" << std::endl;
        return "";
    }
    else
    {
        std::cout
            << "RESULT FILE: "
            << std::string(static_cast<char *>(result_path));
    }

    return std::string(static_cast<char *>(result_path));
}

std::string Gui::OpenFilePicker()
{
    nfdchar_t *result_path = NULL;
    nfdresult_t result = NFD_OpenDialog(NULL, NULL, &result_path);

    if (result != NFD_OKAY)
    {
        std::cerr << "No file selected" << std::endl;
        return "";
    }

    return std::string(static_cast<char *>(result_path));
}

void Gui::UpdateFragment()
{
    ChannelsMask::T mask = static_cast<ChannelsMask::T>(
        (input_data.fragmentize_red_channel
            ? ChannelsMask::kR
            : ChannelsMask::kNone)
        | (input_data.fragmentize_green_channel
            ? ChannelsMask::kG
            : ChannelsMask::kNone)
        | (input_data.fragmentize_blue_channel
            ? ChannelsMask::kB
            : ChannelsMask::kNone)
    );

    output_data.fragment_image = controller.GetFragment(
        input_data.fragments_count,
        input_data.selected_fragment,
        mask,
        input_data.nonfragment_pixel_value
    );
}

void Gui::SaveFragment()
{
    std::string path_to_save = OpenFilePickerForSave();

    ChannelsMask::T mask = static_cast<ChannelsMask::T>(
        (input_data.fragmentize_red_channel
            ? ChannelsMask::kR
            : ChannelsMask::kNone)
        | (input_data.fragmentize_green_channel
            ? ChannelsMask::kG
            : ChannelsMask::kNone)
        | (input_data.fragmentize_blue_channel
            ? ChannelsMask::kB
            : ChannelsMask::kNone)
    );


    controller.SaveFragmentToFile(
        path_to_save,
        input_data.fragments_count,
        input_data.selected_fragment,
        mask,
        input_data.nonfragment_pixel_value
    );
}

void Gui::SelectImage()
{
    input_data.path_to_image = OpenFilePicker();

    if(controller.SetNewImage(input_data.path_to_image))
    {
        input_data.path_to_image = "none";
    }
    else
    {
        RerunFragmentation();
        AdjustImageScale();
    }
}

void Gui::AdjustImageScale()
{
    input_data.origin_image_scale = static_cast<int>(
        (
            ImGui::GetMainViewport()->WorkSize.x * (1.0f - splitter_ratio)
            - 3 * ImGui::GetStyle().WindowPadding.x
        )
        / controller.GetImage().GetWidth() * 100.0f
    );
    input_data.fragment_image_scale = input_data.origin_image_scale;
}

void Gui::RerunFragmentation()
{
    output_data.origin_image = controller.GetImage();
    UpdateFragment();
}

inline bool operator==(const ImVec2& lhs, const ImVec2& rhs)
{
    return std::abs(lhs.x - rhs.x) < kFloatEpsilon
        && std::abs(lhs.y - rhs.y) < kFloatEpsilon;
}

inline bool operator!=(const ImVec2& lhs, const ImVec2& rhs)
{
    return !(lhs == rhs);
}

void Gui::ValidateInputData()
{
    if(input_data.fragments_count > 255)
        input_data.fragments_count = 255;

    if(input_data.fragments_count < 2)
        input_data.fragments_count = 2;

    if(input_data.selected_fragment >= input_data.fragments_count)
        input_data.selected_fragment = input_data.fragments_count - 1;

    if(input_data.selected_fragment < 0)
        input_data.selected_fragment = 0;

    if(input_data.nonfragment_pixel_value > 255)
        input_data.nonfragment_pixel_value = 255;

    if(input_data.nonfragment_pixel_value < 0)
        input_data.nonfragment_pixel_value = 0;
}

void Gui::BuildInputUI()
{

    if (ImGui::Button("Select Image"))
    {
        SelectImage();
        has_input_data_changed = true;
    }

    ImGui::SameLine();

    if (ImGui::Button("Save Fragment"))
    {
        SaveFragment();
    }

    if (ImGui::Button("Rerun"))
    {
        RerunFragmentation();
        has_input_data_changed = true;
    }
    ImGui::SetItemTooltip("Rerun fragmentation. It is helpfull when some error occurred");

    ImGui::SameLine();

    if (ImGui::Button("Build intensity"))
    {
        output_data.intensity_graph = controller.GetFragmentIntensityGraph(
            input_data.fragments_count,
            input_data.selected_fragment
        );

        output_data.density_graph = controller.GetFragmentDensityGraph(
            input_data.fragments_count,
            input_data.selected_fragment
        );

        has_input_data_changed = true;
    }
    ImGui::SetItemTooltip("Build intensity plot");

    if (ImGui::Button("Clear cache"))
    {
        controller.ClearCache();
        has_input_data_changed = true;
    }
    ImGui::SetItemTooltip(
        "Helps when your computer has low memory.\n"
        "Cache is cleared automaticaly, when new image is uploaded.\n"
        "Default cache max size is 1Gb\n"
    );

    ImGui::SameLine();

    if (ImGui::Button("Adjust scale"))
    {
        AdjustImageScale();
        has_input_data_changed = true;
    }
    ImGui::SetItemTooltip(
        "Resizes the origin and fragment image to fit the window size"
    );

    // static bool show_benchmark = false;
    // if (ImGui::Button("Run benchmark"))
    // {
    //     show_benchmark = true;
    // }
    // if(show_benchmark) {

    //     ChannelsMask::T mask = static_cast<ChannelsMask::T>(
    //         (input_data.fragmentize_red_channel
    //             ? ChannelsMask::kR
    //             : ChannelsMask::kNone)
    //         | (input_data.fragmentize_green_channel
    //             ? ChannelsMask::kG
    //             : ChannelsMask::kNone)
    //         | (input_data.fragmentize_blue_channel
    //             ? ChannelsMask::kB
    //             : ChannelsMask::kNone)
    //     );

    //     ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    //     ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5, 0.5));

    //     ImGui::OpenPopup("Running benchmark");
    //     std::chrono::milliseconds benchmark_result;

    //     if (ImGui::BeginPopupModal("Running benchmark")) {
    //         ImGui::Text("In progress");
    //         benchmark_result = controller.RunBenchmark(
    //             input_data.fragments_count,
    //             true,
    //             mask
    //         );
    //          if (ImGui::Button("OK", ImVec2(200, 0))) {
    //              ImGui::CloseCurrentPopup();
    //         }
    //         ImGui::EndPopup();
    //     }

    //     // ImGui::OpenPopup("Benchmark result");
    //     // if (ImGui::BeginPopupModal("Benchmark result")) {
    //     //     ImGui::Text("Time: %lld ms", benchmark_result);
    //     //     if (ImGui::Button("OK", ImVec2(200, 0))) {
    //     //         ImGui::CloseCurrentPopup();
    //     //     }
    //     //     ImGui::EndPopup();
    //     // }

    // }


    ImGui::Separator();


    ImGui::PushTextWrapPos(
        splitter_ratio * ImGui::GetMainViewport()->WorkSize.x
        - 2 * ImGui::GetStyle().WindowPadding.x
    );

    ImGui::Text("Selected Image: %s", input_data.path_to_image.c_str());

    ImGui::PopTextWrapPos();


    ImGui::Separator();


    ImGui::Text("Selected Fragment");
    if (ImGui::InputInt (
            "##selected_fragment",
            &(input_data.selected_fragment),
            1,
            10
        )
    )
    {
        has_input_data_changed = true;
    }
    ImGui::SetItemTooltip(
        "You can find out more information about"
        " the calculated fragment under its image"
    );

    ImGui::Text("Number of Fragments");
    if (ImGui::InputInt(
            "##fragments_count",
            &(input_data.fragments_count),
            1,
            10
        )
    )
    {
        has_input_data_changed = true;
    }
    ImGui::SetItemTooltip(
        "You can find out more information about"
        " the calculated fragment under its image"
    );


    ImGui::Separator();

    ImGui::Text("Channels to fragmentize");
    ImGui::SetItemTooltip(
        "Include or exclude channels to be fragmentize. "
    );

    if (ImGui::Checkbox("Red", &(input_data.fragmentize_red_channel)))
    {
        has_input_data_changed = true;
    }
    ImGui::SetItemTooltip(
        "Include or exclude channels to be fragmentize. "
    );

    ImGui::SameLine();

    if (ImGui::Checkbox("Green", &(input_data.fragmentize_green_channel)))
    {
        has_input_data_changed = true;
    }
    ImGui::SetItemTooltip(
        "Include or exclude channels to be fragmentize. "
    );

    ImGui::SameLine();

    if (ImGui::Checkbox("Blue", &(input_data.fragmentize_blue_channel)))
    {
        has_input_data_changed = true;
    }
    ImGui::SetItemTooltip(
        "Include or exclude channels to be fragmentize. "
    );


    ImGui::Separator();


    ImGui::Text("Nonfragment value");
    if (ImGui::InputInt(
            "##nonfragment_pixel_value",
            &(input_data.nonfragment_pixel_value),
            255,
            255
        )
    )
    {
        has_input_data_changed = true;
    }
    ImGui::SetItemTooltip(
        "Value to mark pixel which is out of fragment bounds"
    );


    ImGui::Separator();


    ImGui::Text("Calculation method");
    if (ImGui::BeginCombo(
            "##calculation method",
            input_data.calculation_methods[
                input_data.selected_calculation_method
            ].c_str()
        )
    )
    {
        for (int i = 0; i < input_data.calculation_methods.size(); ++i)
        {
            bool is_selected =
                (input_data.selected_calculation_method == i);

            if (ImGui::Selectable(
                    input_data.calculation_methods[i].c_str(),
                    is_selected
                )
            )
            {
                input_data.selected_calculation_method = i;
            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
        has_input_data_changed = true;
    }
    ImGui::SetItemTooltip(
        "OpenCL use video card for calculation speed up"
    );


    controller.SetActiveFragmentCutter(input_data.calculation_methods[input_data.selected_calculation_method]);
}

void Gui::DrawImage(const Texture& texture, int &scale, const std::string &text)
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 cursor_position = ImGui::GetCursorScreenPos();

    ImGui::Text(text.c_str());
    ImGui::SliderInt(
        (std::string("Scale##") + text).c_str(),
        &scale,
        1,
        100,
        "%d%%"
    );

    ImVec2 imgui_image_size(
        static_cast<float>(texture.GetWidth())
            * static_cast<float>(scale) / 100.0f,
        static_cast<float>(texture.GetHeight())
            * static_cast<float>(scale) / 100.0f
    );

    ImGui::Image(
        texture.GetOpenGLId(),
        imgui_image_size
    );

    if (ImGui::BeginItemTooltip())
    {
        float region_size = 40.0f;
        float region_x = io.MousePos.x - cursor_position.x - region_size * 0.5f;
        float region_y = io.MousePos.y - cursor_position.y - region_size * 0.5f;
        float texture_width = imgui_image_size.x;
        float texture_height = imgui_image_size.y;
        float zoom = 4.0f;

        if (region_x < 0.0f)
        {
            region_x = 0.0f;
        }
        else if (region_x > texture_width - region_size)
        {
            region_x = texture_width - region_size;
        }

        if (region_y < 0.0f)
        {
            region_y = 0.0f;
        }
        else if (region_y > texture_height - region_size)
        {
            region_y = texture_height - region_size;
        }

        ImGui::Image(
            texture.GetOpenGLId(),
            ImVec2(
                region_size * zoom,
                region_size * zoom
            ),
            ImVec2(
                (region_x) / texture_width,
                (region_y) / texture_height
            ),
            ImVec2(
                (region_x + region_size) / texture_width,
                (region_y + region_size) / texture_height
            )
        );

        ImGui::EndTooltip();
    }
}

void Gui::BuildOutputUI()
{
    if (ImGui::BeginTable(
            "ImageGrid",
            1,
            ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable
        )
    ) {
        ImVec2 imgui_vector;

        ImGui::TableNextColumn();

        DrawImage(
            output_data.origin_image,
            input_data.origin_image_scale,
            "Origin image"
        );

        if (ImGui::TreeNode("Image info"))
        {
            ImGui::Text(
                "Width: %d\nHeight: %d\nChannels: %d\nSize: %d bytes",
                output_data.origin_image.GetWidth(),
                output_data.origin_image.GetHeight(),
                output_data.origin_image.GetChannels(),
                output_data.origin_image.GetSize()
            );

            ImGui::TreePop();
        }

        ImGui::TableNextColumn();

        DrawImage(
            output_data.fragment_image,
            input_data.fragment_image_scale,
            "Fragment"
        );

        if (ImGui::TreeNode("Fragment info"))
        {
            auto bound = FragmentInfo(
                input_data.fragments_count,
                input_data.selected_fragment
            ).GetBounds();

            ImGui::Text(
                "Lower bound: %d\nUpper bound: %d\nFragment size: %d",
                bound.first,
                bound.second,
                bound.second - bound.first
            );

            ImGui::TreePop();
        }


        ImGui::TableNextColumn();

        if (ImPlot::BeginPlot("Intensity plot")) {

            ImPlot::PushColormap(ImPlotColormap_Deep);
            ImPlot::PlotLine(
                "Blue channel",
                output_data.intensity_graph.x.data(),
                output_data.intensity_graph.b.data(),
                output_data.intensity_graph.x.size()
            );
            ImPlot::PlotLine(
                "All channels",
                output_data.intensity_graph.x.data(),
                output_data.intensity_graph.main.data(),
                output_data.intensity_graph.x.size()
            );
            ImPlot::PlotLine(
                "Green channel",
                output_data.intensity_graph.x.data(),
                output_data.intensity_graph.g.data(),
                output_data.intensity_graph.x.size()
            );
            ImPlot::PlotLine(
                "Red channel",
                output_data.intensity_graph.x.data(),
                output_data.intensity_graph.r.data(),
                output_data.intensity_graph.x.size()
            );
            ImPlot::PopColormap();
            ImPlot::EndPlot();
        }


        ImGui::TableNextColumn();

        if (ImPlot::BeginPlot("Density plot")) {

            ImPlot::PushColormap(ImPlotColormap_Deep);
            ImPlot::PlotLine(
                "Blue channel",
                output_data.density_graph.x.data(),
                output_data.density_graph.b.data(),
                output_data.density_graph.x.size()
            );
            ImPlot::PlotLine(
                "All channels",
                output_data.density_graph.x.data(),
                output_data.density_graph.main.data(),
                output_data.density_graph.x.size()
            );
            ImPlot::PlotLine(
                "Green channel",
                output_data.density_graph.x.data(),
                output_data.density_graph.g.data(),
                output_data.density_graph.x.size()
            );
            ImPlot::PlotLine(
                "Red channel",
                output_data.density_graph.x.data(),
                output_data.density_graph.r.data(),
                output_data.density_graph.x.size()
            );
            ImPlot::PopColormap();
            ImPlot::EndPlot();
        }
        ImGui::EndTable();
    }
}

void Gui::BuildUI()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 work_area = viewport->WorkSize;

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(work_area);
    ImGui::Begin(
        "Main Window",
        nullptr,
        ImGuiWindowFlags_NoDecoration
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoResize
    );

    float left_width = splitter_ratio * work_area.x;
    float right_width = work_area.x - left_width;

    ImGui::BeginChild(
        "InputSection",
        ImVec2(
            left_width - 2 * ImGui::GetStyle().WindowPadding.x,
            work_area.y - ImGui::GetStyle().WindowPadding.y * 2
        ),
        true
    );

    BuildInputUI();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::Button(
        "##Splitter",
        ImVec2(
            8.0f,
            work_area.y - ImGui::GetStyle().WindowPadding.y * 2
        )
    );

    if (ImGui::IsItemActive())
    {
        splitter_ratio += ImGui::GetIO().MouseDelta.x / work_area.x;
        splitter_ratio = std::clamp(splitter_ratio, 0.1f, 0.9f);
    }

    ImGui::SameLine();

    ImGui::BeginChild(
        "OutputSection",
        ImVec2(
            right_width - 2 * ImGui::GetStyle().WindowPadding.x,
            work_area.y - ImGui::GetStyle().WindowPadding.y * 2
        ),
        true
    );
    BuildOutputUI();
    ImGui::EndChild();

    ImGui::End();
}

void Gui::OnRender()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if(has_input_data_changed)
    {
        ValidateInputData();
        RerunFragmentation();
        has_input_data_changed == false;
    }

    BuildUI();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

Gui::~Gui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Gui::SetColorStyle()
{
    ImGuiIO& io = ImGui::GetIO();

    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF(
        Resources::GetAssetFullPath("/fonts/OpenSans-Bold.ttf").c_str(),
        20
    );
    io.Fonts->AddFontFromFileTTF(
        Resources::GetAssetFullPath("/fonts/OpenSans-Regular.ttf").c_str(),
        20
    );
    io.Fonts->AddFontFromFileTTF(
        Resources::GetAssetFullPath("/fonts/OpenSans-Light.ttf").c_str(),
        32
    );
    io.Fonts->Build();

    ImGuiStyle * style = &ImGui::GetStyle();

    style->WindowPadding            = ImVec2(15, 15);
    style->WindowRounding           = 5.0f;
    style->FramePadding             = ImVec2(5, 5);
    style->FrameRounding            = 4.0f;
    style->ItemSpacing              = ImVec2(12, 8);
    style->ItemInnerSpacing         = ImVec2(8, 6);
    style->IndentSpacing            = 25.0f;
    style->ScrollbarSize            = 15.0f;
    style->ScrollbarRounding        = 9.0f;
    style->GrabMinSize              = 5.0f;
    style->GrabRounding             = 3.0f;

    style->Colors[ImGuiCol_Text]                  = ColorStyle::Text;
    style->Colors[ImGuiCol_TextSelectedBg]        = accent_color;
    style->Colors[ImGuiCol_WindowBg]              = ColorStyle::Base;
    style->Colors[ImGuiCol_PopupBg]               = ColorStyle::Surface0;
    style->Colors[ImGuiCol_Border]                = ColorStyle::Surface1;
    style->Colors[ImGuiCol_FrameBg]               = ColorStyle::Surface0;
    style->Colors[ImGuiCol_FrameBgHovered]        = accent_color;
    style->Colors[ImGuiCol_FrameBgActive]         = accent_color;

    // Title
    style->Colors[ImGuiCol_TitleBg]               = ColorStyle::Surface0;
    style->Colors[ImGuiCol_TitleBgCollapsed]      = ColorStyle::Surface1;
    style->Colors[ImGuiCol_TitleBgActive]         = accent_color;


    // Buttons
    style->Colors[ImGuiCol_Button]                = ColorStyle::Surface0;
    style->Colors[ImGuiCol_ButtonHovered]         = accent_color;
    style->Colors[ImGuiCol_ButtonActive]          = accent_color;

    // Headers
    style->Colors[ImGuiCol_Header]                = accent_color;
    style->Colors[ImGuiCol_HeaderHovered]         = accent_color;
    style->Colors[ImGuiCol_HeaderActive]          = accent_color;

    // Scrollbar
    style->Colors[ImGuiCol_ScrollbarBg]           = ColorStyle::Surface1;
    style->Colors[ImGuiCol_ScrollbarGrab]         = ColorStyle::Surface0;
    style->Colors[ImGuiCol_ScrollbarGrabHovered]  = accent_color;
    style->Colors[ImGuiCol_ScrollbarGrabActive]   = accent_color;

    // Resize grip
    style->Colors[ImGuiCol_ResizeGrip]            = ColorStyle::Surface1;
    style->Colors[ImGuiCol_ResizeGripHovered]     = accent_color;
    style->Colors[ImGuiCol_ResizeGripActive]      = accent_color;

    // Checkbox and Slider
    style->Colors[ImGuiCol_CheckMark]             = accent_color;
    style->Colors[ImGuiCol_SliderGrab]            = accent_color;
    style->Colors[ImGuiCol_SliderGrabActive]      = ColorStyle::Base;
}
