#ifndef MATCH_H
#define MATCH_H

#include "Club.h"
#include "GameUtils.h"

class match {
public:
    match(club* d, club* e);
    int simuler_buts(float p_att, float p_def);
    MatchResult jouer_match();

private:
    club* dom; 
    club* ext;
};

#endif // MATCH_H