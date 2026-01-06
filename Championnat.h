#ifndef CHAMPIONNAT_H
#define CHAMPIONNAT_H

#include <string>
#include <vector>
#include "Club.h"

class Championnat {
public:
    Championnat() = default;
    Championnat(std::string le_nom);

    std::vector<club>* getliste() const; // Le "mutable" est géré en C++ via le pointeur/const
    int getnbclub() const;
    std::string getnom() const;
    
    void update_prestige();
    float get_prestige_score() const;
    int get_tier() const;
    std::string get_tier_name() const;

private:
    mutable std::vector<club> liste_club;
    std::string nom;
    float prestige_score;
};

#endif // CHAMPIONNAT_H