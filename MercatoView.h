#ifndef MERCATOVIEW_H
#define MERCATOVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <map>
#include <string>
#include "Championnat.h"
#include "Profil.h"

class MercatoView : public QWidget {
    Q_OBJECT
public:
    explicit MercatoView(QWidget *parent = nullptr);
    
    // Appelé quand une partie démarre ou charge pour lier les données
    void setGameContext(std::map<std::string, Championnat>* champs, club* myClub, profil* myProfil);
    
    // Met à jour l'affichage (table + statut ouvert/fermé)
    void refresh(bool isMercatoOpen);

private slots:
    void onBuyClicked();
    void updateTable();

signals:
    void playerBought(); // Signal émis pour dire à MainWindow de rafraîchir l'argent et l'effectif

private:
    // UI Elements
    QTableWidget *tableMarket;
    QLineEdit *searchMarket;
    QComboBox *comboMarketChamp; 
    QComboBox *comboMarketClub;  
    QComboBox *comboMarketPoste; 
    QLineEdit *searchMinPrice; 
    QLineEdit *searchMaxPrice; 
    QPushButton *btnBuyPlayer; 
    QLabel *lblMercatoStatus;

    // Data Pointers
    std::map<std::string, Championnat>* map_champ = nullptr;
    club* monClub = nullptr;
    profil* profilJoueur = nullptr;
    bool isMarketOpen = false;

    QString formatMoney(double value);
    void setupUI();
};

#endif // MERCATOVIEW_H