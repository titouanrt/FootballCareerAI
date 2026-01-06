#include "Profil.h"

profil::profil(long long a, club* c, Championnat* ch) 
    : argent{a}, club_choosed{c}, champ_choosed{ch}, manager_level{73}, manager_xp{0} {}
    
profil::profil(long long a, club* c, Championnat* ch, int lvl, int xp) 
    : argent{a}, club_choosed{c}, champ_choosed{ch}, manager_level{lvl}, manager_xp{xp} {}

club* profil::getclub() { return club_choosed; }
void profil::changer_saison(saison s) { saison_en_cours = s; }
saison& profil::getsaison() { return saison_en_cours; }

void profil::add_argent(long long amount) { argent += amount; }
bool profil::spend_argent(long long amount) {
    if (argent >= amount) { argent -= amount; return true; }
    return false;
}
long long profil::get_argent() const { return argent; }

int profil::get_manager_level() const { return manager_level; }
int profil::get_manager_xp() const { return manager_xp; }

bool profil::update_rating(int points_match) {
    int old_level = manager_level;
    int xp_gain = 0;

    if (points_match == 3) xp_gain = 25;       
    else if (points_match == 1) xp_gain = 5;   
    else xp_gain = -15;                        

    manager_xp += xp_gain;

    if (manager_xp >= 100) {
        manager_level++;
        manager_xp -= 100;
        if (manager_level > 99) manager_level = 99;
    }
    else if (manager_xp < 0) {
        manager_level--;
        manager_xp += 100; 
        if (manager_level < 1) manager_level = 1;
    }

    return (manager_level != old_level);
}