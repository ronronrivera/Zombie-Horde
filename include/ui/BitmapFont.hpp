#pragma once
#include "renderer/Texture.hpp"
#include "renderer/Shader.hpp"
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <glad/glad.h>


class BitmapFont {
public:
    // atlasRows/atlasCols — how many rows/cols of chars in the PNG
    // for a standard ASCII atlas: 16 cols x 6 rows
    BitmapFont(const std::string& texturePath,
               int atlasCols = 16, int atlasRows = 6);
    ~BitmapFont();

    BitmapFont(const BitmapFont&) = delete;
    BitmapFont& operator=(const BitmapFont&) = delete;

    // draw a string at pixel position (x, y) from top-left
    void drawText(Shader&            shader,
                  const std::string& text,
                  float x, float y,
                  float charWidth, float charHeight,
                  int screenW, int screenH,
                  glm::vec4 color = glm::vec4(1.0f)) const;

private:
    std::unique_ptr<Texture> m_texture;
    int   m_atlasCols;
    int   m_atlasRows;

    // single reusable quad VAO/VBO — we update UVs per character
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
};
