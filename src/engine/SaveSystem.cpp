#include "SaveSystem.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

SaveSystem::SaveSystem(const std::string& file)
    : filename(file), lastID(0), runCount(0) {
    Load();
}

int SaveSystem::GeneratePlayerID() {
    return ++lastID;
}

void SaveSystem::SaveRun(int id, int score) {
    runs.push_back({ id, score });
    runCount++;
    Save();
}

void SaveSystem::Load() {
    if (!std::filesystem::exists(filename)) return;

    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        if (line.find("last_id:") == 0) {
            lastID = std::stoi(line.substr(8));
        } else if (line.find("run_count:") == 0) {
            runCount = std::stoi(line.substr(10));
        } else if (line.find("id=") == 0) {
            int id, score;
            sscanf(line.c_str(), "id=%d,score=%d", &id, &score);
            runs.push_back({ id, score });
        }
    }
}

void SaveSystem::Save() {
    std::ofstream file(filename);
    file << "last_id: " << lastID << "\n";
    file << "run_count: " << runCount << "\n";
    file << "runs:\n";
    for (const auto& run : runs) {
        file << "id=" << run.id << ",score=" << run.score << "\n";
    }
}
