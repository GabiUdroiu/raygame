#include "Score.h"

Score::Score(int startScore)
    : score(startScore),
      timer(0.0f), incrementInterval(0.75f), pointsPerInterval(1) {}

void Score::Update(float deltaTime) {
    timer += deltaTime;
    if (timer >= incrementInterval) {
        score += pointsPerInterval;
        timer = 0.0f;  // reset timer
    }
}

void Score::AddPoints(int points) {
    score += points;
}

void Score::Reset() {
    score = 0;
    timer = 0.0f;
}

void Score::Draw() const {
    int fontSize = 24;
    int margin = 20;
    int textWidth = MeasureText("Score: 00000", fontSize);
    Color color = GOLD;
    DrawText(TextFormat(
        "Score: %05i", score), 
        GetScreenWidth() - textWidth - margin,
        margin, 
        fontSize, 
        color
    );
}

int Score::GetScore() const {
    return score;
}
