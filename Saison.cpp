#include "Saison.h"

saison::saison(){}

saison::saison(Championnat& le_champ) : p_champ{&le_champ} {
    std::vector<club>* clubs = le_champ.getliste();
    int n = clubs->size(); if(n == 0) return;
    
    // Si nombre impair d'équipes, on en ignore une pour l'instant (algo Round-Robin classique)
    if (n % 2 != 0) n--; 
    
    int nb_journees_phase = n - 1; 
    int match_par_journee = n / 2;

    // --- PHASE 1 : MATCHS ALLER ---
    for (int journee = 0; journee < nb_journees_phase; journee++) {
        for (int m = 0; m < match_par_journee; m++) {
            int id_dom = (journee + m) % (n - 1);
            int id_ext = (n - 1 - m + journee) % (n - 1);
            if (m == 0) id_ext = n - 1;
            
            liste_match.push_back(match(&(*clubs)[id_dom], &(*clubs)[id_ext]));
        }
    }

    // --- PHASE 2 : MATCHS RETOUR ---
    // On reprend exactement la même logique, mais on inverse Domicile et Extérieur
    // dans la création du match.
    for (int journee = 0; journee < nb_journees_phase; journee++) {
        for (int m = 0; m < match_par_journee; m++) {
            int id_dom = (journee + m) % (n - 1);
            int id_ext = (n - 1 - m + journee) % (n - 1);
            if (m == 0) id_ext = n - 1;

            // Inversion : ext devient dom, dom devient ext
            liste_match.push_back(match(&(*clubs)[id_ext], &(*clubs)[id_dom]));
        }
    }

    this->match_par_journee = match_par_journee; 
    // On double le nombre de journées total (Aller + Retour)
    this->nb_journees = nb_journees_phase * 2; 
}

std::vector<match>& saison::getlist() { return liste_match; }
int saison::getnbmatch() { return match_par_journee; }
int saison::getnbjour() { return nb_journees; }