#pragma once

#include "renderer/Mesh.hpp"
#include "renderer/Shader.hpp"
#include "renderer/Texture.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <memory.h>

class TileMap{

public:

    TileMap();

    void draw(Shader &shader) const;

    //collision - is this world position inside a wall?
    bool isWall(float worldX, float worldZ) const;

    //where the player starts
    glm::vec3 getPlayerSpawn() const {return m_playerSpawn;}

    int getWidth() const {return MAP_W;}
    int getHeight() const {return MAP_H;}

private:
    void buildGeometry();

    //the warehouse map - 1 = wall, 0 = fall
    //20 columns x 16 rows
    static constexpr int MAP_W = 20;
    static constexpr int MAP_H = 16;

    static constexpr int MAP[MAP_H][MAP_W] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,1,1,1,0,0,0,0,0,0,0,1,1,1,0,0,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
        {1,0,0,1,1,1,0,0,0,0,0,0,0,1,1,1,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    };

    //separate meshes for walls, floor, ceiling
    std::unique_ptr<Mesh> m_wallMesh;
    std::unique_ptr<Mesh> m_floorMesh;
    std::unique_ptr<Mesh> m_ceilMesh;

    //textures
    std::unique_ptr<Texture> m_wallTex;
    std::unique_ptr<Texture> m_floorTex;
    std::unique_ptr<Texture> m_ceilTex;

    glm::vec3 m_playerSpawn {2.5f, 0.5f, 2.5f};


};
