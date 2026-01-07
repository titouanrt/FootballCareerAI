#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QTextEdit>
#include <QProgressBar>
#include <QComboBox>
#include <QStackedWidget>
#include <QTimer>
#include <QTabWidget>
#include <QTableWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileDialog>
#include <QResizeEvent> 
#include <QShowEvent> 
#include <QLineEdit> 
#include <QGroupBox> 
#include <QtNetwork/QNetworkAccessManager>
#include <QDate> // <--- NOUVEAU

// Inclure vos modèles
#include "GameUtils.h"
#include "Player.h"
#include "Club.h"
#include "Championnat.h"
#include "Match.h"
#include "Saison.h"
#include "Profil.h"

struct LiveEvent {
    int minute;
    QString text;
    bool isMyTeam; 
    bool isGoal;       
    bool isHomeGoal;   
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void onSimulateMatch();
    void onSimulateToDate();
    void onViewSquad();
    void onStartGame(); 
    void updateMatchAnim(); 
    void onChampionnatChanged(int index); 
    void onVoirClassement();
    void onTabChanged(int index); 
    void onSaveGame(); 
    void onLoadGame(); 
    void updateMarketTable();
    void onBuyPlayer(); 

private:
    // UI Elements
    QWidget *centralWidget;
    QStackedWidget *stack; 
    QWidget *pageSelection;
    QComboBox *comboChampionnats;  
    QComboBox *comboClubs;          
    QWidget *pageGame;
    QLabel *lblInfoClub;
    QLabel *lblArgent; 
    
    // UI MANAGER
    QLabel *lblCoachName;
    QLabel *lblCoachRating;
    QProgressBar *barCoachXP;

    QLabel *lblDate; // Affiche la date réelle
    QPushButton *btnSauvegarder;
    QPushButton *btnSimuler;
    QTabWidget *tabRight;
    QTableWidget *tableClassement;
    
    // MERCATO
    QTableWidget *tableMarket;
    QLineEdit *searchMarket; 
    QComboBox *comboMarketChamp; 
    QComboBox *comboMarketClub;  
    QComboBox *comboMarketPoste; 
    QLineEdit *searchMinPrice; 
    QLineEdit *searchMaxPrice; 
    QPushButton *btnBuyPlayer; 
    QLabel *lblMercatoStatus; // <--- NOUVEAU : Statut du mercato (Ouvert/Fermé)
    
    // TERRAIN
    QWidget *widgetPitch; 
    QList<QWidget*> pitchPlayers; 
    QListWidget *listReserves;   

    // SCOREBOARD
    QWidget *widgetScoreboard; 
    QLabel *lblTeamHome;
    QLabel *lblTeamAway;
    QLabel *lblScoreBig;       
    QLabel *lblMatchTime;      
    QProgressBar *barMatch;
    QTextEdit *txtLog;         

    // DATA
    std::map<std::string, Championnat> map_champ;
    Championnat* currentChamp = nullptr;
    club* monClub = nullptr;
    saison* maSaison = nullptr;
    profil* profilJoueur = nullptr;
    
    // NETWORK
    QNetworkAccessManager *netManager;

    // GAME STATE
    int jourActuel = 0;
    QDate currentDate; // <--- NOUVEAU : La date actuelle du jeu
    
    QTimer *timerMatch;
    int matchProgress = 0; 
    int liveScoreDom = 0;
    int liveScoreExt = 0;
    int previousTabIndex = 0;

    std::vector<LiveEvent> liveFeed; 
    std::vector<QString> finalResults; 
    
    // Helpers internes UI
    void setupUI();        
    void refreshDashboard(); 
    void updateClassement(); 
    void refreshEffectif();  
    QString formatMoney(double value);
    
    // Helper logique
    bool isMercatoOpen(); // <--- NOUVEAU : Vérifie les dates
    void simulerSemaineRapide(); 

    int numSaison = 1;         // Compteur de saison
    const int MAX_SAISONS = 5; // Limite de la carrière

    void passerSaisonSuivante(); // Fonction principale de transition
};

#endif // MAINWINDOW_H