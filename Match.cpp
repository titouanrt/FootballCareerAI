#include "Match.h"
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iostream>

match::match(club* d, club* e) : dom{d}, ext{e} {}

// Fonction locale pour simuler une distribution de Poisson
// (Loi statistique standard pour les scores de football)
static int prevoir_buts_poisson(double xG) {
    if (xG <= 0) return 0;
    
    // Algorithme de Knuth pour générer un nombre aléatoire selon Poisson
    double L = std::exp(-xG);
    double p = 1.0;
    int k = 0;
    
    // Limite de sécurité (très rare d'aller au-delà de 15 buts)
    while (p > L && k < 15) {
        k++;
        p *= (random_int(0, 10000) / 10000.0); // Random uniforme [0, 1]
    }
    return k - 1;
}

// On garde cette fonction pour la compatibilité avec le header, 
// mais on l'adapte pour utiliser notre nouvelle logique si appelée directement.
int match::simuler_buts(float p_att, float p_def) {
    // Estimation rapide si utilisée hors contexte match
    double diff = p_att - p_def;
    double xG = 1.25 * std::exp(0.03 * diff);
    return prevoir_buts_poisson(xG);
}
MatchResult match::jouer_match() {   
    float s_dom = dom->getscore(); if(s_dom <= 10) s_dom = 60; // Sécurité augmentée à 60
    float s_ext = ext->getscore(); if(s_ext <= 10) s_ext = 60;

    // --- PARAMÈTRES DE SIMULATION ---
    
    // Moyennes xG (Expected Goals) de base
    // Domicile légèrement favorisé (historiquement ~1.4 vs ~1.1)
    // On reste modéré pour ne pas trop avantager domicile.
    double base_xG_dom = 1.30; 
    double base_xG_ext = 1.10;
    
    // Bonus Domicile sur la note (+3 points = avantage léger)
    double home_advantage_rating = 3.0; 

    // Facteur de forme du jour (Aléatoire entre -4 et +4)
    // Permet aux petites équipes de créer l'exploit si la grosse équipe est "dans un mauvais jour"
    double form_dom = (random_int(0, 80) / 10.0) - 4.0;
    double form_ext = (random_int(0, 80) / 10.0) - 4.0;

    // Calcul de la différence de niveau ajustée
    double total_score_dom = s_dom + home_advantage_rating + form_dom;
    double total_score_ext = s_ext + form_ext;

    double diff = total_score_dom - total_score_ext;

    // Scaling Factor : 
    // 0.035 signifie que 10 points d'écart = ~40% de chances de marquer en plus.
    // Votre ancien "0.3" donnait 2000% de chances en plus (trop violent).
    double scaling = 0.06;

    // Calcul des Expected Goals (xG)
    // Si diff > 0 (Dom meilleur), l'exposant est positif pour Dom, négatif pour Ext
    double xG_dom = base_xG_dom * std::exp(scaling * diff);
    double xG_ext = base_xG_ext * std::exp(scaling * -diff); // Inverse la différence pour l'extérieur

    // Simulation du score réel via Poisson
    int but_dom = prevoir_buts_poisson(xG_dom);
    int but_ext = prevoir_buts_poisson(xG_ext);

    // --- CONSTRUCTION DU RÉSULTAT ---
    MatchResult res;
    res.dom = dom; res.ext = ext;
    res.score_dom = but_dom; res.score_ext = but_ext;

    // Génération des buteurs
    for(int i=0; i<but_dom; ++i) res.goals.push_back({random_int(1, 89), dom->get_random_buteur(), dom->getnom()});
    for(int i=0; i<but_ext; ++i) res.goals.push_back({random_int(1, 89), ext->get_random_buteur(), ext->getnom()});
    std::sort(res.goals.begin(), res.goals.end());

    // Mise à jour des classements
    if (but_dom > but_ext) { 
        dom->update_match_stats(but_dom, but_ext, 3); 
        ext->update_match_stats(but_ext, but_dom, 0); 
    } 
    else if (but_dom < but_ext) { 
        dom->update_match_stats(but_dom, but_ext, 0); 
        ext->update_match_stats(but_ext, but_dom, 3); 
    } 
    else { 
        dom->update_match_stats(but_dom, but_ext, 1); 
        ext->update_match_stats(but_ext, but_dom, 1); 
    }

    std::stringstream ss;
    ss << dom->getnom() << " " << but_dom << "-" << but_ext << " " << ext->getnom();
    res.summary = ss.str();
    
    return res;
}