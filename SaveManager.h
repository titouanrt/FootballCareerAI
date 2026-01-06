#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <QJsonObject>
#include <map>
#include <string>
#include <QDate>

// REMPLACEMENT DE classes.h PAR LES HEADERS SPECIFIQUES
#include "Club.h" 
#include "Championnat.h"
#include "Saison.h"
#include "Profil.h"

class SaveManager {
public:
    static bool saveGame(const QString& filename, 
                         int jourActuel, 
                         const QDate& currentDate,
                         club* monClub, 
                         Championnat* currentChamp, 
                         profil* profilJoueur,
                         const std::map<std::string, Championnat>& map_champ);

    struct LoadResult {
        bool success;
        int jourActuel;
        QDate currentDate;
        std::string clubName;
        std::string leagueName;
        long long argent;
        int managerLvl;
        int managerXP;
    };

    static LoadResult loadGame(const QString& filename, std::map<std::string, Championnat>& map_champ);
};

#endif // SAVEMANAGER_H