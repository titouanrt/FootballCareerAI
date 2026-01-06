#include "Club.h"
#include "GameUtils.h"
#include <algorithm>

club::club() : nbpoint{0}, score{0} {}
club::club(std::string le_nom) : nom{le_nom}, nbpoint{0}, score{0} {}

std::string club::getnom() const { return nom; }
float club::getscore() const { return score; }
const std::vector<player>& club::getEffectif() const { return effectif; }

SquadFormation club::get_best_11() {
    SquadFormation squad;
    std::vector<player> all_gk, all_def, all_mid, all_att;

    for(const auto& p : effectif) {
        std::string pos = p.getposte();
        if(pos == "GK") all_gk.push_back(p);
        else if(pos == "LB" || pos == "RB" || pos == "CB" || pos == "LWB" || pos == "RWB") all_def.push_back(p);
        else if(pos == "CDM" || pos == "CM" || pos == "CAM" || pos == "LM" || pos == "RM") all_mid.push_back(p);
        else all_att.push_back(p);
    }

    std::sort(all_gk.begin(), all_gk.end());
    std::sort(all_def.begin(), all_def.end());
    std::sort(all_mid.begin(), all_mid.end());
    std::sort(all_att.begin(), all_att.end());

    if(!all_gk.empty()) { squad.gk = all_gk[0]; all_gk.erase(all_gk.begin()); }
    for(int i=0; i<4 && !all_def.empty(); ++i) { squad.defs.push_back(all_def[0]); all_def.erase(all_def.begin()); }
    for(int i=0; i<3 && !all_mid.empty(); ++i) { squad.mids.push_back(all_mid[0]); all_mid.erase(all_mid.begin()); }
    for(int i=0; i<3 && !all_att.empty(); ++i) { squad.atts.push_back(all_att[0]); all_att.erase(all_att.begin()); }

    squad.subs.insert(squad.subs.end(), all_gk.begin(), all_gk.end());
    squad.subs.insert(squad.subs.end(), all_def.begin(), all_def.end());
    squad.subs.insert(squad.subs.end(), all_mid.begin(), all_mid.end());
    squad.subs.insert(squad.subs.end(), all_att.begin(), all_att.end());
    return squad;
}

std::string club::get_random_buteur() {
    if(effectif.empty()) return "Inconnu";
    for(int i=0; i<5; i++) {
        int idx = random_int(0, effectif.size() - 1);
        std::string p = effectif[idx].getposte();
        if(p == "ST" || p == "CF" || p == "LW" || p == "RW" || p == "CAM") return effectif[idx].getnom();
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