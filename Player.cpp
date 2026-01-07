#include "Player.h"
#include <cmath>

player::player() {}

player::player(int i, std::string n, int l, int a, std::string p, 
       int pac, int sho, int pas, int dri, int def, int phy, std::string u) :
    id{i}, nom{n}, level{l}, age{a}, poste{p}, 
    pac{pac}, sho{sho}, pas{pas}, dri{dri}, def{def}, phy{phy}, url_img{u} {}

int player::getid() const { return id; } 
int player::getlevel() const { return level; }
int player::getage() const { return age; }
std::string player::getposte() const { return poste; }
std::string player::getnom() const { return nom; }
std::string player::geturl() const { return url_img; }

int player::get_pac() const { return pac; }
int player::get_sho() const { return sho; }
int player::get_pas() const { return pas; }
int player::get_dri() const { return dri; }
int player::get_def() const { return def; }
int player::get_phy() const { return phy; }

std::string player::toString() const { return nom + " (" + poste + ") - " + std::to_string(level); }

bool player::operator<(const player& other) const { return level > other.level; } 

double player::calculate_value(float team_avg_score) const {
    // Formule de base exponentielle selon le niveau (Rating)
    double base_value = 350000.0 * std::pow(1.19, level - 60); 
    if (base_value < 50000) base_value = 50000; 

    // --- MODIFICATION AGE (LINÉAIRE) ---
    // "Plus ils sont jeunes, plus ils sont chers"
    // Modèle linéaire centré autour de 26 ans (Peak théorique décalé vers la jeunesse pour le potentiel)
    // Age 16 : Facteur ~1.40 (+40%)
    // Age 26 : Facteur 1.00 (Standard)
    // Age 35 : Facteur ~0.64 (-36%)
    double age_factor = 1.1 + (28.0 - (double)age) * 0.08;
    
    // Bornes de sécurité pour éviter des valeurs négatives ou absurdes
    if (age_factor < 0.3) age_factor = 0.3; // Minimum pour les vieux vétérans
    //if (age_factor > 4) age_factor = 4; // Maximum pour les très jeunes prodiges

    // --- MODIFICATION POSITION (IMPACT RÉDUIT) ---
    double pos_factor = 1.0;
    if (poste == "GK") pos_factor = 0.85;  // Était 0.70
    else if (poste == "CB" || poste == "LB" || poste == "RB" || poste == "LWB" || poste == "RWB") pos_factor = 0.90; // Était 0.85
    else if (poste == "CDM" || poste == "CM" || poste == "CAM") pos_factor = 1.00; // Standard
    else if (poste == "ST" || poste == "CF" || poste == "LW" || poste == "RW" || poste == "RM" || poste == "LM") pos_factor = 1.20; // Était 1.40

    // Facteurs mineurs inchangés
    double club_factor = 1.0 + ((team_avg_score - 72.0) * 0.03); 
    double fluctuation = 0.95 + ((id % 11) / 100.0); 

    return base_value * age_factor * pos_factor * club_factor * fluctuation;
}

void player::vieillir() { 
    age++; 
}