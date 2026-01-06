#include "Championnat.h"

Championnat::Championnat(std::string le_nom) : nom{le_nom}, prestige_score(0) {}

std::vector<club>* Championnat::getliste() const { return &liste_club; }
int Championnat::getnbclub() const { return liste_club.size(); }
std::string Championnat::getnom() const { return nom; }

void Championnat::update_prestige() {
    if (liste_club.empty()) { prestige_score = 0; return; }
    float total = 0;
    for (const auto& c : liste_club) {
        total += c.getscore();
    }
    prestige_score = total / liste_club.size();
}

float Championnat::get_prestige_score() const { return prestige_score; }

int Championnat::get_tier() const {
    if (prestige_score >= 82) return 1; 
    if (prestige_score >= 76) return 2; 
    return 3; 
}

std::string Championnat::get_tier_name() const {
    int t = get_tier();
    if (t == 1) return "Ligue Majeure (Elite)";
    if (t == 2) return "Ligue Secondaire (Pro)";
    return "Ligue Mineure (Semi-Pro)";
}