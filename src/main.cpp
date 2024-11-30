#include <iostream>
#include <string>
#include "opengl_context.h"
#include "model/fragmentizer.h"
#include "view/gui.h"
#include "controller/fragmentizer_controller.h"
#include "resources.h"


int main() {
    OpenGLContext opengl_context;
    Image no_image_image(Resources::GetAssetFullPath("/noimage.png"));
    Fragmentizer fragmentizer(no_image_image);
    FragmentizerController controller(fragmentizer);
    Gui gui(opengl_context, controller);

    while (!opengl_context.ShouldClose())
    {
        opengl_context.SwapBuffer();
        opengl_context.PollEvents();
        opengl_context.Clear();

        gui.OnRender();
    }
    return 0;
}
