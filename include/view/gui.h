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

        ImVec2 origin_image_size;
        ImVec2 fragment_image_size;
        ImVec2 variance_graph_size;
        ImVec2 intensity_graph_size;
    };

    struct OutputData
    {
        Texture origin_image;
        Texture fragment_image;
        Texture variance_graph;
        Texture intensity_graph;
    };

    const OpenGLContext &opengl_context;
    const FragmentizerController &controller;
    ImVec4 accent_color;

    InputData poll_input_helper_input_data;
    InputData input_data;
    OutputData output_data;

    // int ShouldRedraw();

    std::string OpenFilePicker();
    void BuildUI();
    void BuildInputUI();
    void BuildOutputUI();
    void SelectImageButtonCallback();
    void RerunFragmentationButtonCallback();

    void SetColorStyle();

public:
    Gui(const OpenGLContext &opengl_context, const FragmentizerController &controller);
    ~Gui();

    void OnRender();
};

#endif