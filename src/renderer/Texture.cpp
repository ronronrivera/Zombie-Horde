#include "renderer/Texture.hpp"
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>

Texture::Texture(const std::string& filePath) {
    stbi_set_flip_vertically_on_load(false);

    int channels;
    unsigned char* pixels = stbi_load(
        filePath.c_str(), &m_width, &m_height, &channels, 4); // force RGBA

    if (!pixels) {
        std::cerr << "stbi error: " << stbi_failure_reason() << "\n";
        throw std::runtime_error("Failed to load texture: " + filePath);
    }

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 m_width, m_height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(pixels);
}

Texture::Texture(const unsigned char* data, int dataSize) {
    stbi_set_flip_vertically_on_load(false);

    int channels;
    unsigned char* pixels = stbi_load_from_memory(
        data, dataSize, &m_width, &m_height, &channels, 4); // force RGBA

    if (!pixels) {
        std::cerr << "stbi error: " << stbi_failure_reason() << "\n";
        throw std::runtime_error("Failed to load embedded texture");
    }

    std::cout << "Embedded texture loaded: "
              << m_width << "x" << m_height
              << " channels=" << channels << "\n";

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 m_width, m_height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(pixels);
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
