#include <iostream>
#include <utility>
#include <string>
#include "opengl_context.h"
#include "model/fragment_cutter.h"
#include "model/opencl_fragment_cutter.h"
#include "model/fragmentizer.h"
#include "view/gui.h"
#include "controller/fragmentizer_controller.h"
#include "resources.h"

#include <csignal>
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

namespace Signal
{
    volatile std::sig_atomic_t gGotSigInt;
}

void SigIntHandler(int signal)
{
    Signal::gGotSigInt = signal;
}


int main() {
    std::signal(SIGINT, SigIntHandler);

    OpenGLContext opengl_context;

    Image no_image_image(Resources::GetAssetFullPath("/noimage.png"));

    FragmentCutter cpu_fragment_cutter = FragmentCutter();
    OpenCLFragmentCutter opencl_fragment_cutter = OpenCLFragmentCutter(
        Resources::GetAssetFullPath("/fragment_cutter_kernel.cl")
    );

    Fragmentizer fragmentizer(std::move(no_image_image));
    fragmentizer.AddFragmentCutter(cpu_fragment_cutter);
    fragmentizer.AddFragmentCutter(opencl_fragment_cutter);

    std::map<std::string, int> fragment_cutters_map;
    fragment_cutters_map.insert({"Cpu", 0});
    fragment_cutters_map.insert({"OpenCL", 1});

    FragmentizerController controller(fragmentizer, fragment_cutters_map);

    Gui gui(opengl_context, controller);

    while (!opengl_context.ShouldClose() && !Signal::gGotSigInt)
    {
        opengl_context.SwapBuffer();
        opengl_context.PollEvents();
        opengl_context.Clear();

        gui.OnRender();
    }

    return 0;
}
