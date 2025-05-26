#include "Player.h"
#include "Globals.h"
#include <raymath.h>
#include <iostream>

Player::Player() : Entity(100, 10) {
    position = { 32.0f, 32.0f };
    texture = LoadTexture("assets/player.png");
    slashTexture = LoadTexture("assets/attack.png");
    skillTexture = LoadTexture("assets/special_skill.png");
    frame = 0;
    frameTime = 0.0f;
    frameDuration = 0.2f;  // switch frame every 0.2 seconds

    slashFrame = 0;
    slashFrameTime = 0.0f;
    slashFrameDuration = 0.025f;  // faster slash animation frames
    isSlashing = false;
    slashDuration = 0.15f;
    slashTimer = 0.0f;

    slashFadeDuration = 0.0f;  // extra time for fade out
    isFading = false;
    fadeTimer = 0.0f;
    //
    skillFrame = 0;
    skillFrameTime = 0.0f;
    skillFrameDuration = 0.075f;
    isUsingSkill = false;
    skillDuration = 0.74f;
    skillTimer = 0.0f;

    skillFadeDuration = 0.0f;  // extra time for fade out
    skillIsFading = false;
    skillFadeTimer = 0.0f;

    skillOnCooldown = false;
    skillCooldownTimer = 0.0f;
    // Suppose player sprite sheet is 4 frames horizontally, each 32x32
    sourceRect = { 0, 0, 32, 32 };
}

Player::~Player() {
    UnloadTexture(texture);
    UnloadTexture(slashTexture);
    UnloadTexture(skillTexture);
}


void Player::Update(float deltaTime) {

    if (IsKeyPressed(KEY_H)) {
        debug = !debug;
        std::cout << "Debug mode: " << (debug ? "ON" : "OFF") << "\n";
    }
    UpdateDamageCooldown(deltaTime);
    // Simple animation update
    frameTime += deltaTime;
    if (frameTime >= frameDuration) {
        frame = (frame + 1) % 4;  // 4 frames loop
        sourceRect.x = frame * 32;
        frameTime = 0.0f;
    }

    // Movement
    float speed = 200.0f;
    if (IsKeyDown(KEY_W)) {
        position.y -= speed * deltaTime;
        //facingDirection = DIR_UP;
    }
    if (IsKeyDown(KEY_S)) {
        position.y += speed * deltaTime;
        //facingDirection = DIR_DOWN;
    }
    if (IsKeyDown(KEY_A)) {
        position.x -= speed * deltaTime;
        //facingDirection = DIR_LEFT;
    }
    if (IsKeyDown(KEY_D)) {
        position.x += speed * deltaTime;
        //facingDirection = DIR_RIGHT;
    }
    position.x = Clamp(position.x, 0, 1200 - 64);
    position.y = Clamp(position.y, 0, 800 - 64);

    //slash

    if ((IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) && !isSlashing) {
        isSlashing = true;
        slashTimer = 0.0f;
        // Optional: Snap to 8 directions (you can skip this if you want free angle)
        Vector2 mousePos = GetMousePosition();
        Vector2 toMouse = Vector2Subtract(mousePos, position);

        float angle = atan2f(toMouse.y, toMouse.x) * RAD2DEG;
        if (angle >= -22.5f && angle < 22.5f)
            slashDirection = DIR_RIGHT;
        else if (angle >= 22.5f && angle < 67.5f)
            slashDirection = DIR_DOWN_RIGHT;
        else if (angle >= 67.5f && angle < 112.5f)
            slashDirection = DIR_DOWN;
        else if (angle >= 112.5f && angle < 157.5f)
            slashDirection = DIR_DOWN_LEFT;
        else if (angle >= -67.5f && angle < -22.5f)
            slashDirection = DIR_UP_RIGHT;
        else if (angle >= -112.5f && angle < -67.5f)
            slashDirection = DIR_UP;
        else if (angle >= -157.5f && angle < -112.5f)
            slashDirection = DIR_UP_LEFT;
        else
            slashDirection = DIR_LEFT;
        // You can also trigger a sound effect or animation here
    }
    if (isSlashing) {
        slashTimer += deltaTime;

        // Update slash animation frame
        slashFrameTime += deltaTime;
        if (slashFrameTime >= slashFrameDuration) {
            if (slashFrame < slashFrameCount - 1) {
                slashFrame++;
            }
            slashFrameTime = 0.0f;
        }

        if (slashTimer >= slashDuration && !isFading) {
            isFading = true;
            fadeTimer = 0.0f;
        }
        if (isFading) {
            fadeTimer += deltaTime;
            if (fadeTimer >= slashFadeDuration) {
                isSlashing = false;
                isFading = false;
                slashTimer = 0.0f;
                fadeTimer = 0.0f;
                slashFrame = 0;
            }
        }
    }
    //skill
    if ((IsKeyPressed(KEY_B) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) && !isUsingSkill) {
        if(!skillOnCooldown) {
        isUsingSkill = true;
        skillTimer = 0.0f;
        skillOnCooldown = true;
        skillCooldownTimer = 0.0f;
        }
        else 
        std::cout<<"skill on cooldown!!!!\n";
    }
    if (isUsingSkill) {
        skillTimer += deltaTime;

        // Update slash animation frame
        skillFrameTime += deltaTime;
        if (skillFrameTime >= skillFrameDuration) {
            if (skillFrame < skillFrameCount - 1) {
                skillFrame++;
            }
            skillFrameTime = 0.0f;
        }

        if (skillTimer >= skillDuration && !skillIsFading) {
            skillIsFading = true;
            skillFadeTimer = 0.0f;
        }
        if (skillIsFading) {
            skillFadeTimer += deltaTime;
            if (skillFadeTimer >= skillFadeDuration) {
                isUsingSkill = false;
                skillIsFading = false;
                skillTimer = 0.0f;
                skillFadeTimer = 0.0f;
                skillFrame = 0;
            }
        }
    }
    if (skillOnCooldown) {
    skillCooldownTimer += deltaTime;
    if (skillCooldownTimer >= skillCooldownDuration) {
        skillOnCooldown = false;
        skillCooldownTimer = 0.0f;
        std::cout<<"skill ready\n";
    }
}
}

