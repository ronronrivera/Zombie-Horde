#include "engine/Window.hpp"

#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>

Window::Window (int width, int height, const std::string &title):
    m_width(width), m_height(height)     
{
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
        throw std::runtime_error("Failed to initialise GLFW");

    // request OpenGL 3.3 Core — no deprecated features
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_handle) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    // bind the GL context to this thread
    glfwMakeContextCurrent(m_handle);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        glfwTerminate();

        throw std::runtime_error("Failed to initialize GLAD");
    }

    glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetFramebufferSizeCallback(m_handle, framebufferResizeCallback);

    glfwSwapInterval(1);

    glViewport(0, 0, width, height);

    std::cout << "OPENGL VERSION: " << glGetString(GL_VERSION) << "\n";
    std::cout << "GPU: " << glGetString(GL_RENDERER) <<  "\n";
}


Window::~Window(){
    if(m_handle){
        glfwDestroyWindow(m_handle);
    }

    glfwTerminate();
}


bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_handle);
}


void Window::swapBuffers(){
    glfwSwapBuffers(m_handle);
} 


void Window::pollEvents() {
    glfwPollEvents();
}

void Window::framebufferResizeCallback(GLFWwindow*, int w, int h) {
    glViewport(0, 0, w, h);
}

void Window::errorCallback(int error, const char *desc){
    std::cerr << "GLFW error " << error << ": " << desc << "\n";
}
