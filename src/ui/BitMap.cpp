#include "ui/BitmapFont.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <limits>

BitmapFont::BitmapFont(const std::string& texturePath,
                       int atlasCols, int atlasRows)
    : m_texture(std::make_unique<Texture>(texturePath))
    , m_atlasCols(atlasCols)
    , m_atlasRows(atlasRows)
{
    const int texW = m_texture->getWidth();
    const int texH = m_texture->getHeight();

    // If the provided grid cannot evenly tile the atlas, pick a sane fallback.
    if (m_atlasCols <= 0 || m_atlasRows <= 0 ||
        (texW % m_atlasCols) != 0 || (texH % m_atlasRows) != 0) {
        int bestCols = 16;
        int bestRows = 6;
        int bestScore = std::numeric_limits<int>::max();

        for (int cols = 1; cols <= texW; ++cols) {
            if ((texW % cols) != 0) {
                continue;
            }

            const int cellW = texW / cols;
            if (cellW < 6 || cellW > 32) {
                continue;
            }

            for (int rows = 1; rows <= texH; ++rows) {
                if ((texH % rows) != 0) {
                    continue;
                }

                const int cellH = texH / rows;
                if (cellH != cellW) {
                    continue;
                }

                const int dCols = cols - atlasCols;
                const int dRows = rows - atlasRows;
                const int score = dCols * dCols + dRows * dRows;
                if (score < bestScore) {
                    bestScore = score;
                    bestCols = cols;
                    bestRows = rows;
                }
            }
        }

        m_atlasCols = bestCols;
        m_atlasRows = bestRows;
    }

    // unit quad — positions are 0..1, UVs set dynamically per char
    // layout: x y u v (4 floats per vertex, 4 vertices)
    float quad[] = {
        0.0f, 0.0f,  0.0f, 0.0f,
        1.0f, 0.0f,  1.0f, 0.0f,
        1.0f, 1.0f,  1.0f, 1.0f,
        0.0f, 1.0f,  0.0f, 1.0f,
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // GL_DYNAMIC_DRAW — we update UVs every character
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_DYNAMIC_DRAW);

    // position (x y)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texcoord (u v)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

BitmapFont::~BitmapFont() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void BitmapFont::drawText(Shader&            shader,
                          const std::string& text,
                          float x, float y,
                          float charWidth, float charHeight,
                          int screenW, int screenH,
                          glm::vec4 color) const
{
    shader.bind();
    m_texture->bind(0);
    shader.setInt("uTexture", 0);

    // pass screen size and char size to vertex shader
    shader.setVec2("uScreenSize", glm::vec2(screenW, screenH));
    shader.setVec2("uCharSize",   glm::vec2(charWidth, charHeight));
    shader.setVec4("uColor",      color);

    float uSize = 1.0f / (float)m_atlasCols;
    float vSize = 1.0f / (float)m_atlasRows;

    glBindVertexArray(m_vao);

    float cursorX = x;

    for (char c : text) {
        if (c == ' ') { cursorX += charWidth; continue; }

        // ASCII 32 = space = first printable glyph in the atlas.
        const int index = (int)(unsigned char)c - 32;
        if (index < 0 || index >= m_atlasCols * m_atlasRows) {
            cursorX += charWidth;
            continue;
        }

        int col = index % m_atlasCols;
        int row = index / m_atlasCols;
        row = (m_atlasRows - 1) - row; // atlas rows are authored top-to-bottom

        float uOff = col * uSize;
        float vOff = row * vSize;

        // update the UV coordinates in the VBO for this character
        float quad[] = {
            0.0f, 0.0f,  uOff,        vOff,
            1.0f, 0.0f,  uOff+uSize,  vOff,
            1.0f, 1.0f,  uOff+uSize,  vOff+vSize,
            0.0f, 1.0f,  uOff,        vOff+vSize,
        };

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);

        // tell vertex shader where to place this character
        shader.setVec2("uPosition", glm::vec2(cursorX, y));

        // draw the quad (4 verts as triangle fan)
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        cursorX += charWidth;
    }

    glBindVertexArray(0);
}
