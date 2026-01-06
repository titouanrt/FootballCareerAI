#ifndef PROFIL_H
#define PROFIL_H

#include "Club.h"
#include "Championnat.h"
#include "Saison.h"

class profil {
public:
    profil(long long a, club* c, Championnat* ch);
    profil(long long a, club* c, Championnat* ch, int lvl, int xp);

    club* getclub();
    void changer_saison(saison s);
    saison& getsaison();
    
    void add_argent(long long amount);
    bool spend_argent(long long amount);
    long long get_argent() const;

    int get_manager_level() const;
    int get_manager_xp() const;

    bool update_rating(int points_match);

private:
    saison saison_en_cours; 
    long long argent; 
    club* club_choosed; 
    Championnat* champ_choosed;
    
    int manager_level; 
    int manager_xp;
};

#endif