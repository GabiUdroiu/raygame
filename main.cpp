#include <raylib.h>
#include "src/ui/loading.h"
#include "src/ui/score.h"
#include "src/game/Player.h"
#include "src/game/Enemy.h"
#include "src/game/Map.h"
#include "game.h"
#include "src/game/Globals.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>

bool debug = false;

enum GameScreen { MENU, GAMEPLAY, LEADERBOARD };
GameScreen currentScreen = MENU;
bool Button(const char* text, Rectangle bounds, Color normal, Color hover, Color pressed, bool pressedState) {
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, bounds);
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    if (clicked) pressedState = !pressedState;

    //DrawRectangleRec(bounds, hovered ? (clicked ? pressed : hover) : normal);
    Color currentColor = hovered ? Fade(RAYWHITE, 1) : Fade(ORANGE,0.6);
    DrawText(text, bounds.x + 10, bounds.y + 5, 20, currentColor);

    return clicked;
}

void ShowLeaderboard(Texture2D lbBG) {
    ClearBackground(BLACK);
    DrawTexturePro(lbBG,
        (Rectangle){ 0, 0, (float)lbBG.width, (float)lbBG.height },
        (Rectangle){ 0, 0, (float)screenWidth, (float)screenHeight },
        (Vector2){ 0, 0 }, 0.0f, WHITE
        );
    //DrawText("Leaderboard", (screenWidth-MeasureText("Leaderboard",40)) / 2, 40, 40, GOLD);

    std::ifstream file("score_data.txt");
    std::vector<std::pair<int, int>> scores;

    if (file.is_open()) {
        std::string line;
        bool readingRuns = false;

        while (std::getline(file, line)) {
            if (line == "runs:") {
                readingRuns = true;
                continue;
            }

            if (readingRuns && line.find("id=") == 0) {
                size_t idPos = line.find("id=");
                size_t scorePos = line.find("score=");

                if (idPos != std::string::npos && scorePos != std::string::npos) {
                    int id = std::stoi(line.substr(idPos + 3, scorePos - idPos - 4));
                    int score = std::stoi(line.substr(scorePos + 6));
                    scores.emplace_back(id, score);
                }
            }
        }

        file.close();
    }

    // Sort high to low
    std::sort(scores.begin(), scores.end(), [](auto& a, auto& b) {
        return a.second > b.second;
    });

    // Show top 10
    int y = 100;
    int rank = 1;
    for (auto& [id, score] : scores) {
        if (rank > 20) break;
        char lineBuf[256];
        snprintf(lineBuf, sizeof(lineBuf), "%d. Run #%d - %d", rank, id, score);
        int textW = MeasureText(lineBuf,20);
        Color textColor = WHITE;
        if (rank == 1) textColor = GOLD;
        else if (rank == 2) textColor = LIGHTGRAY;
        else if (rank == 3) textColor = ORANGE;
        else textColor = WHITE;
        DrawText(lineBuf, (screenWidth - textW) / 2, y, 20, textColor);
        y += 30;
        rank++;
    }

    DrawText("Press ESC to return", (screenWidth-MeasureText("Press ESC to return",20)) / 2, screenHeight - 50, 20, Fade(ORANGE,0.6));

    if (IsKeyPressed(KEY_ESCAPE)) {
        currentScreen = MENU;
    }
}

int main(void)
{
    ChangeDirectory(GetApplicationDirectory());
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(screenWidth, screenHeight, "Robyka sobolankul");
    SetExitKey(0);
    Texture2D menuBackground = LoadTexture("assets/bg.png");
    Texture2D lbBG = LoadTexture("assets/lbbg.png");
    Image icon = LoadImage("assets/ico.png");
    SetWindowIcon(icon);
    SetTargetFPS(1000);

    Loading();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if (currentScreen == MENU) {
            DrawTexturePro(menuBackground,
                (Rectangle){ 0, 0, (float)menuBackground.width, (float)menuBackground.height },
                (Rectangle){ 0, 0, (float)screenWidth, (float)screenHeight },
                (Vector2){ 0, 0 }, 0.0f, WHITE
            );
            int textWidth = MeasureText("Wizard wizar wiza wiz wi w", 40);

            DrawText("Wizard wizar wiza wiz wi w", (screenWidth - textWidth) / 2, 100, 40, GOLD);

            Rectangle startBtn = { screenWidth / 2 - 100, 250, 200, 50 };
            Rectangle leaderboardBtn = { screenWidth / 2 - 100, 320, 200, 50 };
            Rectangle exitBtn = { screenWidth / 2 - 100, 390, 200, 50 };

            if (Button("Start Game", startBtn, DARKGRAY, GRAY, BLACK, false)) {
                isGameOver = false;
                enemySpawnTimer = 0.0f;
                entityAmount = 5;
                deathEffects.clear();
                currentScreen = GAMEPLAY;
            }

            if (Button("Leaderboard", leaderboardBtn, DARKGRAY, GRAY, BLACK, false)) {
                currentScreen = LEADERBOARD;
            }

            if (Button("Exit", exitBtn, DARKGRAY, GRAY, BLACK, false)) {
                break;
            }

        } else if (currentScreen == GAMEPLAY) {
            StartGame();
            currentScreen = MENU;

        } else if (currentScreen == LEADERBOARD) {
            ShowLeaderboard(lbBG);
            if (IsKeyPressed(KEY_ESCAPE)) {
                currentScreen = MENU;
            }
        }

        EndDrawing();
    }

    UnloadImage(icon);
    UnloadTexture(menuBackground);
    CloseWindow();
    return 0;
}
