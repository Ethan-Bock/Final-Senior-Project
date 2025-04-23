#include "achievements.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "players.h"

std::vector<Achievement> achievements = {
    {"By the Skin of Your Teeth", "Win against AI with 100 or fewer points more than the AI", false},
    {"Dominating Victory", "Win with more than double the AI's points", false},
    {"Comeback King", "Win after being behind by 500+ points", false}
};

std::unordered_map<std::string, Statistic> statistics = {
    {"Wins against Aggressive AI", {"Wins against Aggressive AI", 0}},
    {"Wins against Cautious AI", {"Wins against Cautious AI", 0}},
    {"Wins against Adaptive AI", {"Wins against Adaptive AI", 0}},
    {"Total Wins against AI", {"Total Wins against AI", 0}},
    {"Total number of Games against AI", {"Total number of Games against AI", 0}}
};





// Load progress from a file
void Achievements::loadProgress() {
    std::ifstream file("progress.json");
    if (!file.is_open()) return;

    nlohmann::json jsonData;
    file >> jsonData;
    file.close();

    // Load achievements
    if (jsonData.contains("achievements") && jsonData["achievements"].is_object()) {
        for (auto& ach : achievements) {
            if (jsonData["achievements"].contains(ach.name) && jsonData["achievements"][ach.name].is_boolean()) {
                ach.unlocked = jsonData["achievements"][ach.name];
            } else {
                ach.unlocked = false; // Default to locked if not found
            }
        }
    }

    // Load statistics
    if (jsonData.contains("statistics") && jsonData["statistics"].is_object()) {
        for (auto& stat : statistics) {
            if (jsonData["statistics"].contains(stat.first) && jsonData["statistics"][stat.first].is_number_integer()) {
                statistics[stat.first].count = jsonData["statistics"][stat.first];
            } else {
                statistics[stat.first].count = 0; // Default to 0 if not found
            }
        }
    }
}

// Save progress to a file
void Achievements::saveProgress() {
    std::ofstream file("progress.json");
    nlohmann::json jsonData;

    for (const auto& ach : achievements) {
        jsonData["achievements"][ach.name] = ach.unlocked;
    }

    for (const auto& stat : statistics) {
        jsonData["statistics"][stat.first] = stat.second.count;
    }

    file << jsonData.dump(4);
    file.close();
}

// Check if an achievement should be unlocked
void Achievements::checkAchievements(std::unique_ptr<Player>& humanPlayer, std::unique_ptr<Player>& aiPlayer, int winningPoints) {
    int playerScore = humanPlayer->getHardPoints();  // Retrieve player's score
    int aiScore = aiPlayer->getHardPoints();         // Retrieve AI's score

    // Check for "By the Skin of Your Teeth" achievement
    if (!achievements[0].unlocked && (playerScore - aiScore) <= 100 && (playerScore > aiScore)) {
        achievements[0].unlocked = true;
    }

    // Check for "Dominating" achievement (winning with at least double the AI's score)
    if (!achievements[1].unlocked && playerScore >= aiScore * 2) {
        achievements[1].unlocked = true;
    }

    // Retrieve the player's score history
    const std::vector<std::pair<int, bool>>& playerHistory = humanPlayer->getHistory();
    // Retrieve the AI's score history
    //const std::vector<std::pair<int, bool>>& aiHistory = aiPlayer->getHistory();

    // Ensure there are at least 2 historical entries before accessing the second-most recent score
    int previousScore = (playerHistory.size() >= 2) ? playerHistory[playerHistory.size() - 2].first : playerScore;

    // Check for "Comeback King" achievement (winning despite a large deficit)
    if (!achievements[2].unlocked && (aiScore - previousScore >= 500) && (playerScore > aiScore) && (aiScore > winningPoints)){
        achievements[2].unlocked = true;
    }
}

// Update statistics
void Achievements::updateStatistics(const std::string& key) {
    if (statistics.find(key) != statistics.end()) {
        statistics[key].count++;
    } else {
        std::cerr << "Error: Statistic key '" << key << "' not found.\n";
    }
}


std::vector<Achievement> Achievements::getAchievements() {
    loadProgress();  // Ensure the latest data is loaded
    return achievements;
}

// Retrieve updated statistics
std::unordered_map<std::string, Statistic> Achievements::getStatistics() {
    loadProgress();  // Ensure the latest data is loaded
    return statistics;
}