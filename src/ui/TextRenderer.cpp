#include "ui/TextRenderer.hpp"
#include <stdexcept>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

TextRenderer::TextRenderer(const std::string& fontPath, int fontSize) {
    // ── init FreeType ─────────────────────────────────────
    if (FT_Init_FreeType(&m_ft))
        throw std::runtime_error("Failed to init FreeType");

    if (FT_New_Face(m_ft, fontPath.c_str(), 0, &m_face))
        throw std::runtime_error("Failed to load font: " + fontPath);

    // 0 width = auto based on height
    FT_Set_Pixel_Sizes(m_face, 0, fontSize);

    // ── rasterize ASCII 32..127 ───────────────────────────
    // disable byte-alignment — glyph bitmaps are 1 byte per pixel
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 32; c < 128; c++) {
        if (FT_Load_Char(m_face, c, FT_LOAD_RENDER)) {
            std::cerr << "FreeType: failed to load glyph '"
                      << c << "'\n";
            continue;
        }

        FT_GlyphSlot g = m_face->glyph;

        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexImage2D(
            GL_TEXTURE_2D, 0,
            GL_RED,                          // 1-channel internal format
            g->bitmap.width, g->bitmap.rows,
            0,
            GL_RED, GL_UNSIGNED_BYTE,
            g->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        m_glyphs[c] = {
            tex,
            glm::ivec2(g->bitmap.width, g->bitmap.rows),
            glm::ivec2(g->bitmap_left,  g->bitmap_top),
            (GLuint)g->advance.x   // advance is in 1/64 pixels
        };
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // restore default
    glBindTexture(GL_TEXTURE_2D, 0);

    // free FreeType — glyph data is on GPU now, no longer needed
    FT_Done_Face(m_face);
    FT_Done_FreeType(m_ft);
    m_ft   = nullptr;
    m_face = nullptr;

    // ── setup dynamic quad VAO/VBO ────────────────────────
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // 6 vertices × 4 floats (x y u v) — two triangles per glyph
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * 6 * 4,
                 nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
                          4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    std::cout << "TextRenderer loaded: " << fontPath
              << " @ " << fontSize << "px\n";
}

TextRenderer::~TextRenderer() {
    for (auto& [c, g] : m_glyphs)
        glDeleteTextures(1, &g.textureID);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void TextRenderer::drawText(Shader&            shader,
                             const std::string& text,
                             float x, float y,
                             int   screenW, int screenH,
                             glm::vec3 color,
                             float scale) const
{
    shader.bind();
    shader.setVec3("uTextColor", color);

    // orthographic projection — maps pixel coords to NDC
    glm::mat4 proj = glm::ortho(
        0.0f, (float)screenW,
        (float)screenH, 0.0f   // top-left origin (Y down)
    );
    shader.setMat4("uProjection", proj);
    shader.setInt("uText", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_vao);

    float cursorX = x;

    for (char c : text) {
        auto it = m_glyphs.find(c);
        if (it == m_glyphs.end()) continue;

        const Glyph& g = it->second;

        // pixel position of this glyph quad
        float xPos = cursorX + g.bearing.x * scale;
        float yPos = y + (m_glyphs.at('H').bearing.y
                       - g.bearing.y) * scale; // align to baseline

        float w = g.size.x * scale;
        float h = g.size.y * scale;

        // two triangles — 6 vertices, each x y u v
        float verts[6][4] = {
            { xPos,     yPos,     0.0f, 0.0f },
            { xPos,     yPos + h, 0.0f, 1.0f },
            { xPos + w, yPos + h, 1.0f, 1.0f },

            { xPos,     yPos,     0.0f, 0.0f },
            { xPos + w, yPos + h, 1.0f, 1.0f },
            { xPos + w, yPos,     1.0f, 0.0f },
        };

        glBindTexture(GL_TEXTURE_2D, g.textureID);

        // upload this glyph's quad to GPU
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        // advance.x is in 1/64th pixels — shift right
        cursorX += (g.advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
