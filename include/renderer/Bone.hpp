#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

// one keyframe for position, rotation, or scale
struct KeyVec3 {float time; glm::vec3 value;};
struct KeyQuat {float time; glm::quat value;};

// all keyframes for one bone in one animation
struct BoneAnimation {

    std::string name;
    std::vector<KeyVec3> positions;
    std::vector<KeyQuat> rotations;
    std::vector<KeyVec3> scales;

    glm::vec3 interpolatePos  (float t) const;
    glm::quat interpolateRot  (float t) const;
    glm::vec3 interpolateScale(float t) const;

private:
    template<typename T>
    static int findIndex(const std::vector<T>& keys, float t);
};
