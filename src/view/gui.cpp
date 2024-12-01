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

const float kFloatEpsilon = 0.00001f;

Gui::Gui(
    const OpenGLContext &opengl_context,
    FragmentizerController &controller
)
    : opengl_context(opengl_context)
    , controller(controller)
    , accent_color(ColorStyle::Mauve)
    , validated_input_data(
        InputData {
            .fragments_count = 10,
            .selected_fragment = 5,
            .fragmentize_red_channel   = true,
            .fragmentize_green_channel = true,
            .fragmentize_blue_channel  = true,
            .nonfragment_pixel_value  = 255,
            .origin_image_size      = ImVec2(300.0f, 300.0f),
            .fragment_image_size    = ImVec2(300.0f, 300.0f),
            .variance_graph_size    = ImVec2(300.0f, 300.0f),
            .intensity_graph_size   = ImVec2(300.0f, 300.0f),
        }
    )
    , input_data(validated_input_data)
    , output_data(
        OutputData {
            .origin_image = controller.GetImage(),
            .fragment_image = controller.GetImage(),
            .variance_graph = controller.GetImage(),
            .intensity_graph = controller.GetImage(),
        }
    )
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(opengl_context.GetWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();
    SetColorStyle();
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
        validated_input_data.fragments_count,
        validated_input_data.selected_fragment,
        mask,
        input_data.nonfragment_pixel_value
    );
}

void Gui::SelectImageButtonCallback()
{
    validated_input_data.path_to_image = OpenFilePicker();

    controller.SetNewImage(validated_input_data.path_to_image);

    output_data.origin_image = controller.GetImage();

    UpdateFragment();
}

void Gui::RerunFragmentationButtonCallback()
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

bool Gui::InputData::operator==(const InputData &rhs)
{
    return (
        this->fragments_count           == rhs.fragments_count              &&
        this->selected_fragment         == rhs.selected_fragment            &&
        this->fragmentize_red_channel   == rhs.fragmentize_red_channel      &&
        this->fragmentize_green_channel == rhs.fragmentize_green_channel    &&
        this->fragmentize_blue_channel  == rhs.fragmentize_blue_channel     &&
        this->nonfragment_pixel_value   == rhs.nonfragment_pixel_value
        // this->path_to_image        == rhs.path_to_image       &&
        // this->origin_image_size    == rhs.origin_image_size   &&
        // this->fragment_image_size  == rhs.fragment_image_size &&
        // this->intensity_graph_size == rhs.intensity_graph_size&&
        // this->variance_graph_size  == rhs.variance_graph_size
    );
}

bool Gui::InputData::operator!=(const InputData &rhs)
{
    return !(*this == rhs);
}

bool Gui::HasInputDataChanged()
{
    return input_data != validated_input_data;
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
    if (ImGui::BeginTable(
            "ImageGrid",
            2,
            ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable
        )
    ) {
        ImGui::TableNextColumn();

        if (ImGui::Button("Select Image"))
        {
            SelectImageButtonCallback();
        }

        ImGui::Text("Selected Image: %s", input_data.path_to_image.c_str());
        ImGui::InputInt(
            "Selected Fragment",
            &(input_data.selected_fragment),
            1,
            10
        );
        ImGui::InputInt(
            "Number of Fragments",
            &(input_data.fragments_count),
            1,
            10
        );

        if (ImGui::Button("Rerun"))
        {
            RerunFragmentationButtonCallback();
        }

        ImGui::TableNextColumn();

        ImGui::Checkbox("Red",      &(input_data.fragmentize_red_channel));
        ImGui::Checkbox("Green",    &(input_data.fragmentize_green_channel));
        ImGui::Checkbox("Blue",     &(input_data.fragmentize_blue_channel));

        ImGui::InputInt(
            "Nonfragment value",
            &(input_data.nonfragment_pixel_value),
            255,
            255
        );

        ImGui::EndTable();
    }
}

