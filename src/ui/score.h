#pragma once
#include <raylib.h>

class Score {
private:
    int score;
    int fontSize;
    Color color;

    float timer;           // accumulate time
    float incrementInterval;  // how often to add points (in seconds)
    int pointsPerInterval;    // how many points to add each interval

public:
    Score(int startScore = 0);

    void Update(float deltaTime);
    void AddPoints(int points);
    void Reset();
    void Draw() const;
    int GetScore() const;
};