Rectangle Player::getSlashHitbox() const {
    float slashRange = 64.0f;
    float slashWidth = 64.0f; // height or width of the hitbox depending on direction

    switch(slashDirection) {
        case DIR_UP_RIGHT:
            return Rectangle{
                position.x + 64 - 24,  // offset X and Y diagonally
                position.y - slashRange + 24,
                slashRange,
                slashRange
            };
        case DIR_UP_LEFT:
            return Rectangle{
                position.x - slashRange + 24,
                position.y - slashRange + 24,
                slashRange,
                slashRange
            };
        case DIR_DOWN_RIGHT:
            return Rectangle{
                position.x + 48,
                position.y + 48,
                slashRange,
                slashRange
            };
        case DIR_DOWN_LEFT:
            return Rectangle{
                position.x - slashRange + 24,
                position.y + 48,
                slashRange,
                slashRange
            };
        case DIR_RIGHT:
            return Rectangle{
                position.x + 48.0f,
                position.y,
                slashRange,
                slashWidth
            };
        case DIR_LEFT:
            return Rectangle{
                position.x - slashRange + 24,
                position.y,
                slashRange,
                slashWidth
            };
        case DIR_UP:
            return Rectangle{
                position.x,
                position.y - slashRange + 24,
                slashWidth,  // swap width and height for vertical
                slashRange
            };
        case DIR_DOWN:
            return Rectangle{
                position.x,
                position.y + 48.0f,
                slashWidth,
                slashRange
            };
        default:
            return Rectangle{ position.x, position.y, slashRange, slashWidth };
    }
}

void Player::DrawSlash() const {
    if (!isSlashing) return;

    Rectangle slashHitbox = getSlashHitbox();
    if(debug==true)
    DrawRectangleLinesEx(slashHitbox, 1, RED);

    Rectangle srcRect = {
        slashFrame * 32.0f,
        0.0f,
        32.0f,
        32.0f
    };

    Vector2 origin = { 0.0f, 0.0f };
    float rotation = 0.0f;

    DrawTexturePro(slashTexture, srcRect, slashHitbox, origin, rotation, WHITE);
}

Rectangle Player::getSkillHitbox() const {
    float skillRange = 196.0f;
    float skillWidth = 96.0f; // height or width of the hitbox depending on direction

    return Rectangle{ position.x-64, position.y-16, skillRange, skillWidth };
}

void Player::DrawSkill() const {
    if (!isUsingSkill) return;

    Rectangle skillHitbox = getSkillHitbox();
    skillHitbox.y -= 8;
    if(debug==true)
    DrawRectangleLinesEx(skillHitbox, 1, RED);

    Rectangle srcRect = {
        skillFrame * 32.0f,
        0.0f,
        32.0f,
        32.0f
    };

    Vector2 origin = { 0.0f, 0.0f };
    float rotation = 0.0f;
    skillHitbox.y += 16;
    DrawTexturePro(skillTexture, srcRect, skillHitbox, origin, rotation, WHITE);
}

void Player::Draw() const {
    float scale = 2.0f;

    // Draw player sprite
    Rectangle destRec = { position.x, position.y, 32.0f * scale, 32.0f * scale };
    Rectangle damageableArea = { position.x+12, position.y+12, 40, 40 };
    Vector2 origin = { 0.0f, 0.0f };
    float rotation = 0.0f;

    Color tintColor = (damageCooldown > 0.0f && damageCooldown > (damageInterval - damageTintDuration)) ? RED : WHITE;

    if (facingDirection == DIR_LEFT) {
        Rectangle flippedSrc = sourceRect;
        flippedSrc.width = -flippedSrc.width;
        flippedSrc.x += sourceRect.width;
        DrawTexturePro(texture, flippedSrc, destRec, origin, rotation, tintColor);
    } 
    else DrawTexturePro(texture, sourceRect, destRec, origin, rotation, tintColor);
        // Debug rectangle 
    if(debug == true) {
        DrawRectangleLinesEx(destRec, 1, ORANGE);
        DrawRectangleLinesEx(damageableArea, 1, RED);
    }
}

