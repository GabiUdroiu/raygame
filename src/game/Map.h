#pragma once
#include "raylib.h"
#include <vector>

const int TILE_SIZE = 32;
const int MAP_WIDTH = 40;
const int MAP_HEIGHT = 25;

enum TileType {
    TILE_FLOOR,
    TILE_WALL,
    TILE_GRASS,
    TILE_WATER,
    TILE_COUNT
};

struct Tile {
    TileType type;
};

struct Tree {
    Vector2 position;
    Rectangle srcRect;  // part of the sprite sheet to draw
};


class Map {
private:
    Tile tiles[MAP_HEIGHT][MAP_WIDTH];
    Texture2D tileset;
    Texture2D treeTexture;

    std::vector<Tree> trees;

    float scale = 3.0f;

public:
    Map();
    ~Map();

    void Load(const char* tilesetPath);
    void LoadTree(const char* treePath);
    void Unload();

    void GenerateRandom();
    void GenerateFloorOnly();
    void Draw() const;
    void PlaceTree(int tileX, int tileY, int spriteX, int spriteY);

    std::vector<Rectangle> GetTreeCollisionBounds() const;
    bool IsOnTree(const Map& map) const;
};
