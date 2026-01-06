#include "Match.h"
#include <sstream>
#include <algorithm>

match::match(club* d, club* e) : dom{d}, ext{e} {}

int match::simuler_buts(float p_att, float p_def) {
    float ratio = p_att / (p_def + 1.0f); 
    int chance = random_int(0, 100);
    if(chance < 15 * ratio) return 0;
    if(chance < 45 * ratio) return 1;
    if(chance < 75 * ratio) return 2;
    if(chance < 90 * ratio) return 3;
    if(chance < 97 * ratio) return 4;
    return 5; 
}

MatchResult match::jouer_match() {   
    float s_dom = dom->getscore(); if(s_dom == 0) s_dom = 50; 
    float s_ext = ext->getscore(); if(s_ext == 0) s_ext = 50;

    int but_dom = simuler_buts(s_dom * 1.15f, s_ext);
    int but_ext = simuler_buts(s_ext, s_dom);

    MatchResult res;
    res.dom = dom; res.ext = ext;
    res.score_dom = but_dom; res.score_ext = but_ext;

    for(int i=0; i<but_dom; ++i) res.goals.push_back({random_int(1, 89), dom->get_random_buteur(), dom->getnom()});
    for(int i=0; i<but_ext; ++i) res.goals.push_back({random_int(1, 89), ext->get_random_buteur(), ext->getnom()});
    std::sort(res.goals.begin(), res.goals.end());

    if (but_dom > but_ext) { dom->update_match_stats(but_dom, but_ext, 3); ext->update_match_stats(but_ext, but_dom, 0); } 
    else if (but_dom < but_ext) { dom->update_match_stats(but_dom, but_ext, 0); ext->update_match_stats(but_ext, but_dom, 3); } 
    else { dom->update_match_stats(but_dom, but_ext, 1); ext->update_match_stats(but_ext, but_dom, 1); }

    std::stringstream ss;
    ss << dom->getnom() << " " << but_dom << "-" << but_ext << " " << ext->getnom();
    res.summary = ss.str();
    return res;
}