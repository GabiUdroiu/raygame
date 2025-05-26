#pragma once
#include <raylib.h>
#include "src/engine/Entity.h"

class Enemy : public Entity {
public:
    Enemy(float x, float y, int health, int damage, float speed);
    ~Enemy();

    void Update(float deltaTime, Vector2 playerPos);
    void Draw() const;

    Vector2 position;
    int textureIndex;

    static void LoadAssets();
    static void UnloadAssets();

private:
    static Texture2D textures[2];  // shared texture for all enemies

    Rectangle sourceRect;  // current frame rectangle
    float speed;
    int frame;             // animation frame
    float frameTime;
    float frameDuration;


};
