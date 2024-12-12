#ifndef GUI_H_
#define GUI_H_

#include "controller/fragmentizer_controller.h"

#include "opengl_context.h"
#include "texture.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class Gui
{
private:
    float splitter_ratio;
    struct InputData
    {
        int fragments_count;
        int selected_fragment;
        std::string path_to_image;

        bool fragmentize_red_channel;
        bool fragmentize_green_channel;
        bool fragmentize_blue_channel;
        int nonfragment_pixel_value;

        int origin_image_scale;
        int fragment_image_scale;
        int variance_graph_scale;
        int intensity_graph_scale;

        int selected_calculation_method;
        std::vector<std::string> calculation_methods;

        inline bool operator==(const InputData &rhs);
        inline bool operator!=(const InputData &rhs);
    };

    struct OutputData
    {
        Texture origin_image;
        Texture fragment_image;
        Texture variance_graph;
        IntensityGraph intensity_graph;
        IntensityGraph density_graph;
    };

    // ImU32 color_data_black[3] = { 255, 255, 255};
    // ImU32 color_data_red[3]   = { 255,   0,   0};
    // ImU32 color_data_green[3] = {   0, 255,   0};
    // ImU32 color_data_blue[3]  = {   0,   0, 255};
    int colormap_black;
    int colormap_red;
    int colormap_green;
    int colormap_blue;

    const OpenGLContext &opengl_context;
    FragmentizerController &controller;
    ImVec4 accent_color;

    InputData input_data;
    OutputData output_data;

    bool has_input_data_changed;
    // int ShouldRedraw();

    std::string OpenFilePicker();
    std::string OpenFilePickerForSave();
    void BuildUI();
    void BuildInputUI();
    void BuildOutputUI();
    void SelectImage();
    void RerunFragmentation();
    void DrawImage(const Texture& texture, int &scale, const std::string &text);

    void UpdateFragment();
    void SaveFragment();

    void SetColorStyle();
    void ValidateInputData();

    void AdjustImageScale();

public:
    Gui(
        const OpenGLContext &opengl_context,
        FragmentizerController &controller
    );
    ~Gui();

    void OnRender();
};

#endif