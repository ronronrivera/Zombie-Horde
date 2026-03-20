#include "renderer/Texture.hpp"
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Texture::Texture(const std::string& path) {
    stbi_set_flip_vertically_on_load(true); // OpenGL UVs start bottom-left

    int channels;
    unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &channels, 0);
    if (!data)
        throw std::runtime_error("Failed to load texture: " + path);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    // Prevent row alignment issues for RGB textures with non-4-byte row sizes.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height,
                 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Restore OpenGL default for any subsequent uploads.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    // repeat tiling — good for wall/floor textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
}

Texture::~Texture() {
    glDeleteTextures(1, &m_id);
}

void Texture::bind(GLuint slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}
