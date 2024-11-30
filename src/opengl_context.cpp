#include "opengl_context.h"
#include <iostream>

OpenGLContext::OpenGLContext()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->glfw_window = glfwCreateWindow(
        1024,
        720,
        "Image Segmentation Tool",
        nullptr,
        nullptr
    );

    if (!this->glfw_window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(this->glfw_window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW\n";
        return;
    }
}

OpenGLContext::~OpenGLContext()
{
    glfwDestroyWindow(this->glfw_window);
    glfwTerminate();
}

GLFWwindow *OpenGLContext::GetWindow() const
{
    return this->glfw_window;
}

void OpenGLContext::PollEvents()
{
    glfwPollEvents();
}

void OpenGLContext::SwapBuffer()
{
    glfwSwapBuffers(this->glfw_window);
}

void OpenGLContext::Clear()
{
    int display_w, display_h;

    glfwGetFramebufferSize(this->glfw_window, &display_w, &display_h);

    glViewport(0, 0, display_w, display_h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

bool OpenGLContext::ShouldClose() const
{
    return glfwWindowShouldClose(this->glfw_window);
}
