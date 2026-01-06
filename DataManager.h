#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>
#include <vector>
#include <map>
#include "Championnat.h"

class DataManager {
public:
    // Charge les données du CSV et remplit la map des championnats
    static void chargerDonnees(std::map<std::string, Championnat>& map_champ);

private:
    static std::vector<std::string> parseCSVLine(const std::string& line);
};

#endif // DATAMANAGER_H