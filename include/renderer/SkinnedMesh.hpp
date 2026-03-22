#pragma once
#include "renderer/Bone.hpp"
#include "renderer/Shader.hpp"
#include "renderer/Texture.hpp"
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <assimp/Importer.hpp>

// one vertex with skinning data
struct SkinnedVertex {
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
    glm::ivec4 boneIDs     { -1,-1,-1,-1 };
    glm::vec4  boneWeights { 0,0,0,0 };
};

// one animation clip (e.g. "idle", "walk", "sprint")
struct AnimClip {
    std::string name;
    float       duration;   // in ticks
    float       ticksPerSec;
    std::unordered_map<std::string, BoneAnimation> boneAnims;
};

struct BoneInfo {
    int       id;
    glm::mat4 offsetMatrix; // transforms from mesh space → bone space
};

struct SubMesh {
    GLsizei indexCount  = 0;
    GLuint  indexOffset = 0;  // byte offset into EBO
    int     materialIdx = 0;
};

class SkinnedMesh {
public:
    SkinnedMesh(const std::string& path);
    ~SkinnedMesh();

    // update bone matrices for current animation and time
    void update(float dt);

    // upload bone matrices and draw
    void draw(Shader& shader) const;

    // switch animation by name ("idle", "walk", "sprint" etc.)
    void playAnimation(const std::string& name, bool loop = true);

    // list available animation names (useful for debugging)
    std::vector<std::string> getAnimationNames() const;

private:
    Assimp::Importer m_importer;

    const aiScene* m_scene = nullptr;

    std::vector<SubMesh> m_subMeshes;

    void loadMesh(const aiScene* scene);
    void loadAnimations(const aiScene* scene);
    void processBoneWeights(aiMesh* mesh,
                            std::vector<SkinnedVertex>& verts);
    void calcBoneMatrices(const aiNode* node,
                          const glm::mat4& parentTransform);

    // GPU buffers
    GLuint m_vao = 0, m_vbo = 0, m_ebo = 0;
    GLsizei m_indexCount = 0;

    // skeleton
    std::unordered_map<std::string, BoneInfo> m_boneMap;
    std::vector<glm::mat4> m_boneMatrices; // uploaded to shader every frame
    glm::mat4 m_globalInverse;             // inverse of root node transform

    // animations
    std::vector<AnimClip>  m_clips;
    int                    m_currentClip = 0;
    float                  m_animTime    = 0.0f;
    bool                   m_loop        = true;

    // texture

    std::vector<std::unique_ptr<Texture>> m_textures;
    std::vector<int> m_materialTexture; // material index → texture index

    // assimp helper
    static glm::mat4 toGLM(const aiMatrix4x4& m);

    static constexpr int MAX_BONES = 100;
};
