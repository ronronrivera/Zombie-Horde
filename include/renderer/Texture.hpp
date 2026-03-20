#pragma once
#include <glad/glad.h>
#include <string>

class Texture {
public:
    Texture(const std::string& path);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    void bind(GLuint slot = 0) const;
    void unbind()              const;

    int getWidth()  const { return m_width; }
    int getHeight() const { return m_height; }

private:
    GLuint m_id     = 0;
    int    m_width  = 0;
    int    m_height = 0;
};
