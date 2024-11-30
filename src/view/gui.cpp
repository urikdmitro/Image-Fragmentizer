#include "view/gui.h"
#include <iostream>
#include <nfd.h>
#include <string>
#include "resources.h"
#include "view/color_style.h"
#include <algorithm>

Gui::Gui(const OpenGLContext &opengl_context, const FragmentizerController &controller)
    : opengl_context(opengl_context)
    , controller(controller)
    , accent_color(ColorStyle::Mauve)
    , input_data(
        InputData
        {
            .fragments_count = 10,
            .selected_fragment = 5,
            .origin_image_size      = ImVec2(300.0f, 300.0f),
            .fragment_image_size    = ImVec2(300.0f, 300.0f),
            .variance_graph_size    = ImVec2(300.0f, 300.0f),
            .intensity_graph_size   = ImVec2(300.0f, 300.0f),
        }
    )
    , output_data(
        OutputData
        {
            .origin_image = controller.GetCurrentImage(),
            .fragment_image = controller.GetCurrentImage(),
            .variance_graph = controller.GetCurrentImage(),
            .intensity_graph = controller.GetCurrentImage(),
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

void Gui::SelectImageButtonCallback()
{
    input_data.path_to_image = OpenFilePicker();
    output_data.origin_image = controller.GetCurrentImage();
}

void Gui::RerunFragmentationButtonCallback()
{
    output_data.origin_image = controller.GetCurrentImage();
}


void Gui::BuildInputUI()
{
    if (ImGui::Button("Select Image"))
    {
        SelectImageButtonCallback();
    }

    ImGui::Text("Selected Image: %s", input_data.path_to_image.c_str());
    ImGui::InputInt("Selected Fragment", &(input_data.selected_fragment), 1, 256);
    ImGui::InputInt("Number of Fragments", &(input_data.fragments_count), 1, 256);

    if (ImGui::Button("Rerun"))
    {
        RerunFragmentationButtonCallback();
    }
}

void Gui::BuildOutputUI()
{
    if (ImGui::BeginTable("ImageGrid", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
    {
        ImGui::TableNextColumn();

        ImGui::Text("Origin image");
        ImGui::DragFloat2(("Scale##" + std::to_string(1)).c_str(),
                           reinterpret_cast<float *>(&(input_data.origin_image_size.x)),
                           1.0f, 50.0f, 500.0f, "%.0f"
                           );

        ImGui::Image(output_data.origin_image.GetOpenGLId(),
                     input_data.origin_image_size);

        ImGui::EndTable();
    }
}

void Gui::BuildUI()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 work_area = viewport->WorkSize;

    // Start the main application window
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(work_area);
    ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    static float splitter_ratio = 0.3f; // Top section takes 30% by default

    // Calculate heights
    float top_height = splitter_ratio * work_area.y;
    float bottom_height = work_area.y - top_height;

    // Top section (User Input)
    ImGui::BeginChild("InputSection", ImVec2(work_area.x, top_height), true);
    BuildInputUI();
    ImGui::EndChild();

    // Vertical splitter (resizable by dragging)
    ImGui::InvisibleButton("##Splitter", ImVec2(work_area.x, 8.0f));
    if (ImGui::IsItemActive())
    {
        splitter_ratio += ImGui::GetIO().MouseDelta.y / work_area.y;

        // Clamp the splitter ratio between 10% and 90%
        splitter_ratio = std::clamp(splitter_ratio, 0.1f, 0.9f);
    }

    // Bottom section (Output)
    ImGui::BeginChild("OutputSection", ImVec2(work_area.x, bottom_height), true);
    BuildOutputUI();
    ImGui::EndChild();

    ImGui::End();
}

void Gui::OnRender()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

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
    // io.Fonts->AddFontFromFileTTF(Resources::GetAssetFullPath("/fonts/OpenSans-Light.ttf").c_str(),   16);
    // io.Fonts->AddFontFromFileTTF(Resources::GetAssetFullPath("/fonts/OpenSans-Regular.ttf").c_str(), 16);
    // io.Fonts->AddFontFromFileTTF(Resources::GetAssetFullPath("/fonts/OpenSans-Regular.ttf").c_str(), 11);
    io.Fonts->AddFontFromFileTTF(Resources::GetAssetFullPath("/fonts/OpenSans-Bold.ttf").c_str(),    24);
    io.Fonts->AddFontFromFileTTF(Resources::GetAssetFullPath("/fonts/OpenSans-Regular.ttf").c_str(), 20);
    io.Fonts->AddFontFromFileTTF(Resources::GetAssetFullPath("/fonts/OpenSans-Light.ttf").c_str(),   32);
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
