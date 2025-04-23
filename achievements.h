#ifndef ACHIEVEMENTS_H
#define ACHIEVEMENTS_H

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

//For Players
#include "players.h"


struct Achievement {
    std::string name;
    std::string description;
    bool unlocked;
};

struct Statistic {
    std::string name;
    int count;
};

class Achievements {
public:
    static void loadProgress();
    static void saveProgress();
    void checkAchievements(std::unique_ptr<Player>& humanPlayer, std::unique_ptr<Player>& aiPlayer, int winningPoints);
    static void updateStatistics(const std::string& key);

    // Retrieval functions
    static std::vector<Achievement> getAchievements();
    static std::unordered_map<std::string, Statistic> getStatistics();
};

extern std::vector<Achievement> achievements;
extern std::unordered_map<std::string, Statistic> statistics;

#endif
