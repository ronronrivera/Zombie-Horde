
#include "world/TileMap.hpp"
#include <glm/gtc/matrix_transform.hpp>

// helper - push a quad (2 triangles) into vertex + index buffers
// p0-p3 are the 4 corners, uv scales for texture tiling
static void pushQuad(
    std::vector<float>&         verts,
    std::vector<unsigned int>&  idx,
    glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
    float uScale = 1.0f, float vScale = 1.0f

)
{
    unsigned int base = (unsigned int)(verts.size() / 5);

    // x y z u v x4
    auto push = [&](glm::vec3 p, float u, float v) {
        verts.insert(verts.end(), {p.x, p.y, p.z, u, v});
    };

    push(p0, 0.0f, 0.0f);
    push(p1, uScale, 0.0f);
    push(p2, uScale, vScale);
    push(p3, 0.0f, vScale);

    idx.insert(idx.end(), {
        base, base+1, base+2,
        base+2, base+3, base
    });
}


TileMap::TileMap() {
    m_wallTex  = std::make_unique<Texture>("assets/textures/walls/brick.jpg");
    m_floorTex = std::make_unique<Texture>("assets/textures/floors/concrete.jpg");
    m_ceilTex  = std::make_unique<Texture>("assets/textures/walls/ceiling.jpg");
    buildGeometry();
}

void TileMap::buildGeometry(){
    
    std::vector<float>      wallV, floorV, ceilV;
    std::vector<unsigned int> wallI, floorI, ceilI;
    

    for (int row = 0; row < MAP_H; ++row) {
        for (int col = 0; col < MAP_W; ++col) {

            float x = (float)col;
            float z = (float)row;

            if (MAP[row][col] == 1) {
                // ── wall cube — 4 visible faces ──────────────────
                // we only push faces that border a floor tile
                // (no point drawing faces buried inside other walls)

                // south face (toward +Z)
                if (row+1 < MAP_H && MAP[row+1][col] == 0)
                    pushQuad(wallV, wallI,
                             {x,     0, z+1}, {x+1, 0, z+1},
                             {x+1,   1, z+1}, {x,   1, z+1});

                // north face (toward -Z)
                if (row-1 >= 0 && MAP[row-1][col] == 0)
                    pushQuad(wallV, wallI,
                             {x+1, 0, z}, {x,   0, z},
                             {x,   1, z}, {x+1, 1, z});

                // east face (toward +X)
                if (col+1 < MAP_W && MAP[row][col+1] == 0)
                    pushQuad(wallV, wallI,
                             {x+1, 0, z},   {x+1, 0, z+1},
                             {x+1, 1, z+1}, {x+1, 1, z});

                // west face (toward -X)
                if (col-1 >= 0 && MAP[row][col-1] == 0)
                    pushQuad(wallV, wallI,
                             {x, 0, z+1}, {x, 0, z},
                             {x, 1, z},   {x, 1, z+1});

            } else {
                // ── floor quad ───────────────────────────────────
                pushQuad(floorV, floorI,
                         {x,   0, z+1}, {x+1, 0, z+1},
                         {x+1, 0, z},   {x,   0, z});

                // ── ceiling quad ─────────────────────────────────
                pushQuad(ceilV, ceilI,
                         {x,   1, z},   {x+1, 1, z},
                         {x+1, 1, z+1}, {x,   1, z+1});
            }
        }
    }

    m_wallMesh  = std::make_unique<Mesh>(wallV,  wallI);
    m_floorMesh = std::make_unique<Mesh>(floorV, floorI);
    m_ceilMesh  = std::make_unique<Mesh>(ceilV,  ceilI);

}

void TileMap::draw(Shader& shader) const {
    //identity model = the map IS world space
    glm::mat4 model(1.0f);
    shader.setMat4("uModel", model);

    m_wallTex->bind(0);
    shader.setInt("uTexture", 0);
    m_wallMesh->draw();

    m_floorTex->bind(0);
    m_floorMesh->draw();

    m_ceilTex->bind(0);
    m_ceilMesh->draw();
}

bool TileMap::isWall(float worldX, float worldZ) const {
    //convert world position -> grid cell
    int col = (int)worldX;
    int row = (int)worldZ;

    // out of bounds = treat as wall
    if (col < 0 || col >= MAP_W || row < 0 || row >= MAP_H)
        return true;

    return MAP[row][col] == 1;
}
