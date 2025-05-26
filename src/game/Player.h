#pragma once
#include <raylib.h>
#include "src/engine/Entity.h"
#include <vector>

enum Direction {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_UP_LEFT,
    DIR_UP_RIGHT,
    DIR_DOWN_LEFT,
    DIR_DOWN_RIGHT
};

class Player : public Entity {
public:
    Vector2 position;
    Texture2D texture;
    Rectangle sourceRect;

    // --- Slash ---
    Texture2D slashTexture;
    int slashFrame;
    float slashFrameTime;
    float slashFrameDuration;
    const int slashFrameCount = 12;
    bool isSlashing;
    float slashDuration;
    float slashTimer;
    Direction slashDirection;

    float slashFadeDuration;
    bool isFading;
    float fadeTimer;

    // --- Special Skill ---
    Texture2D skillTexture;
    int skillFrame;
    float skillFrameTime;
    float skillFrameDuration;
    const int skillFrameCount = 12;  // you can change based on your spritesheet
    bool isUsingSkill;
    float skillTimer;
    float skillDuration;
    Direction skillDirection;

    float skillFadeDuration;
    bool skillIsFading;
    float skillFadeTimer;

    bool skillOnCooldown;
    float skillCooldownTimer;
    const float skillCooldownDuration = 5.0f;

    // --- Other ---
    int frame;
    float frameTime;
    float frameDuration;
    bool damaged = false;
    float damageTimer = 0.0f;
    const float damageDuration = 0.5f;
    Direction facingDirection = DIR_RIGHT;

    Player();
    ~Player();

    void Update(float deltaTime);
    void Draw() const;
    void DrawSlash() const;
    void DrawSkill() const;
    Rectangle getSlashHitbox() const;
    Rectangle getSkillHitbox() const;

    bool getIsSlashing() const { return isSlashing; }
    bool getIsUsingSkill() const { return isUsingSkill; }
};
