#include <raylib.h>
#include <math.h>
#include "src/engine/SaveSystem.h"

#include "src/ui/loading.h"
#include "src/ui/score.h"

#include "src/game/Player.h"
#include "src/game/Enemy.h"
#include "src/game/Map.h"
#include "src/game/Globals.h"

#include <iostream>
#include <vector>
#include <algorithm>


float GetRandomFloat(float min, float max) {
    return min + (float)GetRandomValue(0, 10000) / 10000.0f * (max - min);
}

// void DrawWavyOverlay(float time) {
//     int waveHeight = 8;
//     int sliceCount = GetScreenHeight() / waveHeight;
//     float waveSpeed = 3.0f;
//     float waveAmplitude = 20.0f;

//     for (int i = 0; i < sliceCount; i++) {
//         float offset = cosf(time * waveSpeed + i * 0.3f) * waveAmplitude;
//         DrawRectangle(
//             offset,               // x shift
//             i * waveHeight,       // y
//             GetScreenWidth(),          // width (covers whole screen)
//             waveHeight,           // height per slice
//             Fade(BLACK, 0.3f)     // overlay color and transparency
//         );
//         DrawRectangle(
//             offset,               // x shift
//             i * waveHeight,       // y
//             GetScreenWidth(),          // width (covers whole screen)
//             waveHeight,           // height per slice
//             Fade(RED, 0.1f)     // overlay color and transparency
//         );
//     }
// }
// struct DeathEffect {
//     Vector2 position;
//     float lifetime = 0.5f;
//     float timer = 0.0f;
//     Color color;

//     DeathEffect(Vector2 pos) : position(pos), color(RED) {}

//     void Update(float dt) { 
//         timer += dt; 
//         // no position change here
//     }

//     void Draw() const {
//         float alpha = 1.0f - (timer / lifetime);
//         Vector2 drawPos = { position.x, position.y + 32 };  // fixed offset, e.g. +128 pixels on Y
//         DrawCircleV(drawPos, 30 * alpha, Fade(color, alpha));
//         DrawText("+50", position.x, position.y +32, 20, Fade(GOLD, alpha));
//     }

//     bool isFinished() const { return timer >= lifetime; }
// };

struct BloodParticle {
    Vector2 position;
    Vector2 velocity;
    float size;
};

struct DeathEffect {
    std::vector<BloodParticle> particles;
    float lifetime = 0.75f;
    float timer = 0.0f;
    Color color;

    DeathEffect(Vector2 pos) : color(RED) {
        int count = GetRandomValue(4, 6);
        for (int i = 0; i < count; i++) {
            BloodParticle p;
            p.position = pos;
            p.velocity = {
                GetRandomValue(-100, 100) / 100.0f, // small random x speed
                GetRandomValue(-100, 100) / 100.0f  // small random y speed
            };
            p.size = GetRandomValue(8, 14); // small square size
            particles.push_back(p);
        }
    }

    void Update(float dt) {
        timer += dt;
        for (auto& p : particles) {
            p.position.x += p.velocity.x * 60 * dt; // move over time
            p.position.y += p.velocity.y * 60 * dt;
        }
    }

    void Draw() const {
        float alpha = 1.0f - (timer / lifetime);
        for (const auto& p : particles) {
            DrawRectangle(p.position.x, p.position.y, p.size, p.size, Fade(color, alpha));
        }
        // Optionally draw the score text
        DrawText("+50", particles[0].position.x, particles[0].position.y - 10, 20, Fade(GOLD, alpha));
    }

    bool isFinished() const { return timer >= lifetime; }
};

const int screenWidth = 1200;
const int screenHeight = 800;

float enemySpawnTimer = 0.0f;
float spawnInterval;
int entityAmount = 5;

int barX = 20;
int barY = 40;
int barWidth = 200;
int barHeight = 20;
int eHP = 20;
int eDMG = 5;
float eSpeed = 100.0f;
std::vector<DeathEffect> deathEffects;
bool isGameOver = false;

struct SmokeParticle {
    Vector2 position;
    Vector2 velocity;
    float size;
    float lifetime;
    float age;
};

