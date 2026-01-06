#ifndef SAISON_H
#define SAISON_H

#include <vector>
#include "Match.h"
#include "Championnat.h"

class saison {
public:
    saison();
    saison(Championnat& le_champ);
    std::vector<match>& getlist();
    int getnbmatch();
    int getnbjour();

private:
    std::vector<match> liste_match; 
    Championnat* p_champ;
    int nb_journees = 0; 
    int match_par_journee = 0;
};

#endif // SAISON_H