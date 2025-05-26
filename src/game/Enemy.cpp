#include "Enemy.h"
#include "Globals.h"
#include <cmath>
#include <stdlib.h>

// Initialize static member
Texture2D Enemy::textures[];

Enemy::Enemy(float x, float y, int health = 20, int damage = 5, float speed = 100.0f)
    : Entity(health, damage), position{ x, y }, speed(speed), frame(0), frameTime(0.0f), frameDuration(0.2f)
{
    sourceRect = { 0, 0, 32, 32 };
    textureIndex = rand() % 2;
}

Enemy::~Enemy()
{
    // Nothing to unload here because texture is static and unloaded separately
}

void Enemy::LoadAssets()
{
    if (textures[0].id == 0 && textures[1].id == 0) {
        textures[0] = LoadTexture("assets/enemy1.png");
        textures[1] = LoadTexture("assets/enemy2.png");
    }
}

void Enemy::UnloadAssets()
{
    for (int i = 0; i < 2; ++i) {
        if (textures[i].id != 0) {
            UnloadTexture(textures[i]);
            textures[i] = { 0 };
        }
    }
}

void Enemy::Update(float deltaTime, Vector2 playerPos)
{
    UpdateDamageCooldown(deltaTime);

    frameTime += deltaTime;
    if (frameTime >= frameDuration) {
        frame = (frame + 1) % 4;  // 4 frames animation
        sourceRect.x = frame * 32;
        frameTime = 0.0f;
    }

    // Move towards player
    Vector2 direction = { playerPos.x - position.x, playerPos.y - position.y };
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
    if (length != 0) {
        direction.x /= length;
        direction.y /= length;
    }

    position.x += direction.x * speed * deltaTime;
    position.y += direction.y * speed * deltaTime;
}

void Enemy::Draw() const
{
    float scale = 2.0f;
    Rectangle destRec = { position.x, position.y, 32.0f * scale, 32.0f * scale };
    Vector2 origin = { 0.0f, 0.0f };
    float rotation = 0.0f;

    Color tintColor = (damageCooldown > 0.0f && damageCooldown > (damageInterval - damageTintDuration)) ? RED : WHITE;

    DrawTexturePro(textures[textureIndex], sourceRect, destRec, origin, rotation, tintColor);

    if (debug) {
        DrawRectangleLinesEx(destRec, 1, ORANGE);
        Rectangle damageableArea = { position.x + 8, position.y + 8, 48.0f, 48.0f };
        DrawRectangleLinesEx(damageableArea, 1, RED);
    }
}