std::vector<SmokeParticle> smokeParticles;
float smokeSpawnTimer = 0.0f;
const float smokeSpawnInterval = 0.15f;

void StartGame()
{
    Score score;
    SaveSystem save;
    int playerID = save.GeneratePlayerID();
    Map map;
    map.Load("assets/tileset.png");
    map.LoadTree("assets/tree.png");

    map.GenerateRandom();
    map.PlaceTree(9, 3, 0, 0);
    map.PlaceTree(6, 0, 0, 0);
    map.PlaceTree(1, 5, 0, 0);

    Player player;
    int maxHealth = player.getMaxHealth();
    Enemy::LoadAssets();

    std::vector<Enemy*> enemies;

    // --- Level display variables ---
    float levelDisplayTimer = 0.0f;
    const float levelDisplayDuration = 2.0f; // seconds
    int currentLevel = entityAmount - 4;
    spawnInterval = 5.0f;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        if (isGameOver && IsKeyPressed(KEY_ESCAPE)) break;

        if (!isGameOver) {
            score.Update(deltaTime);
            player.Update(deltaTime);

            for (auto& enemy : enemies)
                enemy->Update(deltaTime, player.position);

            // Remove dead enemies
            enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](Enemy* e) {
                if (!e->isAlive()) {
                    delete e;
                    return true;
                }
                return false;
            }), enemies.end());
            // Smoke particle spawning
            smokeSpawnTimer += deltaTime;
            while (smokeSpawnTimer >= smokeSpawnInterval) {
                smokeSpawnTimer -= smokeSpawnInterval;

                SmokeParticle p;
                p.position = { (float)GetRandomValue(0, screenWidth), -10 };
                p.velocity = {
                    sinf(GetTime() * 0.5f + p.position.x * 0.01f) * 10.0f, // gentle side-to-side
                    GetRandomValue(1000, 2000) / 10.0f                      // stronger downward fall (10 to 20 units/sec)
                };
                p.size = GetRandomValue(4, 12);
                p.lifetime = 7.0f;
                p.age = 0.0f;
                smokeParticles.push_back(p);
            }

            // Update particles
            for (auto& p : smokeParticles) {
                p.age += deltaTime;
                p.position.x += p.velocity.x * deltaTime;
                p.position.y += p.velocity.y * deltaTime;
            }

            // Remove old smoke particles
            smokeParticles.erase(std::remove_if(smokeParticles.begin(), smokeParticles.end(),
                [](SmokeParticle& p) {
                    return p.age >= p.lifetime;
                }), smokeParticles.end());
            // Update death effects
            for (auto& effect : deathEffects)
                effect.Update(deltaTime);
            deathEffects.erase(std::remove_if(deathEffects.begin(), deathEffects.end(),
            [](const DeathEffect& e) { return e.isFinished(); }), deathEffects.end());

            Rectangle playerHitbox = { player.position.x + 12, player.position.y + 12, 40, 40 };

            for (auto& enemy : enemies) {
                if (!enemy->isAlive()) continue;

                Rectangle enemyHitbox = { enemy->position.x, enemy->position.y, 64, 64 };

                if (player.getIsSlashing()) {
                    Rectangle slashHitbox = player.getSlashHitbox();
                    if (CheckCollisionRecs(slashHitbox, enemyHitbox) && enemy->canTakeDamage()) {
                        enemy->takeDamage(player.getDamage());
                        if (enemy->getHealth() <= 0) {
                            deathEffects.emplace_back(enemy->position);
                            score.AddPoints(50);
                            std::cout << "+Enemy killed 50 points added\n";
                        }
                    }
                }

                if (player.getIsUsingSkill()) {
                    Rectangle skillHitbox = player.getSkillHitbox();
                    if (CheckCollisionRecs(skillHitbox, enemyHitbox) && enemy->canTakeDamage()) {
                        enemy->takeDamage(player.getDamage() * 5);
                        if (enemy->getHealth() <= 0) {
                            deathEffects.emplace_back(enemy->position);
                            score.AddPoints(50);
                            std::cout << "+Enemy killed 50 points added\n";
                        }
                    }
                }

                if (CheckCollisionRecs(playerHitbox, enemyHitbox) && enemy->canTakeDamage()) {
                    player.takeDamage(enemy->getDamage());
                }
            }

            enemySpawnTimer += deltaTime;
            if (enemySpawnTimer >= spawnInterval) {
                enemySpawnTimer = 0.0f;
                for (int i = 0; i < entityAmount; i++) {
                    float x = GetRandomValue(0, screenWidth - 64);
                    float y = GetRandomValue(0, screenHeight - 64);
                    enemies.push_back(new Enemy(x, y, eHP, eDMG, eSpeed));
                }
                currentLevel = entityAmount - 4;      // Update current level
                levelDisplayTimer = levelDisplayDuration; // Reset timer to show level text
                std::cout << "Level " << currentLevel << '\n';
                std::cout << "Enemies spawned: " << entityAmount << '\n';
                if(currentLevel == 1) spawnInterval = 10.0f;
                entityAmount++;
                eHP += 5;
                eDMG++;
                eSpeed += 5;
            }

            // Update level display timer
            if (levelDisplayTimer > 0.0f) {
                levelDisplayTimer -= deltaTime;
                if (levelDisplayTimer < 0.0f) levelDisplayTimer = 0.0f;
            }

            if (!player.isAlive()) {
                isGameOver = true;
                for (auto& enemy : enemies)
                    delete enemy;
                enemies.clear();
                Enemy::UnloadAssets();
                save.SaveRun(playerID, score.GetScore());
                GameOver(score);
                break;
            }
        }

        // --- Drawing ---
        BeginDrawing();
        ClearBackground(BLACK);

        if (isGameOver) {
            GameOver(score);
        } else {
            map.Draw();
            player.Draw();
            score.Draw();

            int currentHealth = player.getHealth();
            float healthPercent = (float)currentHealth / (float)maxHealth;
            Color healthColor = (healthPercent > 0.5f) ? GREEN : (healthPercent > 0.25f) ? ORANGE : RED;

            DrawRectangle(barX, barY - 10, barWidth, barHeight, DARKGRAY);
            DrawRectangle(barX, barY - 10, (int)(barWidth * healthPercent), barHeight, healthColor);
            DrawRectangleLines(barX, barY - 10, barWidth, barHeight, BLACK);
            DrawText(TextFormat("HP: %d / %d", currentHealth, maxHealth), barX * 2, barY - 10, 20, WHITE);
            DrawText(TextFormat("Attack: %d", player.getDamage()), barX, barY + 30, 20, WHITE);

            for (auto& enemy : enemies)
                if (enemy->isAlive()) enemy->Draw();

            player.DrawSlash();
            player.DrawSkill();

            // Draw particles
            for (auto& effect : deathEffects)
                effect.Draw();
            // Draw smoke particles (last to make them overlay on top)

            // Draw level text centered for 1 second
            if (levelDisplayTimer > 0.0f) {
                const char* levelText = TextFormat("Wave %d", currentLevel);
                int fontSize = 40;
                int textWidth = MeasureText(levelText, fontSize);
                int textHeight = fontSize;

                int centerX = screenWidth / 2 - textWidth / 2;
                int centerY = screenHeight / 2 - textHeight / 2;

                DrawText(levelText, centerX, centerY, fontSize, GOLD);
            }

            if (debug)
                DrawText(TextFormat("FPS: %d", GetFPS()), 20, 80, 10, GREEN);
        }

        DrawText("Left click : Normal Attack", GetScreenWidth() / 2 - 100, barY - 10, 10, LIGHTGRAY);
        DrawText("Right click : Special Attack", GetScreenWidth() / 2 - 100, barY, 10, LIGHTGRAY);
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.3f)); // Darken screen
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(RED, 0.1f));   // Reddish overlay
        for (auto& p : smokeParticles) {
            float alpha = GetRandomFloat(0.3f,0.6f);
            //DrawCircleV(p.position, p.size, Fade(GRAY, alpha * 0.8f));
            DrawRectangle(
                p.position.x, p.position.y,
                p.size, p.size,
                Fade(RED, alpha)
            );
        }
        EndDrawing();
    }

    // Free all enemies
    for (auto& enemy : enemies)
        delete enemy;
    enemies.clear();
    Enemy::UnloadAssets();
    map.Unload();
}