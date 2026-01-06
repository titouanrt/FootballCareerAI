#include "Saison.h"

saison::saison(){}

saison::saison(Championnat& le_champ) : p_champ{&le_champ} {
    std::vector<club>* clubs = le_champ.getliste();
    int n = clubs->size(); if(n == 0) return;
    if (n % 2 != 0) n--; 
    int nb_journees_phase = n - 1; int match_par_journee = n / 2;
    for (int journee = 0; journee < nb_journees_phase; journee++) {
        for (int m = 0; m < match_par_journee; m++) {
            int id_dom = (journee + m) % (n - 1);
            int id_ext = (n - 1 - m + journee) % (n - 1);
            if (m == 0) id_ext = n - 1;
            liste_match.push_back(match(&(*clubs)[id_dom], &(*clubs)[id_ext]));
        }
    }
    this->match_par_journee = match_par_journee; this->nb_journees = nb_journees_phase;
}

std::vector<match>& saison::getlist() { return liste_match; }
int saison::getnbmatch() { return match_par_journee; }
int saison::getnbjour() { return nb_journees; }