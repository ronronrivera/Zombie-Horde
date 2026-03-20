
#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Input {

public:

    static void init(GLFWwindow *window);
    
    static void update();

    //keyboard
    static bool isKeyHeld(int glfwKey);
    static bool isKeyPressed (int glfwKey); // true only on first frame
    
    //mouse
    static float getMouseDX(); //delta x since last frame
    static float getMouseDY(); //delta y since last frame

private:
    
    static void keyCallback(GLFWwindow *, int key, int, int action, int);
    static void cursorCallback(GLFWwindow *, double x, double y);

    static bool s_currentKeys[GLFW_KEY_LAST];
    static bool s_previousKeys[GLFW_KEY_LAST];
    static float s_mouseDX, s_mouseDY;
    static float s_lastX, s_lastY;
    static bool s_firstMouse;
};
