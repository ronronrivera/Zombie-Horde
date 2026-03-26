#include "renderer/SkinnedMesh.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <stdexcept>
#include <iostream>

SkinnedMesh::SkinnedMesh(const std::string& path) {
    m_scene = m_importer.ReadFile(path,
        aiProcess_Triangulate      |
        aiProcess_GenNormals       |
        aiProcess_FlipUVs          |
        aiProcess_LimitBoneWeights |
        aiProcess_CalcTangentSpace
    );

    if (!m_scene || !m_scene->mRootNode ||
        m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
        throw std::runtime_error("Assimp: " +
            std::string(m_importer.GetErrorString()));

    m_globalInverse = glm::inverse(toGLM(m_scene->mRootNode->mTransformation));
    m_boneMatrices.resize(MAX_BONES, glm::mat4(1.0f));

    loadMesh(m_scene);
    loadAnimations(m_scene);

    // load all embedded textures (GLB stores them as binary blobs)
    m_textures.resize(m_scene->mNumTextures);
    for (unsigned int i = 0; i < m_scene->mNumTextures; i++) {
        aiTexture* tex = m_scene->mTextures[i];
        if (tex->mHeight == 0) {
            // compressed PNG/JPG — mWidth is the byte size
            try {
                m_textures[i] = std::make_unique<Texture>(
                    reinterpret_cast<unsigned char*>(tex->pcData),
                    (int)tex->mWidth
                );
            } catch (const std::exception& e) {
                std::cerr << "Warning: embedded texture " << i
                          << " failed: " << e.what() << "\n";
            }
        }
    }

    // map each material to its diffuse texture index
    m_materialTexture.resize(m_scene->mNumMaterials, -1);
    for (unsigned int i = 0; i < m_scene->mNumMaterials; i++) {
        aiString texPath;
        if (m_scene->mMaterials[i]->GetTexture(
                aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
            if (texPath.C_Str()[0] == '*') {
                m_materialTexture[i] = std::atoi(texPath.C_Str() + 1);
            }
        }
    }

    std::cout << "Loaded " << m_clips.size() << " animations:\n";
    for (auto& c : m_clips)
        std::cout << "  [" << c.name << "] duration="
                  << c.duration / c.ticksPerSec << "s\n";
}

SkinnedMesh::~SkinnedMesh() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void SkinnedMesh::loadMesh(const aiScene* scene) {
    std::vector<SkinnedVertex> verts;
    std::vector<unsigned int>  indices;

    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];
        unsigned int baseVertex = (unsigned int)verts.size();

        // vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            SkinnedVertex v;
            v.position = { mesh->mVertices[i].x,
                           mesh->mVertices[i].y,
                           mesh->mVertices[i].z };
            v.normal   = { mesh->mNormals[i].x,
                           mesh->mNormals[i].y,
                           mesh->mNormals[i].z };
            v.texCoord = mesh->mTextureCoords[0]
                       ? glm::vec2(mesh->mTextureCoords[0][i].x,
                                   mesh->mTextureCoords[0][i].y)
                       : glm::vec2(0.0f);
            verts.push_back(v);
        }

        // bone weights
        processBoneWeights(mesh, verts);

        // track submesh for per-material texture binding
        SubMesh sub;
        sub.indexOffset = (GLuint)indices.size();
        sub.materialIdx = (int)mesh->mMaterialIndex;

        // indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
            for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
                indices.push_back(baseVertex + mesh->mFaces[i].mIndices[j]);

        sub.indexCount = (GLsizei)(indices.size() - sub.indexOffset);
        m_subMeshes.push_back(sub);
    }

    m_indexCount = (GLsizei)indices.size();

    // upload to GPU
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 verts.size() * sizeof(SkinnedVertex),
                 verts.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    auto stride = sizeof(SkinnedVertex);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,stride,(void*)offsetof(SkinnedVertex,position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,stride,(void*)offsetof(SkinnedVertex,texCoord));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,stride,(void*)offsetof(SkinnedVertex,normal));
    glEnableVertexAttribArray(2);

    glVertexAttribIPointer(3,4,GL_INT,stride,(void*)offsetof(SkinnedVertex,boneIDs));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4,4,GL_FLOAT,GL_FALSE,stride,(void*)offsetof(SkinnedVertex,boneWeights));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
}

