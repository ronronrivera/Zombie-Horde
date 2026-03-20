#include "engine/Input.hpp"
#include <GLFW/glfw3.h>
#include <cstring>

bool Input::s_currentKeys[GLFW_KEY_LAST] = {};
bool Input::s_previousKeys[GLFW_KEY_LAST] = {};

float Input::s_mouseDX      = 0.0f;
float Input::s_mouseDY      = 0.0f;
float Input::s_lastX        = 0.0f;
float Input::s_lastY        = 0.0f;
bool Input::s_firstMouse    = true;

void Input::init(GLFWwindow *window){
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorCallback);

}

void Input::update(){
    
    // snapshot current → previous so isKeyPressed works
    std::memcpy(s_previousKeys, s_currentKeys, sizeof(s_currentKeys));
    
    // reset mouse delta each frame — accumulates fresh in the callback
    s_mouseDX = 0.0f;
    s_mouseDY = 0.0f;
}

bool Input::isKeyHeld(int key){
    return s_currentKeys[key];
}

bool Input::isKeyPressed(int key){
    // true only on the frame the key went down
    return s_currentKeys[key] && !s_previousKeys[key];
}

float Input::getMouseDX() { return s_mouseDX; }
float Input::getMouseDY() { return s_mouseDY; }


void Input::keyCallback(GLFWwindow*, int key, int, int action, int) {
    if (key < 0 || key >= GLFW_KEY_LAST) return;
    if (action == GLFW_PRESS)   s_currentKeys[key] = true;
    if (action == GLFW_RELEASE) s_currentKeys[key] = false;
}

void Input::cursorCallback(GLFWwindow*, double x, double y) {
    if (s_firstMouse) {
        s_lastX = (float)x;
        s_lastY = (float)y;
        s_firstMouse = false;
    }
    s_mouseDX += (float)x - s_lastX;
    s_mouseDY += s_lastY - (float)y; // inverted: y goes up in 3D
    s_lastX = (float)x;
    s_lastY = (float)y;
}
