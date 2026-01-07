#ifndef CLUB_H
#define CLUB_H

#include <string>
#include <vector>
#include "Player.h"

// Structure nécessaire pour la tactique
struct SquadFormation {
    player gk;
    std::vector<player> defs; 
    std::vector<player> mids; 
    std::vector<player> atts; 
    std::vector<player> subs; 
};

class club {
public:
    club();
    club(std::string le_nom);
    
    std::string getnom() const;
    float getscore() const;
    const std::vector<player>& getEffectif() const;

    SquadFormation get_best_11();
    std::string get_random_buteur();

    void ajouter_joueur(player p);
    void remove_player(int id_joueur);
    void update_match_stats(int buts_marques, int buts_encaisse, int points);
    
    int getnbpoint() const;
    int get_gagne() const;
    int get_nul() const;
    int get_perdu() const;
    int get_bp() const; 
    int get_bc() const;
    int get_diff() const; 

    void set_stats(int p, int g, int n, int l, int b_p, int b_c);
    void update_score();

    // --- C'EST CETTE LIGNE QUI MANQUAIT ---
    std::vector<std::string> gestion_fin_saison(); 
    // --------------------------------------

private:
    std::string nom;
    std::vector<player> effectif;
    float score;
    int nbpoint=0, gagne=0, nul=0, perdu=0, bp=0, bc=0;
};

#endif // CLUB_H