void SkinnedMesh::processBoneWeights(aiMesh* mesh,
                                     std::vector<SkinnedVertex>& verts)
{
    unsigned int baseVertex = (unsigned int)verts.size() - mesh->mNumVertices;

    for (unsigned int b = 0; b < mesh->mNumBones; b++) {
        aiBone* bone = mesh->mBones[b];
        std::string boneName = bone->mName.C_Str();

        // register bone if new
        if (m_boneMap.find(boneName) == m_boneMap.end()) {
            BoneInfo info;
            info.id           = (int)m_boneMap.size();
            info.offsetMatrix = toGLM(bone->mOffsetMatrix);
            m_boneMap[boneName] = info;
        }

        int boneID = m_boneMap[boneName].id;

        // assign bone ID and weight to each affected vertex
        for (unsigned int w = 0; w < bone->mNumWeights; w++) {
            unsigned int vi = baseVertex + bone->mWeights[w].mVertexId;
            float weight    = bone->mWeights[w].mWeight;

            // find first empty slot (max 4 bones per vertex)
            for (int slot = 0; slot < 4; slot++) {
                if (verts[vi].boneIDs[slot] < 0) {
                    verts[vi].boneIDs[slot]     = boneID;
                    verts[vi].boneWeights[slot] = weight;
                    break;
                }
            }
        }
    }
}

void SkinnedMesh::loadAnimations(const aiScene* scene) {
    for (unsigned int a = 0; a < scene->mNumAnimations; a++) {
        aiAnimation* anim = scene->mAnimations[a];
        AnimClip clip;
        clip.name       = anim->mName.C_Str();
        clip.duration   = (float)anim->mDuration;
        clip.ticksPerSec= (float)(anim->mTicksPerSecond != 0
                                  ? anim->mTicksPerSecond : 25.0);

        for (unsigned int c = 0; c < anim->mNumChannels; c++) {
            aiNodeAnim* ch = anim->mChannels[c];
            BoneAnimation ba;
            ba.name = ch->mNodeName.C_Str();

            for (unsigned int k=0;k<ch->mNumPositionKeys;k++)
                ba.positions.push_back({
                    (float)ch->mPositionKeys[k].mTime,
                    { ch->mPositionKeys[k].mValue.x,
                      ch->mPositionKeys[k].mValue.y,
                      ch->mPositionKeys[k].mValue.z }
                });

            for (unsigned int k=0;k<ch->mNumRotationKeys;k++)
                ba.rotations.push_back({
                    (float)ch->mRotationKeys[k].mTime,
                    { ch->mRotationKeys[k].mValue.w,
                      ch->mRotationKeys[k].mValue.x,
                      ch->mRotationKeys[k].mValue.y,
                      ch->mRotationKeys[k].mValue.z }
                });

            for (unsigned int k=0;k<ch->mNumScalingKeys;k++)
                ba.scales.push_back({
                    (float)ch->mScalingKeys[k].mTime,
                    { ch->mScalingKeys[k].mValue.x,
                      ch->mScalingKeys[k].mValue.y,
                      ch->mScalingKeys[k].mValue.z }
                });

            clip.boneAnims[ba.name] = ba;
        }
        m_clips.push_back(clip);
    }
}

void SkinnedMesh::update(float dt) {
    if (m_clips.empty() || !m_scene || !m_scene->mRootNode) return;
    const AnimClip& clip = m_clips[m_currentClip];

    m_animTime += dt * clip.ticksPerSec;
    if (m_loop)
        m_animTime = std::fmod(m_animTime, clip.duration);
    else
        m_animTime = std::min(m_animTime, clip.duration);

    // reset all bone matrices to identity
    std::fill(m_boneMatrices.begin(), m_boneMatrices.end(), glm::mat4(1.0f));

    // walk the node tree and compute final bone matrices
    calcBoneMatrices(nullptr, glm::mat4(1.0f));
}

