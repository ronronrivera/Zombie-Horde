#version 330 core

layout(location = 0) in vec3  aPos;
layout(location = 1) in vec2  aTexCoord;
layout(location = 2) in vec3  aNormal;
layout(location = 3) in ivec4 aBoneIDs;
layout(location = 4) in vec4  aBoneWeights;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

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

    mat3 skinMat      = mat3(skin);
    vec3 skinnedNorm  = normalize(skinMat * aNormal);
    mat3 normalMatrix = mat3(transpose(inverse(uModel)));

    vec4 worldPos = uModel * skinnedPos;

    gl_Position = uProjection * uView * worldPos;
    TexCoord    = aTexCoord;
    FragPos     = worldPos.xyz;
    Normal      = normalize(normalMatrix * skinnedNorm);
}
