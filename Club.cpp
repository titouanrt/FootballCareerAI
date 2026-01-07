#include "Club.h"
#include "GameUtils.h"
#include <algorithm>
#include <set>
club::club() : nbpoint{0}, score{0} {}
club::club(std::string le_nom) : nom{le_nom}, nbpoint{0}, score{0} {}

std::string club::getnom() const { return nom; }
float club::getscore() const { return score; }
const std::vector<player>& club::getEffectif() const { return effectif; }

SquadFormation club::get_best_11() {
    SquadFormation squad;
    std::set<int> usedIds; // Pour éviter les doublons

    // 1. Lambda pour récupérer les joueurs disponibles triés par niveau
    auto get_available = [&](const std::vector<std::string>& valid_pos) -> std::vector<player> {
        std::vector<player> candidates;
        for(const auto& p : effectif) {
            if(usedIds.count(p.getid())) continue;
            // Vérifie si le poste du joueur est dans la liste demandée
            for(const auto& pos : valid_pos) {
                if(p.getposte() == pos) {
                    candidates.push_back(p);
                    break;
                }
            }
        }
        std::sort(candidates.begin(), candidates.end()); // Tri décroissant (opérateur < surchargé)
        return candidates;
    };

    // 2. Lambda pour sélectionner le meilleur d'une liste et l'ajouter
    auto pick_best = [&](const std::vector<std::string>& positions, std::vector<player>& targetVec) -> bool {
        std::vector<player> candidates = get_available(positions);
        if(!candidates.empty()) {
            player best = candidates[0];
            targetVec.push_back(best);
            usedIds.insert(best.getid());
            return true;
        }
        return false;
    };

    // --- GARDIEN ---
    if (!pick_best({"GK"}, squad.subs)) { 
        // Astuce: on utilise squad.subs temporairement ou on assigne direct
        // Ici on assigne manuellement car squad.gk est un objet unique, pas un vecteur
        std::vector<player> gks = get_available({"GK"});
        if(!gks.empty()) {
            squad.gk = gks[0];
            usedIds.insert(squad.gk.getid());
        }
    } else {
        // Si pick_best a mis dans subs par erreur (car j'ai passé subs), on corrige
        // Mieux vaut le faire manuellement pour le GK :
        squad.subs.clear(); // Nettoyage si erreur
        std::vector<player> gks = get_available({"GK"});
        if(!gks.empty()) {
            squad.gk = gks[0];
            usedIds.insert(squad.gk.getid());
        }
    }

    // --- DÉFENSE (Ordre : LB, CB, CB, RB) ---
    // 1. Latéral Gauche
    if(!pick_best({"LB", "LWB"}, squad.defs)) pick_best({"RB", "RWB", "CB"}, squad.defs); // Fallback
    
    // 2. Défenseurs Centraux (x2)
    for(int i=0; i<2; ++i) {
        if(!pick_best({"CB"}, squad.defs)) pick_best({"CDM", "LB", "RB"}, squad.defs);
    }

    // 3. Latéral Droit
    if(!pick_best({"RB", "RWB"}, squad.defs)) pick_best({"LB", "LWB", "CB"}, squad.defs);


    // --- MILIEU (3 joueurs) ---
    // On cherche d'abord des profils axiaux
    for(int i=0; i<3; ++i) {
        if(!pick_best({"CDM", "CM", "CAM"}, squad.mids)) {
            // Fallback : Milieux latéraux ou ailiers reconvertis
            pick_best({"LM", "RM", "LW", "RW"}, squad.mids);
        }
    }

    // --- ATTAQUE (Ordre : LW, ST, RW) ---
    // 1. Ailier Gauche
    if(!pick_best({"LW", "LM"}, squad.atts)) pick_best({"RW", "RM", "ST", "CF"}, squad.atts);

    // 2. Buteur (Centre)
    if(!pick_best({"ST", "CF"}, squad.atts)) pick_best({"LW", "RW", "CAM"}, squad.atts);

    // 3. Ailier Droit
    if(!pick_best({"RW", "RM"}, squad.atts)) pick_best({"LW", "LM", "ST", "CF"}, squad.atts);


    // --- REMPLAÇANTS ---
    // On met tout le reste dans les subs
    for(const auto& p : effectif) {
        if(usedIds.find(p.getid()) == usedIds.end()) {
            squad.subs.push_back(p);
        }
    }
    
    // On trie le banc par niveau pour avoir les meilleurs remplaçants en premier
    std::sort(squad.subs.begin(), squad.subs.end());

    return squad;
}

std::string club::get_random_buteur() {
    if(effectif.empty()) return "Inconnu";
    for(int i=0; i<5; i++) {
        int idx = random_int(0, effectif.size() - 1);
        std::string p = effectif[idx].getposte();
        if(p == "ST" || p == "CF" || p == "LW" || p == "RW" || p == "CAM" || p == "RM" || p == "LM") return effectif[idx].getnom();
    }
    return effectif[random_int(0, effectif.size() - 1)].getnom();
}

void club::ajouter_joueur(player p) {
    for(const auto& existing : effectif) {
        if(existing.getid() == p.getid()) return; 
    }
    effectif.push_back(p);
    update_score();
}

void club::remove_player(int id_joueur) {
    for(auto it = effectif.begin(); it != effectif.end(); ++it) {
        if(it->getid() == id_joueur) {
            effectif.erase(it);
            break;
        }
    }
    update_score();
}

void club::update_match_stats(int buts_marques, int buts_encaisse, int points) {
    bp += buts_marques; bc += buts_encaisse; nbpoint += points;
    if(points == 3) gagne++; else if(points == 1) nul++; else perdu++;
}

int club::getnbpoint() const { return nbpoint; }
int club::get_gagne() const { return gagne; }
int club::get_nul() const { return nul; }
int club::get_perdu() const { return perdu; }
int club::get_bp() const { return bp; } 
int club::get_bc() const { return bc; }
int club::get_diff() const { return bp - bc; } 

void club::set_stats(int p, int g, int n, int l, int b_p, int b_c) {
    nbpoint = p; gagne = g; nul = n; perdu = l; bp = b_p; bc = b_c;
}

void club::update_score() {
    float somme{};
    if (effectif.empty()) { score = 0; return; }
    for (int i{}; i<effectif.size(); i++) somme += effectif[i].getlevel();
    score = somme/effectif.size();
}

std::vector<std::string> club::gestion_fin_saison() {
    std::vector<std::string> retraites;
    
    // On utilise un itérateur pour pouvoir supprimer des éléments en bouclant
    for (auto it = effectif.begin(); it != effectif.end(); ) {
        it->vieillir(); // +1 an
        
        bool part = false;
        int age = it->getage();

        // Logique de Retraite
        // À partir de 33 ans, le risque augmente chaque année
        if (age >= 33) {
            int chance = (age - 32) * 12; 
            // 33 ans = 12%
            // 34 ans = 24%
            // ...
            // 38 ans = 72%
            
            if (chance > 95) chance = 95; // On garde toujours 5% de chance qu'il continue (Légende)
            
            if (random_int(0, 100) < chance) part = true;
        }

        if (part) {
            retraites.push_back(it->getnom());
            it = effectif.erase(it); // Suppression du vecteur
        } else {
            ++it;
        }
    }
    update_score(); // Recalcule le niveau global du club après les départs
    return retraites;
}