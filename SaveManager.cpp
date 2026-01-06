#include "SaveManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

bool SaveManager::saveGame(const QString& filename, 
                           int jourActuel, 
                           const QDate& currentDate,
                           club* monClub, 
                           Championnat* currentChamp, 
                           profil* profilJoueur,
                           const std::map<std::string, Championnat>& map_champ) 
{
    if (!monClub || !currentChamp) return false;

    QJsonObject root;
    root["jourActuel"] = jourActuel;
    root["date"] = currentDate.toString(Qt::ISODate);
    root["monClub"] = QString::fromStdString(monClub->getnom());
    root["monChampionnat"] = QString::fromStdString(currentChamp->getnom());
    
    if(profilJoueur) {
        root["argent"] = (qint64)profilJoueur->get_argent();
        root["managerLevel"] = profilJoueur->get_manager_level();
        root["managerXP"] = profilJoueur->get_manager_xp();
    }

    QJsonArray leaguesArray;
    for(const auto& pair : map_champ) {
        QJsonObject leagueObj;
        leagueObj["nom"] = QString::fromStdString(pair.first);

        QJsonArray clubsArray;
        const std::vector<club>* clubs = pair.second.getliste();
        if(clubs) {
            for(const auto& c : *clubs) {
                QJsonObject clubObj;
                clubObj["nom"] = QString::fromStdString(c.getnom());
                clubObj["points"] = c.getnbpoint();
                clubObj["gagne"] = c.get_gagne();
                clubObj["nul"] = c.get_nul();
                clubObj["perdu"] = c.get_perdu();
                clubObj["bp"] = c.get_bp();
                clubObj["bc"] = c.get_bc();
                clubsArray.append(clubObj);
            }
        }
        leagueObj["clubs"] = clubsArray;
        leaguesArray.append(leagueObj);
    }
    root["championnats"] = leaguesArray;

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) return false;
    file.write(QJsonDocument(root).toJson());
    return true;
}

SaveManager::LoadResult SaveManager::loadGame(const QString& filename, std::map<std::string, Championnat>& map_champ) {
    LoadResult res;
    res.success = false;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) return res;

    QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();

    res.jourActuel = root["jourActuel"].toInt();
    if(root.contains("date")) res.currentDate = QDate::fromString(root["date"].toString(), Qt::ISODate);
    else res.currentDate = QDate(2025, 8, 8);

    res.clubName = root["monClub"].toString().toStdString();
    res.leagueName = root["monChampionnat"].toString().toStdString();
    res.argent = root["argent"].toVariant().toLongLong();
    res.managerLvl = root["managerLevel"].toInt();
    if(res.managerLvl == 0) res.managerLvl = 65;
    res.managerXP = root["managerXP"].toInt();

    // Mise à jour des stats des championnats
    QJsonArray leaguesArray = root["championnats"].toArray();
    for(const auto& lValue : leaguesArray) {
        QJsonObject lObj = lValue.toObject();
        std::string lName = lObj["nom"].toString().toStdString();

        if(map_champ.find(lName) != map_champ.end()) {
            Championnat& champ = map_champ[lName];
            std::vector<club>* clubs = champ.getliste();

            QJsonArray clubsArray = lObj["clubs"].toArray();
            for(const auto& cValue : clubsArray) {
                QJsonObject cObj = cValue.toObject();
                std::string cName = cObj["nom"].toString().toStdString();

                for(auto& realClub : *clubs) {
                    if(realClub.getnom() == cName) {
                        realClub.set_stats(
                            cObj["points"].toInt(),
                            cObj["gagne"].toInt(),
                            cObj["nul"].toInt(),
                            cObj["perdu"].toInt(),
                            cObj["bp"].toInt(),
                            cObj["bc"].toInt()
                        );
                        break;
                    }
                }
            }
        }
    }
    res.success = true;
    return res;
}