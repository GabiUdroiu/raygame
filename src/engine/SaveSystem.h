#pragma once
#include <string>
#include <vector>

struct Run {
    int id;
    int score;
};

class SaveSystem {
private:
    std::string filename;
    int lastID;
    int runCount;
    std::vector<Run> runs;

public:
    SaveSystem(const std::string& file = "score_data.txt");
    int GeneratePlayerID();
    void SaveRun(int id, int score);
    void Load();
    void Save();
};
