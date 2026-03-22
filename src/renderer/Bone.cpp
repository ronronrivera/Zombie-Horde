#include "renderer/Bone.hpp"
#include <glm/gtx/quaternion.hpp>

template <typename T>
int BoneAnimation::findIndex(const std::vector<T>& keys, float t) {

    for (int i = 0; i < (int)keys.size() - 1; i++)
        if (t < keys[i+1].time) return i;
    return (int)keys.size() - 2;

}

glm::vec3 BoneAnimation::interpolatePos(float t) const {
    if (positions.size() == 1) return positions[0].value;
    int i = findIndex(positions, t);
    float dt = positions[i+1].time - positions[i].time;
    float f  = (t - positions[i].time) / dt;
    return glm::mix(positions[i].value, positions[i+1].value, f);
}

glm::quat BoneAnimation::interpolateRot(float t) const {
    if (rotations.size() == 1) return rotations[0].value;
    int i = findIndex(rotations, t);
    float dt = rotations[i+1].time - rotations[i].time;
    float f  = (t - rotations[i].time) / dt;
    // slerp for smooth quaternion interpolation — no gimbal lock
    return glm::normalize(glm::slerp(rotations[i].value, rotations[i+1].value, f));
}

glm::vec3 BoneAnimation::interpolateScale(float t) const {
    if (scales.size() == 1) return scales[0].value;
    int i = findIndex(scales, t);
    float dt = scales[i+1].time - scales[i].time;
    float f  = (t - scales[i].time) / dt;
    return glm::mix(scales[i].value, scales[i+1].value, f);
}
