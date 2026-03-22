#pragma once
#include <glm/glm.hpp>


class Camera {
public:
    Camera(glm::vec3 startPos = glm::vec3(0.0f, 0.0f, 3.0f));

    void update(float dt);          // reads Input, moves camera
    void onResize(int w, int h);    // update aspect ratio

    // shaders need these every frame
    glm::mat4 getViewMatrix()       const;
    glm::mat4 getProjectionMatrix() const;

    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getFront()    const { return m_front; }
    
    glm::vec3 returnPosition() const {return m_position;}
    glm::vec3 returnM_right() const {return m_right;}
    glm::vec3 returnM_up() const {return m_up;}

    void setPosition(const glm::vec3& pos) { m_position = pos; }

    float getCurrentSpeed()  const { return m_currentSpeed; }
    bool  isSprinting()      const { return m_isSprinting; }
    bool isReloading() const {return m_isReloading;}

private:
    void updateVectors();   // recalculate front/right/up from yaw+pitch
    
    float m_currentSpeed = 0.0f;
    bool  m_isSprinting  = false;
    bool m_isReloading = false;
    static constexpr float SPRINT_MULTIPLIER = 1.9f;

    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_right;
    glm::vec3 m_up;

    float m_yaw         = -90.0f;  // -90 so we start facing -Z (into screen)
    float m_pitch       =   0.0f;

    float m_moveSpeed   =   3.0f;  // units per second
    float m_mouseSens   =   0.1f;  // degrees per pixel
    float m_fov         =  75.0f;  // degrees
    float m_aspectRatio =  16.0f / 9.0f;

    static constexpr float NEAR_PLANE =   0.1f;
    static constexpr float FAR_PLANE  = 100.0f;
    static constexpr float MAX_PITCH  =  89.0f; // prevent gimbal lock
};
