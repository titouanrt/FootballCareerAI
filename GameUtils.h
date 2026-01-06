#ifndef GAMEUTILS_H
#define GAMEUTILS_H

#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

// Fonction helper globale
inline int random_int(int min, int max) {
    static bool initialized = false;
    if (!initialized) { std::srand(std::time({})); initialized = true; }
    return min + std::rand() % (max - min + 1);
}

struct GoalEvent {
    int minute;
    std::string scorer;
    std::string teamName;
    bool operator<(const GoalEvent& other) const { return minute < other.minute; }
};

class club; // Forward declaration

struct MatchResult {
    std::string summary;
    std::vector<GoalEvent> goals;
    club* dom;
    club* ext;
    int score_dom;
    int score_ext;
};

#endif // GAMEUTILS_H