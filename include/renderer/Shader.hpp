
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {

public:
    Shader(const std::string& vertPath, const std::string& fragPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void bind() const;
    void unbind() const;
    
    void setInt  (const std::string& name, int value)          const;
    void setFloat(const std::string& name, float value)        const;
    void setMat4 (const std::string& name, const glm::mat4& m) const;
    void setVec3 (const std::string& name, const glm::vec3& v) const;
    void setVec4 (const std::string& name, const glm::vec4& v) const;
    void setVec2(const std::string& name, const glm::vec2& v) const;

private:
    GLuint m_id = 0;

    static std::string readFile(const std::string& path);
    static GLuint compileShader(GLenum type, const std::string& src);
};