void Gui::BuildOutputUI()
{
    if (ImGui::BeginTable(
            "ImageGrid",
            2,
            ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable
        )
    ) {
        ImGui::TableNextColumn();
        ImGui::Text("Origin image");
        ImGui::DragFloat2(
            ("Scale##" + std::to_string(1)).c_str(),
            reinterpret_cast<float *>(&(validated_input_data.origin_image_size.x)),
            1.0f,
            50.0f,
            500.0f,
            "%.0f"
        );
        ImGui::Image(
            output_data.origin_image.GetOpenGLId(),
            validated_input_data.origin_image_size
        );

        ImGui::TableNextColumn();
        ImGui::Text("Fragment");
        ImGui::DragFloat2(
            ("Scale##" + std::to_string(2)).c_str(),
            reinterpret_cast<float *>(&(validated_input_data.fragment_image_size.x)),
            1.0f,
            50.0f,
            500.0f,
            "%.0f"
        );
        ImGui::Image(
            output_data.fragment_image.GetOpenGLId(),
            validated_input_data.fragment_image_size
        );

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

    static float splitter_ratio = 0.3f;

    float top_height = splitter_ratio * work_area.y;
    float bottom_height = work_area.y - top_height;

    ImGui::BeginChild("InputSection", ImVec2(work_area.x, top_height), true);
    BuildInputUI();
    ImGui::EndChild();

    ImGui::InvisibleButton("##Splitter", ImVec2(work_area.x, 8.0f));
    if (ImGui::IsItemActive())
    {
        splitter_ratio += ImGui::GetIO().MouseDelta.y / work_area.y;
        splitter_ratio = std::clamp(splitter_ratio, 0.1f, 0.9f);
    }

    ImGui::BeginChild(
        "OutputSection",
        ImVec2(work_area.x, bottom_height),
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

    if(HasInputDataChanged())
    {
        ValidateInputData();
        if(HasInputDataChanged())
        {
            validated_input_data = input_data;
            RerunFragmentationButtonCallback();
        }
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
        24
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
    style->Colors[ImGuiCol_PopupBg]               = ColorStyle::Surface2;
    style->Colors[ImGuiCol_Border]                = ColorStyle::Surface1;
    style->Colors[ImGuiCol_FrameBg]               = ColorStyle::Surface2;
    style->Colors[ImGuiCol_FrameBgHovered]        = accent_color;
    style->Colors[ImGuiCol_FrameBgActive]         = accent_color;

    // Title
    style->Colors[ImGuiCol_TitleBg]               = ColorStyle::Surface2;
    style->Colors[ImGuiCol_TitleBgCollapsed]      = ColorStyle::Surface1;
    style->Colors[ImGuiCol_TitleBgActive]         = accent_color;


    // Buttons
    style->Colors[ImGuiCol_Button]                = ColorStyle::Surface2;
    style->Colors[ImGuiCol_ButtonHovered]         = accent_color;
    style->Colors[ImGuiCol_ButtonActive]          = accent_color;

    // Headers
    style->Colors[ImGuiCol_Header]                = accent_color;
    style->Colors[ImGuiCol_HeaderHovered]         = accent_color;
    style->Colors[ImGuiCol_HeaderActive]          = accent_color;

    // Scrollbar
    style->Colors[ImGuiCol_ScrollbarBg]           = ColorStyle::Surface1;
    style->Colors[ImGuiCol_ScrollbarGrab]         = ColorStyle::Surface2;
    style->Colors[ImGuiCol_ScrollbarGrabHovered]  = accent_color;
    style->Colors[ImGuiCol_ScrollbarGrabActive]   = accent_color;

    // Resize grip
    style->Colors[ImGuiCol_ResizeGrip]            = ColorStyle::Surface1;
    style->Colors[ImGuiCol_ResizeGripHovered]     = accent_color;
    style->Colors[ImGuiCol_ResizeGripActive]      = accent_color;

    // Checkbox and Slider
    style->Colors[ImGuiCol_CheckMark]             = accent_color;
    style->Colors[ImGuiCol_SliderGrab]            = ColorStyle::Surface2;
    style->Colors[ImGuiCol_SliderGrabActive]      = accent_color;
}
