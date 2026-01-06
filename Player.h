#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class player {
public:
    player();
    player(int i, std::string n, int l, int a, std::string p, 
           int pac, int sho, int pas, int dri, int def, int phy, std::string u);
    
    int getid() const;
    int getlevel() const;
    int getage() const;
    std::string getposte() const;
    std::string getnom() const;
    std::string geturl() const;

    // Stats
    int get_pac() const;
    int get_sho() const;
    int get_pas() const;
    int get_dri() const;
    int get_def() const;
    int get_phy() const;
    
    std::string toString() const;
    bool operator<(const player& other) const;
    double calculate_value(float team_avg_score) const;

private:
    int id;
    std::string nom;
    int level; 
    int age;
    std::string poste;
    int pac, sho, pas, dri, def, phy;
    std::string url_img;
};

#endif