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
    double base_value = 350000.0 * std::pow(1.19, level - 60); 
    if (base_value < 50000) base_value = 50000; 

    double age_factor = 1.0;
    if (age < 21) age_factor = 1.5; 
    else if (age > 32) age_factor = 0.6; 

    double pos_factor = 1.0;
    if (poste == "GK") pos_factor = 0.70; 
    else if (poste == "CB" || poste == "LB" || poste == "RB") pos_factor = 0.85; 
    else if (poste == "ST" || poste == "CF" || poste == "LW" || poste == "RW") pos_factor = 1.40; 

    double club_factor = 1.0 + ((team_avg_score - 72.0) * 0.03); 
    double fluctuation = 0.95 + ((id % 11) / 100.0); 

    return base_value * age_factor * pos_factor * club_factor * fluctuation;
}