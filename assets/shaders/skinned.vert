#version 330 core

layout(location = 0) in vec3  aPos;
layout(location = 1) in vec2  aTexCoord;
layout(location = 2) in vec3  aNormal;
layout(location = 3) in ivec4 aBoneIDs;
layout(location = 4) in vec4  aBoneWeights;

out vec2 TexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uBones[100];

void main() {
    // blend up to 4 bone transforms weighted by influence
    mat4 skin = uBones[aBoneIDs.x] * aBoneWeights.x
              + uBones[aBoneIDs.y] * aBoneWeights.y
              + uBones[aBoneIDs.z] * aBoneWeights.z
              + uBones[aBoneIDs.w] * aBoneWeights.w;

    // if no bones influence this vertex, skin = identity
    vec4 skinnedPos = skin * vec4(aPos, 1.0);

    gl_Position = uProjection * uView * uModel * skinnedPos;
    TexCoord    = aTexCoord;
}
