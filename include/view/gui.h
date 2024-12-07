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

    struct InputData
    {
        int fragments_count;
        int selected_fragment;
        std::string path_to_image;

        bool fragmentize_red_channel;
        bool fragmentize_green_channel;
        bool fragmentize_blue_channel;
        int nonfragment_pixel_value;

        ImVec2 origin_image_size;
        ImVec2 fragment_image_size;
        ImVec2 variance_graph_size;
        ImVec2 intensity_graph_size;

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
        Texture intensity_graph;
    };

    const OpenGLContext &opengl_context;
    FragmentizerController &controller;
    ImVec4 accent_color;

    InputData validated_input_data;
    InputData input_data;
    OutputData output_data;

    // int ShouldRedraw();

    std::string OpenFilePicker();
    void BuildUI();
    void BuildInputUI();
    void BuildOutputUI();
    void SelectImageButtonCallback();
    void RerunFragmentationButtonCallback();

    void UpdateFragment();

    void SetColorStyle();
    bool HasInputDataChanged();
    void ValidateInputData();

public:
    Gui(
        const OpenGLContext &opengl_context,
        FragmentizerController &controller
    );
    ~Gui();

    void OnRender();
};

#endif