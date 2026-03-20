
#include "engine/Camera.hpp"
#include "engine/Input.hpp"
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>

Camera::Camera(glm::vec3 startPos):
    m_position(startPos),
    m_front (0.0f, 0.0f, -1.0f),
    m_right (1.0f, 0.0f, 0.0f),
    m_up    (0.0f, 1.0f, 0.0f)
{
    updateVectors();
}

void Camera::update(float dt){
    //mouse look
    m_yaw   += Input::getMouseDX() * m_mouseSens;
    m_pitch += Input::getMouseDY() * m_mouseSens; 

    //clamps pitch so camera never flips upside down
    m_pitch = std::clamp(m_pitch, -MAX_PITCH, MAX_PITCH);
    
    updateVectors();

    // WASD movement 
    // move along the flat ground plane — ignore Y component of front
    // so WASD never floats the player up or down
    
    glm::vec3 flatFront = glm::normalize(
        glm::vec3(m_front.x, 0.0f, m_front.z)
    );
    glm::vec3 flatRight = glm::normalize(
        glm::vec3(m_right.x, 0.0f, m_right.z)
    );

    float speed = m_moveSpeed * dt;
    
    if (Input::isKeyHeld(GLFW_KEY_W)) m_position += flatFront * speed;
    if (Input::isKeyHeld(GLFW_KEY_S)) m_position -= flatFront * speed;
    if (Input::isKeyHeld(GLFW_KEY_A)) m_position -= flatRight * speed;
    if (Input::isKeyHeld(GLFW_KEY_D)) m_position += flatRight * speed;

}

void Camera::onResize(int w, int h){
    if(h>0){
        m_aspectRatio = (float)w/(float)h;
    }
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(
        glm::radians(m_fov),
        m_aspectRatio,
        NEAR_PLANE,
        FAR_PLANE
    );
}

void Camera::updateVectors() {
    // convert yaw + pitch (degrees) → front vector
    float yawR   = glm::radians(m_yaw);
    float pitchR = glm::radians(m_pitch);

    m_front = glm::normalize(glm::vec3(
        glm::cos(yawR) * glm::cos(pitchR),
        glm::sin(pitchR),
        glm::sin(yawR) * glm::cos(pitchR)
    ));

    // right and up follow from front
    m_right = glm::normalize(glm::cross(m_front, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_up    = glm::normalize(glm::cross(m_right, m_front));
}
