#ifndef OPENGL_CONTEXT_H_
#define OPENGL_CONTEXT_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class OpenGLContext{
private:
    GLFWwindow* glfw_window;

public:
    OpenGLContext();
    ~OpenGLContext();

    GLFWwindow* GetWindow() const;
    void PollEvents();
    void SwapBuffer();
    void Clear();
    bool ShouldClose() const;
};


#endif