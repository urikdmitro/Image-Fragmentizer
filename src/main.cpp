#include <iostream>
#include <utility>
#include <string>
#include "opengl_context.h"
#include "model/fragmentizer.h"
#include "view/gui.h"
#include "controller/fragmentizer_controller.h"
#include "resources.h"

#include <csignal>

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
    Fragmentizer fragmentizer(std::move(no_image_image));
    FragmentizerController controller(fragmentizer);
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