void SkinnedMesh::calcBoneMatrices(const aiNode* node,
                                   const glm::mat4& parentTransform)
{
    if (!node) {
        if (!m_scene || !m_scene->mRootNode) {
            return;
        }
        calcBoneMatrices(m_scene->mRootNode, glm::mat4(1.0f));
        return;
    }

    std::string nodeName = node->mName.C_Str();
    glm::mat4 nodeTransform = toGLM(node->mTransformation);

    const AnimClip& clip = m_clips[m_currentClip];
    auto it = clip.boneAnims.find(nodeName);
    if (it != clip.boneAnims.end()) {
        const BoneAnimation& ba = it->second;
        glm::mat4 T = glm::translate(glm::mat4(1.0f), ba.interpolatePos(m_animTime));
        glm::mat4 R = glm::toMat4(ba.interpolateRot(m_animTime));
        glm::mat4 S = glm::scale(glm::mat4(1.0f), ba.interpolateScale(m_animTime));
        nodeTransform = T * R * S;
    }

    glm::mat4 globalTransform = parentTransform * nodeTransform;

    auto boneIt = m_boneMap.find(nodeName);
    if (boneIt != m_boneMap.end()) {
        int id = boneIt->second.id;
        m_boneMatrices[id] = m_globalInverse
                           * globalTransform
                           * boneIt->second.offsetMatrix;
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
        calcBoneMatrices(node->mChildren[i], globalTransform);
     
}

void SkinnedMesh::draw(Shader& shader) const {
    // upload bone matrices
    for (int i = 0; i < MAX_BONES; i++)
        shader.setMat4("uBones[" + std::to_string(i) + "]", m_boneMatrices[i]);

    glBindVertexArray(m_vao);

    for (auto& sub : m_subMeshes) {
        // bind correct texture for this submesh
        int texIdx = (sub.materialIdx < (int)m_materialTexture.size())
                   ? m_materialTexture[sub.materialIdx]
                   : -1;

        if (texIdx >= 0 && texIdx < (int)m_textures.size()
            && m_textures[texIdx]) {
            m_textures[texIdx]->bind(0);
            shader.setInt("uTexture", 0);
        }

        glDrawElements(GL_TRIANGLES, sub.indexCount,
                       GL_UNSIGNED_INT,
                       (void*)(sub.indexOffset * sizeof(unsigned int)));
    }

    glBindVertexArray(0);
}

void SkinnedMesh::playAnimation(const std::string& name, bool loop) {
    for (int i = 0; i < (int)m_clips.size(); i++) {
        if (m_clips[i].name == name) {
            if (m_currentClip != i) {
                m_currentClip = i;
                m_animTime    = 0.0f;
            }
            m_loop = loop;
            return;
        }
    }
}

float SkinnedMesh::getAnimationDuration(const std::string& name, float fallbackSec) const {
    for (const auto& clip : m_clips) {
        if (clip.name == name) {
            if (clip.ticksPerSec <= 0.0f) {
                return fallbackSec;
            }
            return clip.duration / clip.ticksPerSec;
        }
    }
    return fallbackSec;
}

std::vector<std::string> SkinnedMesh::getAnimationNames() const {
    std::vector<std::string> names;
    for (auto& c : m_clips) names.push_back(c.name);
    return names;
}

glm::mat4 SkinnedMesh::toGLM(const aiMatrix4x4& m) {
    return glm::transpose(glm::mat4(
        m.a1,m.a2,m.a3,m.a4,
        m.b1,m.b2,m.b3,m.b4,
        m.c1,m.c2,m.c3,m.c4,
        m.d1,m.d2,m.d3,m.d4
    ));
}
