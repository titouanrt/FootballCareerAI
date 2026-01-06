#include "DataManager.h"
#include <QFile>
#include <QCoreApplication>
#include <QDebug>
#include <set>
#include <iostream>

std::vector<std::string> DataManager::parseCSVLine(const std::string& line) {
    std::vector<std::string> result;
    std::string cell;
    bool insideQuotes = false;
    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        if (c == '"') {
            if (insideQuotes && i + 1 < line.length() && line[i + 1] == '"') {
                cell += '"'; i++;
            } else { insideQuotes = !insideQuotes; }
        } else if (c == ',' && !insideQuotes) {
            result.push_back(cell); cell.clear();
        } else { cell += c; }
    }
    result.push_back(cell);
    return result;
}

void DataManager::chargerDonnees(std::map<std::string, Championnat>& map_champ) {
    map_champ.clear(); 

    QString csvPath;
    if(QFile("EAFC26-Men.csv").exists()) csvPath = "EAFC26-Men.csv";
    else if(QFile("build/EAFC26-Men.csv").exists()) csvPath = "build/EAFC26-Men.csv";
    else csvPath = QCoreApplication::applicationDirPath() + "/EAFC26-Men.csv";
    
    QFile file(csvPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Erreur: Impossible de trouver le CSV à " << csvPath;
        return;
    }
    
    if (!file.atEnd()) file.readLine(); // Skip header

    std::set<int> processedIds; 

    while (!file.atEnd()) {
        QByteArray raw = file.readLine();
        std::string lineStr = raw.toStdString();
        
        if (!lineStr.empty() && lineStr.back() == '\n') lineStr.pop_back();
        if (!lineStr.empty() && lineStr.back() == '\r') lineStr.pop_back();

        std::vector<std::string> cols = parseCSVLine(lineStr);

        if (cols.size() < 50) continue; 

        try {
            int id = std::stoi(cols[0]);
            if(processedIds.find(id) != processedIds.end()) continue;
            processedIds.insert(id);

            std::string nom = cols[2];
            int note = std::stoi(cols[4]);
            
            int pac = std::stoi(cols[5]);
            int sho = std::stoi(cols[6]);
            int pas = std::stoi(cols[7]);
            int dri = std::stoi(cols[8]);
            int def = std::stoi(cols[9]);
            int phy = std::stoi(cols[10]);

            std::string poste = cols[40];
            int age = std::stoi(cols[47]);
            std::string ligue = cols[49]; 
            std::string clubNom = cols[50];
            
            std::string url = "";
            if (cols.size() > 58) url = cols[58]; 
            else if (!cols.empty()) url = cols.back(); 

            if(clubNom.empty() || poste.empty()) continue;

            if(map_champ.find(ligue) == map_champ.end()) {
                map_champ[ligue] = Championnat(ligue);
            }
            
            std::vector<club>* listeClubs = map_champ[ligue].getliste();
            club* c = nullptr;
            for(auto& existingClub : *listeClubs) {
                if(existingClub.getnom() == clubNom) {
                    c = &existingClub;
                    break;
                }
            }
            if(!c) {
                club nouveauClub(clubNom);
                listeClubs->push_back(nouveauClub);
                c = &listeClubs->back();
            }

            player p(id, nom, note, age, poste, pac, sho, pas, dri, def, phy, url);
            c->ajouter_joueur(p);

        } catch (...) { continue; }
    }
    file.close();
    
    for (auto& [nom, champ] : map_champ) {
        champ.update_prestige();
    }
}