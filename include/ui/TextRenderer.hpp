#pragma once

#include "renderer/Shader.hpp"
#include <glm/glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <unordered_map>
#include <glad/glad.h>

struct Glyph{
    GLuint textureID;
    glm::ivec2 size; 
    glm::ivec2 bearing;
    GLuint advance;
};

class TextRenderer {

public:
    TextRenderer(const std::string& fontPath, int fontSize);
    ~TextRenderer();

    TextRenderer(const TextRenderer&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;
    
    //draw text at pixel coordinates from top-left screen
    void drawText(  Shader& shader,
                    const std::string& text,
                    float x, float y,
                    int screenW, int screenH,
                    glm::vec3 color = glm::vec3(1.0f),
                    float scale = 1.0f
                  ) const;
private:
    FT_Library m_ft = nullptr;
    FT_Face m_face = nullptr;

    std::unordered_map<char, Glyph> m_glyphs;

    GLuint m_vao = 0;
    GLuint m_vbo = 0;

};
