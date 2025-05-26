#include <raylib.h>
#include "loading.h"
#include "Score.h"
void Loading(void)
{

    const short int screenWidth = GetScreenWidth();
    const short int screenHeight = GetScreenHeight();
    
    for(char bar = 0; bar < 100; bar++) {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawRectangle(screenWidth / 4, screenHeight / 2, screenWidth / 2, 30, DARKGRAY);
        DrawRectangle(screenWidth / 4, screenHeight / 2, (screenWidth / 2) * (bar / 100.0f), 30, GREEN);

        EndDrawing();
    }
}

void GameOver(Score score)
{
    while (true)
    {
        BeginDrawing();
        // DrawTexturePro(
        // menuBackground,
        // (Rectangle){ 0, 0, (float)menuBackground.width, (float)menuBackground.height }, // source
        // (Rectangle){ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }, // destination
        // (Vector2){ 0, 0 }, // origin
        // 0.0f, // rotation
        // WHITE // tint
        // );
        const char* msg = "GAME OVER";
        int textSize = MeasureText(msg, 60);
        int textWidth = MeasureText("Score: 00000", 60);
        Color color = GOLD;
        DrawRectangle(
            GetScreenWidth() / 2 - 300,
            GetScreenHeight() / 2 - 200,
            600,
            300,
            BROWN
        );
        // DrawRectangleLines(
        //     GetScreenWidth() / 2 - 300,
        //     GetScreenHeight() / 2 - 150,
        //     600,
        //     300,
        //     RED
        // );
        DrawText(TextFormat(
            "Score: %05i", score.GetScore()), 
            (GetScreenWidth() - textWidth) / 2,
            GetScreenHeight() / 2 - 140, 
            60, 
            color
        );
        DrawText(msg, (GetScreenWidth() - textSize) / 2, GetScreenHeight() / 2 - 30, 60, RED);
        DrawText("Press ENTER to main menu", GetScreenWidth() / 2 - 120, GetScreenHeight() / 2 + 70, 20, WHITE);

        EndDrawing();

        if (IsKeyPressed(KEY_ESCAPE)) {
            break;      // Exit from any remaining logic
        }

        if (IsKeyPressed(KEY_ENTER)) {
            break;  // Break the GameOver loop to restart the game
        }
    }
}