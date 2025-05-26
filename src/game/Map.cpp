#include "Map.h"
#include "Globals.h"
#include <cstdlib>  // for rand()
#include <ctime>    // for time()
#include <vector>

Map::Map() {
    std::srand(static_cast<unsigned int>(time(nullptr)));
}

Map::~Map() {
    Unload();
}

void Map::Load(const char* tilesetPath) {
    tileset = LoadTexture(tilesetPath);
}

void Map::LoadTree(const char* treePath) {
    treeTexture = LoadTexture(treePath);
}

void Map::Unload() {
    UnloadTexture(tileset);
    UnloadTexture(treeTexture);
}

void Map::GenerateRandom() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int r = std::rand() % TILE_COUNT;
            tiles[y][x].type = static_cast<TileType>(r);
        }
    }
}

void Map::PlaceTree(int tileX, int tileY, int spriteX, int spriteY) {
    const float SPRITE_SIZE = 128.0f; // assuming each tree is 128x128
    Rectangle src = {
        spriteX * SPRITE_SIZE,
        spriteY * SPRITE_SIZE,
        SPRITE_SIZE,
        SPRITE_SIZE
    };
    trees.push_back({
        Vector2{ (float)tileX, (float)tileY },
        src
    });
}

std::vector<Rectangle> Map::GetTreeCollisionBounds() const {
    std::vector<Rectangle> bounds;
    for (const Tree& tree : trees) {
        Rectangle dest = {
            tree.position.x * TILE_SIZE * scale,
            tree.position.y * TILE_SIZE * scale,
            tree.srcRect.width * (scale / 2),   // same as used in Draw
            tree.srcRect.height * (scale / 2)
        };
        bounds.push_back(dest);
    }
    return bounds;
}


void Map::Draw() const {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            Rectangle sourceRect;

            switch (tiles[y][x].type) {
                case TILE_FLOOR: sourceRect = { 0 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE }; break;
                case TILE_WALL:  sourceRect = { 1 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE }; break;
                case TILE_GRASS: sourceRect = { 2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE }; break;
                case TILE_WATER: sourceRect = { 3 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE }; break;
                default:         sourceRect = { 0, 0, TILE_SIZE, TILE_SIZE }; break;
            }

            Rectangle destRect = {
                x * TILE_SIZE * scale,
                y * TILE_SIZE * scale,
                TILE_SIZE * scale,
                TILE_SIZE * scale
            };

            DrawTexturePro(tileset, sourceRect, destRect, {0, 0}, 0.0f, WHITE);
            if(debug==true)
            DrawRectangleLines(
                (int)(x * TILE_SIZE * scale), 
                (int)(y * TILE_SIZE * scale), 
                (int)(TILE_SIZE * scale), 
                (int)(TILE_SIZE * scale), 
                DARKGREEN
            );
        }
    }

    // --- Draw trees ---
    for (const Tree& tree : trees) {
        Rectangle dest = {
            tree.position.x * TILE_SIZE * scale,
            tree.position.y * TILE_SIZE * scale,
            tree.srcRect.width * (scale / 2),   // adjust scale as needed
            tree.srcRect.height * (scale / 2)
        };

        DrawTexturePro(treeTexture, tree.srcRect, dest, {0, 0}, 0.0f, WHITE);
        if(debug == true)
        DrawRectangleLines(
            (int)(tree.position.x * TILE_SIZE * scale), 
            (int)(tree.position.y * TILE_SIZE * scale), 
            (int)(TILE_SIZE * scale *2), 
            (int)(TILE_SIZE * scale*2), 
            YELLOW
        );
    }
}
