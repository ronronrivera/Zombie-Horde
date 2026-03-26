#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
public:
    Window (int width, int height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();
    void setVSync(int interval);

    int getWidth() {return m_width;}
    int getHeight() {return m_height;}
    
    GLFWwindow * getHandle() const {return m_handle;}

private:
    GLFWwindow *m_handle;
    int m_width, m_height;

    static void framebufferResizeCallback(GLFWwindow *win, int w, int h);
    static void errorCallback(int error, const char* desc);

};


