#include "mainwindow.h"
#include "DataManager.h" 
#include "PlayerCard.h" 

#include <QMessageBox>
#include <QDebug>
#include <QCoreApplication>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QScrollBar> 
#include <QDoubleValidator> 
#include <QLocale>
#include <QCalendarWidget>
#include <QDialogButtonBox>
#include <QDialog>
#include<QProgressDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Initialisation du gestionnaire réseau
    netManager = new QNetworkAccessManager(this);
    
    // --- CONFIGURATION INITIALE ---
    // Date de début : Vendredi 8 Août 2025
    currentDate = QDate(2025, 8, 8); 

    setupUI();
    
    // Chargement des données via le module dédié
    DataManager::chargerDonnees(map_champ);
    
    // Remplissage de la liste des championnats
    for(const auto& pair : map_champ) {
        comboChampionnats->addItem(QString::fromStdString(pair.first));
        comboMarketChamp->addItem(QString::fromStdString(pair.first));
    }
    comboMarketChamp->model()->sort(0);
    
    if(comboChampionnats->count() > 0) {
        onChampionnatChanged(0);
    }
}

MainWindow::~MainWindow() {}

// --- LOGIQUE MERCATO ---
bool MainWindow::isMercatoOpen() {
    int m = currentDate.month();
    int d = currentDate.day();
    
    // Mercato d'Été : Juillet (7), Août (8), et le 1er Septembre
    if (m == 7 || m == 8) return true;
    if (m == 9 && d <= 1) return true;
    
    // Mercato d'Hiver : Tout le mois de Janvier (1)
    if (m == 1) return true;
    
    return false;
}

// --- GESTION DE L'EFFECTIF (Affichage Cartes) ---
void MainWindow::refreshEffectif()
{
    listReserves->clear();
    for(auto w : pitchPlayers) delete w;
    pitchPlayers.clear();

    if(!monClub) return;

    SquadFormation squad = monClub->get_best_11();

    int W = widgetPitch->width();
    int H = widgetPitch->height();
    if (W < 400) W = 650; 
    if (H < 300) H = 500;

    // Fonction lambda pour placer une carte joueur sur le terrain
    auto placePlayer = [&](const player& p, float xPct, float yPct) {
        PlayerCard* token = new PlayerCard(p, netManager, widgetPitch);
        
        int x = (W * xPct) - (token->width() / 2);
        int y = (H * yPct) - (token->height() / 2);
        token->move(x, y);
        token->show();
        pitchPlayers.append(token);
    };

    // Positionnement Tactique (4-3-3 Standard)
    if(squad.gk.getid() != 0) placePlayer(squad.gk, 0.5f, 0.88f);

    float defY = 0.70f;
    if(squad.defs.size() > 0) placePlayer(squad.defs[0], 0.15f, defY);
    if(squad.defs.size() > 1) placePlayer(squad.defs[1], 0.38f, defY);
    if(squad.defs.size() > 2) placePlayer(squad.defs[2], 0.62f, defY);
    if(squad.defs.size() > 3) placePlayer(squad.defs[3], 0.85f, defY);

    float midY = 0.45f;
    if(squad.mids.size() > 0) placePlayer(squad.mids[0], 0.30f, midY);
    if(squad.mids.size() > 1) placePlayer(squad.mids[1], 0.70f, midY);
    if(squad.mids.size() > 2) placePlayer(squad.mids[2], 0.50f, 0.35f); 

    float attY = 0.15f;
    if(squad.atts.size() > 0) placePlayer(squad.atts[0], 0.20f, attY);
    if(squad.atts.size() > 1) placePlayer(squad.atts[1], 0.50f, 0.08f); 
    if(squad.atts.size() > 2) placePlayer(squad.atts[2], 0.80f, attY);

    // Remplissage de la liste des réservistes
    for(const auto& p : squad.subs) {
        listReserves->addItem(QString::fromStdString(p.toString()));
    }
}

// --- CONSTRUCTION DE L'INTERFACE ---
void MainWindow::setupUI()
{
    resize(1400, 900); 
    setWindowTitle("Ultimate Football Manager Simulator 2026");

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0,0,0,0); 

    stack = new QStackedWidget();
    mainLayout->addWidget(stack);

    // --- PAGE 1 : SÉLECTION / LOGIN ---
    pageSelection = new QWidget();
    QVBoxLayout *layoutSelFull = new QVBoxLayout(pageSelection);
    layoutSelFull->setAlignment(Qt::AlignCenter);

    QFrame *frameLogin = new QFrame();
    frameLogin->setFixedWidth(450);
    frameLogin->setStyleSheet("background-color: #1e1e1e; border-radius: 12px; border: 1px solid #333;");
    QVBoxLayout *layoutCard = new QVBoxLayout(frameLogin);
    layoutCard->setContentsMargins(40, 40, 40, 40);
    layoutCard->setSpacing(15);

    QLabel *lblIcon = new QLabel("⚽");
    lblIcon->setAlignment(Qt::AlignCenter);
    lblIcon->setStyleSheet("font-size: 60px; margin-bottom: 10px;");

    QLabel *lblTitle = new QLabel("NOUVELLE CARRIÈRE");
    lblTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #2ecc71; letter-spacing: 1px;");
    lblTitle->setAlignment(Qt::AlignCenter);
    
    QLabel *lblChamp = new QLabel("🏆 Sélectionner un Championnat");
    lblChamp->setStyleSheet("color: #aaa; margin-top: 10px;");
    
    comboChampionnats = new QComboBox();
    comboChampionnats->setFixedHeight(45);
    connect(comboChampionnats, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onChampionnatChanged);
    
    QLabel *lblClub = new QLabel("👕 Sélectionner un Club");
    lblClub->setStyleSheet("color: #aaa; margin-top: 10px;");
    
    comboClubs = new QComboBox();
    comboClubs->setFixedHeight(45);
    
    QPushButton *btnStart = new QPushButton("COMMENCER LA SAISON ➤");
    btnStart->setCursor(Qt::PointingHandCursor);
    btnStart->setFixedHeight(55);
    btnStart->setStyleSheet("font-size: 16px; font-weight: bold;");
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::onStartGame);

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #333; margin: 10px 0;");

    QPushButton *btnLoad = new QPushButton("📂 CHARGER UNE PARTIE");
    btnLoad->setStyleSheet("background-color: #34495e; color: white; padding: 12px; border-radius: 6px;");
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadGame);

    layoutCard->addWidget(lblIcon);
    layoutCard->addWidget(lblTitle);
    layoutCard->addWidget(lblChamp);
    layoutCard->addWidget(comboChampionnats);
    layoutCard->addWidget(lblClub);
    layoutCard->addWidget(comboClubs);
    layoutCard->addSpacing(20);
    layoutCard->addWidget(btnStart);
    layoutCard->addWidget(line);
    layoutCard->addWidget(btnLoad);

    layoutSelFull->addWidget(frameLogin);
    stack->addWidget(pageSelection);

    // --- PAGE 2 : JEU PRINCIPAL ---
    pageGame = new QWidget();
    QHBoxLayout *layoutGame = new QHBoxLayout(pageGame);
    layoutGame->setContentsMargins(0,0,0,0);
    layoutGame->setSpacing(0);

    // PANNEAU GAUCHE (INFO CLUB & MANAGER)
    QWidget *leftPanel = new QWidget();
    leftPanel->setFixedWidth(280);
    leftPanel->setStyleSheet("background-color: #181818; border-right: 1px solid #2e2e2e;");
    QVBoxLayout *layoutLeft = new QVBoxLayout(leftPanel);
    layoutLeft->setContentsMargins(20, 30, 20, 30);

    lblInfoClub = new QLabel("Club Inconnu");
    lblInfoClub->setStyleSheet("color: #2ecc71; font-size: 22px; font-weight: 800; margin-bottom: 5px;");
    lblInfoClub->setWordWrap(true);
    lblInfoClub->setAlignment(Qt::AlignCenter);

    lblArgent = new QLabel("0 €");
    lblArgent->setStyleSheet("color: #f1c40f; font-size: 18px; font-weight: bold; margin-bottom: 15px; border: 1px solid #444; border-radius: 5px; padding: 5px;");
    lblArgent->setAlignment(Qt::AlignCenter);

    // Section Manager
    QFrame *frameManager = new QFrame();
    frameManager->setStyleSheet("background-color: #222; border-radius: 6px; margin-bottom: 15px; border: 1px solid #333;");
    QVBoxLayout *layoutManager = new QVBoxLayout(frameManager);
    layoutManager->setContentsMargins(15, 15, 15, 15);
    layoutManager->setSpacing(8);

    lblCoachName = new QLabel("MANAGER");
    lblCoachName->setStyleSheet("color: #aaa; font-size: 12px; font-weight: bold; border:none; text-transform: uppercase;");
    lblCoachName->setAlignment(Qt::AlignCenter);

    lblCoachRating = new QLabel("Note: 65");
    lblCoachRating->setStyleSheet("color: #2ecc71; font-size: 24px; font-weight: 900; border:none;");
    lblCoachRating->setAlignment(Qt::AlignCenter);

    barCoachXP = new QProgressBar();
    barCoachXP->setRange(0, 100);
    barCoachXP->setValue(0);
    barCoachXP->setFixedHeight(16);
    barCoachXP->setTextVisible(true);
    barCoachXP->setFormat("%v / 100 XP");
    barCoachXP->setAlignment(Qt::AlignCenter);
    barCoachXP->setStyleSheet(
        "QProgressBar { background-color: #444; border: 1px solid #666; border-radius: 8px; color: white; font-weight: bold; font-size: 10px; }"
        "QProgressBar::chunk { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #3498db, stop:1 #2980b9); border-radius: 7px; }"
    );

    layoutManager->addWidget(lblCoachName);
    layoutManager->addWidget(lblCoachRating);
    layoutManager->addWidget(barCoachXP);

    // Section Date
    lblDate = new QLabel("8 Août 2025");
    lblDate->setStyleSheet("color: #ecf0f1; font-size: 16px; font-weight: bold; margin-bottom: 20px; border-bottom: 2px solid #2ecc71; padding-bottom: 5px;");
    lblDate->setAlignment(Qt::AlignCenter);
    
    btnSauvegarder = new QPushButton("💾 Sauvegarder");
    btnSauvegarder->setStyleSheet("background-color: #2980b9; text-align: left; padding-left: 20px; border-radius: 6px; padding: 12px;");
    connect(btnSauvegarder, &QPushButton::clicked, this, &MainWindow::onSaveGame);

    btnSimuler = new QPushButton("⚡ SIMULER MATCH (+1 sem)");
    btnSimuler->setStyleSheet("background-color: #e67e22; font-size: 14px; padding: 20px; margin-top: 20px; border-radius: 8px; font-weight: 900; letter-spacing: 1px;"); 
    connect(btnSimuler, &QPushButton::clicked, this, &MainWindow::onSimulateMatch);

    QPushButton *btnSimulateDate = new QPushButton("📅 SIMULER JUSQU'À...");
    btnSimulateDate->setStyleSheet("background-color: #8e44ad; font-size: 12px; padding: 10px; margin-top: 5px; border-radius: 6px; font-weight: bold;");
    connect(btnSimulateDate, &QPushButton::clicked, this, &MainWindow::onSimulateToDate);

    layoutLeft->addWidget(lblInfoClub);
    layoutLeft->addWidget(lblArgent);
    layoutLeft->addWidget(frameManager);
    layoutLeft->addWidget(lblDate);
    
    QFrame *lineLeft = new QFrame();
    lineLeft->setFrameShape(QFrame::HLine);
    lineLeft->setStyleSheet("color: #333;");
    layoutLeft->addWidget(lineLeft);
    layoutLeft->addSpacing(20);

    layoutLeft->addWidget(btnSauvegarder);
    layoutLeft->addStretch();
    layoutLeft->addWidget(btnSimuler);
    layoutLeft->addWidget(btnSimulateDate); 

    // PANNEAU DROIT (ONGLETS)
    QWidget *rightPanel = new QWidget();
    QVBoxLayout *layoutRight = new QVBoxLayout(rightPanel);
    layoutRight->setContentsMargins(20, 20, 20, 20);

    tabRight = new QTabWidget();
    tabRight->setStyleSheet("QTabWidget::pane { border: 0; }");

    // ONGLET 1 : MATCH LIVE
    QWidget *tabMatch = new QWidget();
    QVBoxLayout *tmLayout = new QVBoxLayout(tabMatch);
    tmLayout->setSpacing(0);
    tmLayout->setContentsMargins(0,0,0,0);

    widgetScoreboard = new QWidget();
    widgetScoreboard->setFixedHeight(140);
    widgetScoreboard->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #252526, stop:1 #1e1e1e);"
        "border-bottom: 1px solid #333; border-radius: 8px;"
    );
    QHBoxLayout *scoreLayout = new QHBoxLayout(widgetScoreboard);
    
    lblTeamHome = new QLabel("DOMICILE");
    lblTeamHome->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    lblTeamHome->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    
    QVBoxLayout *centerScoreLayout = new QVBoxLayout();
    lblScoreBig = new QLabel("0 - 0");
    lblScoreBig->setAlignment(Qt::AlignCenter);
    lblScoreBig->setStyleSheet("font-size: 48px; font-weight: 900; color: #ecf0f1;");
    
    lblMatchTime = new QLabel("0'");
    lblMatchTime->setAlignment(Qt::AlignCenter);
    lblMatchTime->setStyleSheet("background-color: #2ecc71; color: #1e1e1e; font-weight: bold; padding: 2px 8px; border-radius: 4px; font-size: 16px;");
    lblMatchTime->setFixedWidth(60);
    
    centerScoreLayout->addWidget(lblScoreBig);
    centerScoreLayout->addWidget(lblMatchTime, 0, Qt::AlignCenter);

    lblTeamAway = new QLabel("EXTÉRIEUR");
    lblTeamAway->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    lblTeamAway->setStyleSheet("font-size: 24px; font-weight: bold; color: #bdc3c7;");

    scoreLayout->addWidget(lblTeamHome, 1);
    scoreLayout->addLayout(centerScoreLayout, 0);
    scoreLayout->addWidget(lblTeamAway, 1);

    barMatch = new QProgressBar();
    barMatch->setRange(0, 90);
    barMatch->setValue(0);
    barMatch->setFixedHeight(6);
    barMatch->setTextVisible(false);
    barMatch->setStyleSheet("QProgressBar { background: #333; border: none; } QProgressBar::chunk { background: #2ecc71; }");

    txtLog = new QTextEdit();
    txtLog->setReadOnly(true);
    txtLog->setFrameShape(QFrame::NoFrame);
    txtLog->setStyleSheet("background-color: #121212; padding: 10px; font-family: 'Segoe UI'; font-size: 14px;");

    tmLayout->addWidget(widgetScoreboard);
    tmLayout->addWidget(barMatch);
    tmLayout->addWidget(txtLog);

    // ONGLET 2 : EFFECTIF / TACTIQUE
    QWidget *tabEffectifWidget = new QWidget();
    QHBoxLayout *effLayout = new QHBoxLayout(tabEffectifWidget);
    
    QWidget *leftSquad = new QWidget();
    QVBoxLayout *lsLayout = new QVBoxLayout(leftSquad);
    lsLayout->setContentsMargins(0,0,0,0);
    
    QLabel *lblRes = new QLabel("Remplaçants & Réserves");
    lblRes->setStyleSheet("font-weight:bold; color:#aaa; margin-bottom:5px;");
    listReserves = new QListWidget();
    listReserves->setStyleSheet("background: #252526; border-radius: 4px;");
    
    lsLayout->addWidget(lblRes);
    lsLayout->addWidget(listReserves);
    leftSquad->setFixedWidth(250);

    widgetPitch = new QWidget();
    widgetPitch->setStyleSheet(
        "background-color: #27ae60;" 
        "border: 2px solid white; border-radius: 8px;"
    );

    effLayout->addWidget(leftSquad);
    effLayout->addWidget(widgetPitch, 1);

    // ONGLET 3 : CLASSEMENT
    tableClassement = new QTableWidget();
    tableClassement->setColumnCount(4);
    tableClassement->setHorizontalHeaderLabels(QStringList() << "#" << "Club" << "Pts" << "Diff");
    tableClassement->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tableClassement->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    tableClassement->setColumnWidth(0, 40);
    tableClassement->setColumnWidth(2, 60);
    tableClassement->setColumnWidth(3, 60);
    tableClassement->verticalHeader()->setVisible(false);
    tableClassement->setAlternatingRowColors(true);
    tableClassement->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableClassement->setFocusPolicy(Qt::NoFocus);

    // ONGLET 4 : MERCATO
    QWidget *tabMarket = new QWidget();
    QVBoxLayout *marketLayout = new QVBoxLayout(tabMarket);
    
    // Ajout du statut Mercato
    lblMercatoStatus = new QLabel("STATUT MERCATO");
    lblMercatoStatus->setAlignment(Qt::AlignCenter);
    lblMercatoStatus->setStyleSheet("background-color: #7f8c8d; color: white; font-weight: bold; padding: 10px; border-radius: 5px; font-size: 16px; margin-bottom: 10px;");

    QGroupBox *boxFilter = new QGroupBox("🔍 Recherche de Joueurs");
    boxFilter->setStyleSheet("QGroupBox { border: 1px solid #444; border-radius: 5px; margin-top: 10px; font-size:14px; font-weight:bold; color:#ccc; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }");
    
    QGridLayout *filterGrid = new QGridLayout(boxFilter);
    filterGrid->setSpacing(15);

    searchMarket = new QLineEdit();
    searchMarket->setPlaceholderText("Tapez le nom d'un joueur...");
    searchMarket->setStyleSheet("padding: 10px; font-size: 14px; background: #252526; border: 1px solid #444; color: white;");
    
    QLabel *l1 = new QLabel("Championnat :"); l1->setStyleSheet("color:#aaa;");
    comboMarketChamp = new QComboBox();
    comboMarketChamp->addItem("Tous les championnats");
    comboMarketChamp->setFixedHeight(35);

    QLabel *l2 = new QLabel("Club :"); l2->setStyleSheet("color:#aaa;");
    comboMarketClub = new QComboBox();
    comboMarketClub->addItem("Tous les clubs");
    comboMarketClub->setFixedHeight(35);

    QLabel *l3 = new QLabel("Poste :"); l3->setStyleSheet("color:#aaa;");
    comboMarketPoste = new QComboBox();
    comboMarketPoste->addItem("Tous les postes");
    comboMarketPoste->addItems(QStringList() << "Gardiens" << "Défenseurs" << "Milieux" << "Attaquants");
    comboMarketPoste->setFixedHeight(35);

    QLabel *l4 = new QLabel("Budget (Min/Max) :"); l4->setStyleSheet("color:#aaa;");
    QWidget *priceWidget = new QWidget();
    QHBoxLayout *priceLayout = new QHBoxLayout(priceWidget);
    priceLayout->setContentsMargins(0,0,0,0);
    
    searchMinPrice = new QLineEdit();
    searchMinPrice->setPlaceholderText("Min €");
    searchMinPrice->setValidator(new QDoubleValidator(0, 1000000000, 0, this)); 
    searchMinPrice->setStyleSheet("padding: 5px; background: #252526; border: 1px solid #444; color: white;");
    
    searchMaxPrice = new QLineEdit();
    searchMaxPrice->setPlaceholderText("Max €");
    searchMaxPrice->setValidator(new QDoubleValidator(0, 1000000000, 0, this));
    searchMaxPrice->setStyleSheet("padding: 5px; background: #252526; border: 1px solid #444; color: white;");
    
    priceLayout->addWidget(searchMinPrice);
    priceLayout->addWidget(searchMaxPrice);

    filterGrid->addWidget(searchMarket, 0, 0, 1, 4); 
    filterGrid->addWidget(l1, 1, 0); filterGrid->addWidget(comboMarketChamp, 1, 1);
    filterGrid->addWidget(l2, 1, 2); filterGrid->addWidget(comboMarketClub, 1, 3);
    filterGrid->addWidget(l3, 2, 0); filterGrid->addWidget(comboMarketPoste, 2, 1);
    filterGrid->addWidget(l4, 2, 2); filterGrid->addWidget(priceWidget, 2, 3); 

    // Connexions Filtres Mercato
    connect(comboMarketChamp, &QComboBox::currentTextChanged, this, [this](const QString &text){
        comboMarketClub->blockSignals(true);
        comboMarketClub->clear();
        comboMarketClub->addItem("Tous les clubs");
        if (text != "Tous les championnats" && map_champ.find(text.toStdString()) != map_champ.end()) {
             std::vector<club>* clubs = map_champ[text.toStdString()].getliste();
             std::vector<std::string> names;
             for(auto& c : *clubs) names.push_back(c.getnom());
             std::sort(names.begin(), names.end());
             for(const auto& name : names) comboMarketClub->addItem(QString::fromStdString(name));
        }
        comboMarketClub->blockSignals(false);
        updateMarketTable();
    });
    connect(comboMarketClub, &QComboBox::currentTextChanged, this, [this](){ updateMarketTable(); });
    connect(comboMarketPoste, &QComboBox::currentTextChanged, this, [this](){ updateMarketTable(); });
    connect(searchMarket, &QLineEdit::textChanged, this, [this](){ updateMarketTable(); });
    connect(searchMinPrice, &QLineEdit::textChanged, this, [this](){ updateMarketTable(); });
    connect(searchMaxPrice, &QLineEdit::textChanged, this, [this](){ updateMarketTable(); });

    // Table Mercato
    tableMarket = new QTableWidget();
    tableMarket->setColumnCount(12); 
    tableMarket->setHorizontalHeaderLabels(QStringList() 
        << "Nom" << "Age" << "Poste" << "Club" << "OVR" 
        << "PAC" << "SHO" << "PAS" << "DRI" << "DEF" << "PHY" << "Valeur");
    
    tableMarket->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tableMarket->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    for(int i=4; i<=10; i++) tableMarket->setColumnWidth(i, 45);
    tableMarket->setAlternatingRowColors(true);
    tableMarket->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableMarket->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableMarket->setSortingEnabled(true); 

    btnBuyPlayer = new QPushButton("ACHETER LE JOUEUR SÉLECTIONNÉ 💰");
    btnBuyPlayer->setStyleSheet("background-color: #27ae60; color: white; padding: 15px; font-size: 16px; margin-top: 10px; font-weight: bold; border-radius: 5px;");
    connect(btnBuyPlayer, &QPushButton::clicked, this, &MainWindow::onBuyPlayer);

    marketLayout->addWidget(lblMercatoStatus);
    marketLayout->addWidget(boxFilter);
    marketLayout->addWidget(tableMarket);
    marketLayout->addWidget(btnBuyPlayer); 

    // AJOUT DES ONGLETS
    tabRight->addTab(tabMatch, "🏟️ MATCH LIVE");
    tabRight->addTab(tabEffectifWidget, "📋 TACTIQUE");
    tabRight->addTab(tableClassement, "📊 CLASSEMENT");
    tabRight->addTab(tabMarket, "💰 MERCATO");

    layoutRight->addWidget(tabRight);
    
    connect(tabRight, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    layoutGame->addWidget(leftPanel);
    layoutGame->addWidget(rightPanel);

    stack->addWidget(pageGame);

    timerMatch = new QTimer(this);
    connect(timerMatch, &QTimer::timeout, this, &MainWindow::updateMatchAnim);
}

QString MainWindow::formatMoney(double value) {
    if (value >= 1000000) {
        return QString::number(value / 1000000.0, 'f', 1) + " M€";
    } else if (value >= 1000) {
        return QString::number(value / 1000.0, 'f', 0) + " K€";
    }
    return QString::number(value) + " €";
}

void MainWindow::updateMarketTable()
{
    QString search = searchMarket->text();
    QString filterChamp = comboMarketChamp->currentText();
    QString filterClub = comboMarketClub->currentText();
    QString filterPoste = comboMarketPoste->currentText();
    
    double minPrice = searchMinPrice->text().toDouble();
    double maxPrice = searchMaxPrice->text().toDouble();
    if(maxPrice == 0) maxPrice = 10000000000.0; 

    tableMarket->setSortingEnabled(false);
    tableMarket->setRowCount(0);

    int row = 0;
    
    for (auto const& [nomChamp, champObj] : map_champ) {
        if (filterChamp != "Tous les championnats" && QString::fromStdString(nomChamp) != filterChamp) continue;

        std::vector<club>* clubs = champObj.getliste();
        if(!clubs) continue;

        for (const auto& c : *clubs) {
            std::string clubName = c.getnom();
            if (filterClub != "Tous les clubs" && QString::fromStdString(clubName) != filterClub) continue;

            float clubScore = c.getscore();

            for (const auto& p : c.getEffectif()) {
                QString qNom = QString::fromStdString(p.getnom());
                std::string pPos = p.getposte();

                if (!search.isEmpty() && !qNom.contains(search, Qt::CaseInsensitive)) continue;

                bool matchPoste = false;
                if (filterPoste == "Tous les postes") matchPoste = true;
                else if (filterPoste == "Gardiens" && pPos == "GK") matchPoste = true;
                else if (filterPoste == "Défenseurs" && (pPos == "CB" || pPos == "LB" || pPos == "RB" || pPos == "LWB" || pPos == "RWB" || pPos == "DC")) matchPoste = true;
                else if (filterPoste == "Milieux" && (pPos == "CDM" || pPos == "CM" || pPos == "CAM" || pPos == "LM" || pPos == "RM")) matchPoste = true;
                else if (filterPoste == "Attaquants" && (pPos == "ST" || pPos == "CF" || pPos == "LW" || pPos == "RW")) matchPoste = true;

                if (!matchPoste) continue;

                double val = p.calculate_value(clubScore);
                if (val < minPrice || val > maxPrice) continue;

                tableMarket->insertRow(row);

                QTableWidgetItem *itNom = new QTableWidgetItem(qNom);
                itNom->setData(Qt::UserRole, p.getid()); 
                itNom->setFont(QFont("Segoe UI", 10, QFont::Bold));
                tableMarket->setItem(row, 0, itNom);
                
                QTableWidgetItem *itAge = new QTableWidgetItem();
                itAge->setData(Qt::DisplayRole, p.getage());
                itAge->setTextAlignment(Qt::AlignCenter);
                tableMarket->setItem(row, 1, itAge);

                QTableWidgetItem *itPoste = new QTableWidgetItem(QString::fromStdString(pPos));
                itPoste->setTextAlignment(Qt::AlignCenter);
                tableMarket->setItem(row, 2, itPoste);

                tableMarket->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(clubName)));

                QTableWidgetItem *itNote = new QTableWidgetItem();
                itNote->setData(Qt::DisplayRole, p.getlevel());
                itNote->setTextAlignment(Qt::AlignCenter);
                itNote->setFont(QFont("Segoe UI", 11, QFont::Bold));
                if(p.getlevel() >= 88) itNote->setBackground(QBrush(QColor("#e6b0aa"))); 
                tableMarket->setItem(row, 4, itNote);

                auto addStat = [&](int col, int statVal) {
                    QTableWidgetItem *it = new QTableWidgetItem();
                    it->setData(Qt::DisplayRole, statVal);
                    it->setTextAlignment(Qt::AlignCenter);
                    if(statVal >= 85) it->setForeground(QBrush(QColor("#2ecc71"))); 
                    else if(statVal >= 70) it->setForeground(QBrush(QColor("#f1c40f")));
                    else it->setForeground(QBrush(QColor("#e74c3c")));
                    tableMarket->setItem(row, col, it);
                };

                addStat(5, p.get_pac());
                addStat(6, p.get_sho());
                addStat(7, p.get_pas());
                addStat(8, p.get_dri());
                addStat(9, p.get_def());
                addStat(10, p.get_phy());

                QTableWidgetItem *itVal = new QTableWidgetItem();
                itVal->setData(Qt::DisplayRole, val); 
                itVal->setData(Qt::UserRole, val);    
                itVal->setText(formatMoney(val));     
                itVal->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                itVal->setFont(QFont("Segoe UI", 10, QFont::Bold));
                tableMarket->setItem(row, 11, itVal);

                row++;
                if (filterChamp == "Tous les championnats" && filterClub == "Tous les clubs" && search.isEmpty() && filterPoste == "Tous les postes" && minPrice == 0 && maxPrice > 9999999999 && row > 1000) break;
            }
            if (filterChamp == "Tous les championnats" && filterClub == "Tous les clubs" && search.isEmpty() && filterPoste == "Tous les postes" && minPrice == 0 && maxPrice > 9999999999 && row > 1000) break;
        }
        if (filterChamp == "Tous les championnats" && filterClub == "Tous les clubs" && search.isEmpty() && filterPoste == "Tous les postes" && minPrice == 0 && maxPrice > 9999999999 && row > 1000) break;
    }

    tableMarket->setSortingEnabled(true);
}

void MainWindow::onBuyPlayer()
{
    // --- VÉRIFICATION MERCATO ---
    if(!isMercatoOpen()) {
        QMessageBox::warning(this, "Mercato Fermé", 
            "La période des transferts est actuellement fermée !\n\n"
            "Attendez l'été (Juillet-Août) ou l'hiver (Janvier).");
        return;
    }

    if (!tableMarket->currentItem()) {
        QMessageBox::warning(this, "Mercato", "Veuillez sélectionner un joueur dans la liste.");
        return;
    }

    int row = tableMarket->currentRow();
    
    QString nomJoueur = tableMarket->item(row, 0)->text();
    int idJoueur = tableMarket->item(row, 0)->data(Qt::UserRole).toInt();
    QString nomClubSource = tableMarket->item(row, 3)->text(); 
    double prix = tableMarket->item(row, 11)->data(Qt::UserRole).toDouble();

    if (nomClubSource == QString::fromStdString(monClub->getnom())) {
        QMessageBox::information(this, "Transfert", "Ce joueur est déjà dans votre club !");
        return;
    }

    if (!profilJoueur->spend_argent((long long)prix)) {
        QMessageBox::critical(this, "Fonds insuffisants", "Vous n'avez pas assez d'argent pour acheter " + nomJoueur + " !");
        return;
    }

    club* clubVendeur = nullptr;
    for (auto& [nom, champ] : map_champ) {
        std::vector<club>* liste = champ.getliste();
        for (auto& c : *liste) {
            if (c.getnom() == nomClubSource.toStdString()) {
                clubVendeur = &c;
                break;
            }
        }
        if (clubVendeur) break;
    }

    if (clubVendeur) {
        player joueurTransfere;
        bool found = false;
        for (const auto& p : clubVendeur->getEffectif()) {
            if (p.getid() == idJoueur) {
                joueurTransfere = p;
                found = true;
                break;
            }
        }

        if (found) {
            clubVendeur->remove_player(idJoueur);
            monClub->ajouter_joueur(joueurTransfere);
            
            QMessageBox::information(this, "Transfert Réussi", 
                "Bienvenue à " + nomJoueur + " !\n"
                "Montant du transfert : " + formatMoney(prix));
            
            refreshDashboard(); 
            updateMarketTable(); 
            refreshEffectif(); 
        }
    }
}

void MainWindow::updateClassement()
{
    if(!currentChamp) return;
    std::vector<club>* liste = currentChamp->getliste();
    if(!liste) return;

    struct Row { std::string name; int pts; int diff; int bp; };
    std::vector<Row> rows;
    for(const auto& c : *liste) {
        rows.push_back({c.getnom(), c.getnbpoint(), c.get_diff(), c.get_bp()});
    }

    std::sort(rows.begin(), rows.end(), [](const Row &a, const Row &b){
        if(a.pts != b.pts) return a.pts > b.pts;
        if(a.diff != b.diff) return a.diff > b.diff;
        if(a.bp != b.bp) return a.bp > b.bp;
        return a.name < b.name;
    });

    tableClassement->setRowCount((int)rows.size());
    std::string myClubName = (monClub) ? monClub->getnom() : "";

    for(int i=0; i<rows.size(); ++i) {
        QTableWidgetItem *itPos = new QTableWidgetItem(QString::number(i+1));
        QTableWidgetItem *itName = new QTableWidgetItem(QString::fromStdString(rows[i].name));
        QTableWidgetItem *itPts = new QTableWidgetItem(QString::number(rows[i].pts));
        
        QString strDiff = (rows[i].diff > 0 ? "+" : "") + QString::number(rows[i].diff);
        QTableWidgetItem *itDiff = new QTableWidgetItem(strDiff);

        itPos->setTextAlignment(Qt::AlignCenter);
        itPts->setTextAlignment(Qt::AlignCenter);
        itDiff->setTextAlignment(Qt::AlignCenter);
        
        itPts->setFont(QFont("Segoe UI", 10, QFont::Bold));

        if(rows[i].diff > 0) itDiff->setForeground(QBrush(QColor("#2ecc71")));
        else if(rows[i].diff < 0) itDiff->setForeground(QBrush(QColor("#e74c3c")));

        if (monClub && rows[i].name == myClubName) {
            QColor highlightColor(46, 204, 113, 80); 
            itPos->setBackground(highlightColor);
            itName->setBackground(highlightColor);
            itPts->setBackground(highlightColor);
            itDiff->setBackground(highlightColor);
            itName->setForeground(QBrush(QColor("#ffffff")));
            itName->setFont(QFont("Segoe UI", 10, QFont::Bold));
        }

        tableClassement->setItem(i, 0, itPos);
        tableClassement->setItem(i, 1, itName);
        tableClassement->setItem(i, 2, itPts);
        tableClassement->setItem(i, 3, itDiff);
    }
}

void MainWindow::onStartGame()
{
    QString clubName = comboClubs->currentText();
    
    if(!currentChamp) {
        QMessageBox::warning(this, "Erreur", "Aucun championnat sélectionné!");
        return;
    }
    
    std::vector<club>* liste = currentChamp->getliste();
    if(!liste) return;
    
    for(auto& c : *liste) {
        if(c.getnom() == clubName.toStdString()) {
            monClub = &c;
            break;
        }
    }

    if(monClub) {
        long long budget = 5000000 + (monClub->getscore() * 1000000);
        if(currentChamp->get_tier() == 1) budget *= 3; 
        else if(currentChamp->get_tier() == 2) budget *= 1.5;

        maSaison = new saison(*currentChamp);
        profilJoueur = new profil(budget, monClub, currentChamp);
        profilJoueur->changer_saison(*maSaison);

        lblInfoClub->setText(clubName); 
        refreshDashboard();
        
        stack->setCurrentWidget(pageGame);
        updateClassement();
        onViewSquad(); 
        updateMarketTable(); 
    }
}

// --- MISE A JOUR DASHBOARD AVEC DATE & MERCATO ---
void MainWindow::refreshDashboard()
{
    if(!monClub) return;
    
    // 1. Mise à jour de la Date
    QLocale locale(QLocale::French);
    QString dateStr = locale.toString(currentDate, "dd MMMM yyyy");
    // Majuscule au mois (cosmétique)
    if(dateStr.split(" ").size() > 1) {
        QStringList parts = dateStr.split(" ");
        if(parts.size() >= 2) parts[1] = parts[1].at(0).toUpper() + parts[1].mid(1);
        dateStr = parts.join(" ");
    }
lblDate->setText(dateStr + "\nSaison " + QString::number(numSaison) + "/" + QString::number(MAX_SAISONS) + " - J" + QString::number(jourActuel + 1));
    // 2. Mise à jour statut Mercato
    if(isMercatoOpen()) {
        lblMercatoStatus->setText("🟢 MERCATO OUVERT");
        lblMercatoStatus->setStyleSheet("background-color: #27ae60; color: white; font-weight: bold; padding: 8px; border-radius: 4px; font-size: 16px; margin-bottom: 10px;");
        btnBuyPlayer->setEnabled(true);
        btnBuyPlayer->setText("ACHETER LE JOUEUR 💰");
        btnBuyPlayer->setStyleSheet("background-color: #27ae60; color: white; padding: 15px; font-weight: bold; border-radius: 5px;");
    } else {
        lblMercatoStatus->setText("🔴 MERCATO FERMÉ");
        lblMercatoStatus->setStyleSheet("background-color: #c0392b; color: white; font-weight: bold; padding: 8px; border-radius: 4px; font-size: 16px; margin-bottom: 10px;");
        btnBuyPlayer->setEnabled(false);
        btnBuyPlayer->setText("MERCATO FERMÉ 🔒");
        btnBuyPlayer->setStyleSheet("background-color: #7f8c8d; color: #bdc3c7; padding: 15px; font-weight: bold; border-radius: 5px;");
    }

    // 3. Argent & Manager
    if(profilJoueur) {
        lblArgent->setText(formatMoney(profilJoueur->get_argent()));
        
        int lvl = profilJoueur->get_manager_level();
        int xp = profilJoueur->get_manager_xp();
        
        lblCoachRating->setText("Note : " + QString::number(lvl));
        barCoachXP->setValue(xp);
        
        QString color = "#2ecc71"; // Vert
        if(lvl < 60) color = "#e74c3c"; // Rouge
        else if(lvl < 75) color = "#f39c12"; // Orange
        else if(lvl >= 85) color = "#3498db"; // Bleu Elite
        
        lblCoachRating->setStyleSheet("color: " + color + "; font-size: 20px; font-weight: 900; border:none;");
    }
}

void MainWindow::onViewSquad()
{
    if(tabRight) tabRight->setCurrentIndex(1);
    QTimer::singleShot(100, this, [this](){
        refreshEffectif();
    });
}

void MainWindow::onSimulateMatch()
{
    if(jourActuel >= maSaison->getnbjour()) {
        passerSaisonSuivante();
        return;
    }

    if(tabRight) {
        previousTabIndex = tabRight->currentIndex();
        tabRight->setCurrentIndex(0); 
    }

    liveScoreDom = 0;
    liveScoreExt = 0;
    lblScoreBig->setText("0 - 0");
    lblScoreBig->setStyleSheet("font-size: 48px; font-weight: 900; color: #ecf0f1;");
    lblMatchTime->setText("0'");
    txtLog->clear();
    widgetScoreboard->setVisible(true); 
    
    liveFeed.clear();
    finalResults.clear();
    
    int nb_matchs = maSaison->getnbmatch();
    int start = jourActuel * nb_matchs;
    std::vector<match>& matches = maSaison->getlist();
    std::string monClubNom = (monClub ? monClub->getnom() : "");

    for(int i = start; i < start + nb_matchs && i < matches.size(); i++) {
        MatchResult res = matches[i].jouer_match(); 
        bool isMyGame = (res.dom->getnom() == monClubNom || res.ext->getnom() == monClubNom);

        if(isMyGame) {
            lblTeamHome->setText(QString::fromStdString(res.dom->getnom()));
            lblTeamAway->setText(QString::fromStdString(res.ext->getnom()));
            
            if(res.dom->getnom() == monClubNom) {
                lblTeamHome->setStyleSheet("font-size: 24px; font-weight: bold; color: #2ecc71;");
                lblTeamAway->setStyleSheet("font-size: 24px; font-weight: bold; color: #bdc3c7;");
            } else {
                lblTeamHome->setStyleSheet("font-size: 24px; font-weight: bold; color: #bdc3c7;");
                lblTeamAway->setStyleSheet("font-size: 24px; font-weight: bold; color: #2ecc71;");
            }

            for(const auto& g : res.goals) {
                LiveEvent ev;
                ev.minute = g.minute;
                ev.isMyTeam = true;
                ev.isGoal = true;
                ev.isHomeGoal = (g.teamName == res.dom->getnom());
                ev.text = QString("⚽ %1' <b>BUT !</b> %2 ouvre le score pour %3 !")
                              .arg(g.minute)
                              .arg(QString::fromStdString(g.scorer))
                              .arg(QString::fromStdString(g.teamName));
                liveFeed.push_back(ev);
            }

            int nbEvents = random_int(4, 7);
            QStringList actions = {
                "Parade décisive du gardien !", "Frappe qui frôle la transversale.",
                "Tacle appuyé, l'arbitre avertit le joueur.", "Corner pour l'équipe locale.",
                "Belle construction au milieu de terrain.", "Le public siffle une faute.", "Contre-attaque rapide !"
            };

            for(int k=0; k<nbEvents; k++) {
                LiveEvent ev;
                ev.minute = random_int(1, 89);
                ev.isMyTeam = true;
                ev.isGoal = false;
                ev.text = QString("%1' %2").arg(ev.minute).arg(actions[random_int(0, actions.size()-1)]);
                liveFeed.push_back(ev);
            }

        } else {
            finalResults.push_back(QString::fromStdString(res.summary));
        }
    }

    std::sort(liveFeed.begin(), liveFeed.end(), [](const LiveEvent& a, const LiveEvent& b){
        return a.minute < b.minute;
    });

    txtLog->append("<div style='text-align:center; color:#777; font-style:italic;'>Le match va commencer...</div>");
    
    btnSimuler->setEnabled(false);
    barMatch->setValue(0);
    matchProgress = 0;
    timerMatch->start(80); 
}

void MainWindow::updateMatchAnim()
{
    matchProgress++; 
    barMatch->setValue(matchProgress);
    lblMatchTime->setText(QString::number(matchProgress) + "'");

    while(!liveFeed.empty() && liveFeed.front().minute <= matchProgress) {
        LiveEvent ev = liveFeed.front();
        
        if(ev.isGoal) {
            if(ev.isHomeGoal) liveScoreDom++; else liveScoreExt++;
            
            lblScoreBig->setText(QString::number(liveScoreDom) + " - " + QString::number(liveScoreExt));
            lblScoreBig->setStyleSheet("font-size: 50px; font-weight: 900; color: #f1c40f;");
            
            QString html = QString(
                "<div style='background-color: #1e2b1e; border-left: 4px solid #2ecc71; padding: 8px; margin: 5px 0;'>"
                "<span style='color: #2ecc71; font-weight: bold; font-size: 14px;'>GOAL !!!</span><br>"
                "<span style='color: #fff;'>%1</span>"
                "</div>").arg(ev.text);
            txtLog->append(html);

        } else {
            QString html = QString(
                "<div style='border-left: 2px solid #555; padding-left: 8px; margin: 2px 0; color: #aaa;'>"
                "%1"
                "</div>").arg(ev.text);
            txtLog->append(html);
        }
        
        liveFeed.erase(liveFeed.begin());
        txtLog->verticalScrollBar()->setValue(txtLog->verticalScrollBar()->maximum());
    }
    
    if(matchProgress % 5 == 0) {
        lblScoreBig->setStyleSheet("font-size: 48px; font-weight: 900; color: #ecf0f1;");
    }

    if(matchProgress >= 90) {
        timerMatch->stop();
        btnSimuler->setEnabled(true);
        lblMatchTime->setText("FT"); 
        
        int points = 0;
        QString homeName = lblTeamHome->text();
        QString myName = QString::fromStdString(monClub->getnom());
        bool iamHome = (homeName == myName);
        
        if(iamHome) {
            if(liveScoreDom > liveScoreExt) points = 3;
            else if(liveScoreDom == liveScoreExt) points = 1;
        } else {
            if(liveScoreExt > liveScoreDom) points = 3;
            else if(liveScoreExt == liveScoreDom) points = 1;
        }

        int tier = currentChamp->get_tier();
        long long gain = 0;
        int multiplier = (tier == 1) ? 10 : (tier == 2 ? 4 : 1);
        
        if(points == 3) gain = 50000 * multiplier;
        else if(points == 1) gain = 15000 * multiplier;
        else gain = 5000 * multiplier;

        if(profilJoueur) {
            profilJoueur->add_argent(gain);
            
            // --- MISE A JOUR NOTE MANAGER ---
            bool levelChanged = profilJoueur->update_rating(points);
            
            QString colorNote = (points == 3) ? "#2ecc71" : ((points == 1) ? "#f39c12" : "#e74c3c");
            QString symbol = (points == 3) ? "📈" : ((points == 1) ? "➡️" : "📉");
            
            txtLog->append("<div style='text-align:center; margin-top:5px;'>"
                           "<span style='color:#aaa;'>Note Manager : </span>"
                           "<b style='color:" + colorNote + ";'>" + QString::number(profilJoueur->get_manager_level()) + "</b> " + symbol + 
                           " <span style='font-size:10px; color:#666;'>(" + QString::number(profilJoueur->get_manager_xp()) + "%)</span>"
                           "</div>");
            
            if(levelChanged) {
                txtLog->append("<div style='text-align:center; font-weight:bold; color:#3498db;'>⭐ NIVEAU MANAGER CHANGÉ ! ⭐</div>");
            }
        }

        txtLog->append("<br><div style='border: 2px solid #f1c40f; padding: 10px; text-align:center;'>");
        txtLog->append("<b style='color:#f1c40f; font-size:16px;'>FIN DU MATCH</b><br>");
        txtLog->append("Gain du match : <b style='color:#2ecc71;'>" + formatMoney(gain) + "</b>");
        txtLog->append("</div><br>");

        txtLog->append("<br><div style='background-color:#222; padding:10px; border-radius:5px;'><b>🌍 RÉSULTATS AILLEURS :</b><br>");
        for(const QString& line : finalResults) {
            txtLog->append("<span style='color:#888;'>" + line + "</span>");
        }
        txtLog->append("</div>");
        
        // --- AVANCEMENT DU TEMPS ---
        jourActuel++;
        currentDate = currentDate.addDays(7); // +1 Semaine par match
        
        refreshDashboard();
        updateClassement(); 
        
        txtLog->verticalScrollBar()->setValue(txtLog->verticalScrollBar()->maximum());
    }
}

void MainWindow::onTabChanged(int index)
{
    if (index == 1) {
        QTimer::singleShot(10, this, [this](){ refreshEffectif(); });
    }
    if (index == 3) { 
        if(tableMarket->rowCount() == 0) updateMarketTable();
    }
}

void MainWindow::onChampionnatChanged(int index)
{
    comboClubs->clear();
    QString champName = comboChampionnats->itemText(index);
    auto it = map_champ.find(champName.toStdString());
    if(it != map_champ.end()) {
        currentChamp = &it->second;
        std::vector<club>* clubs = currentChamp->getliste();
        if(clubs) {
            std::vector<std::string> clubNames;
            for(const auto& c : *clubs) clubNames.push_back(c.getnom());
            std::sort(clubNames.begin(), clubNames.end());
            for(const auto& name : clubNames) comboClubs->addItem(QString::fromStdString(name));
        }
    }
    updateClassement(); 
}

void MainWindow::onVoirClassement()
{
    if(tabRight) tabRight->setCurrentIndex(2);
}

void MainWindow::onSaveGame()
{
    if (!monClub || !currentChamp) return;

    QJsonObject root;
    root["jourActuel"] = jourActuel;
    root["monClub"] = QString::fromStdString(monClub->getnom());
    root["monChampionnat"] = QString::fromStdString(currentChamp->getnom());
    
    // Sauvegarde de la date (Format ISO standard)
    root["date"] = currentDate.toString(Qt::ISODate);
    
    if(profilJoueur) {
        root["argent"] = (qint64)profilJoueur->get_argent();
        root["managerLevel"] = profilJoueur->get_manager_level();
        root["managerXP"] = profilJoueur->get_manager_xp();
    }

    QJsonArray leaguesArray;

    for(const auto& pair : map_champ) {
        QJsonObject leagueObj;
        leagueObj["nom"] = QString::fromStdString(pair.first);

        QJsonArray clubsArray;
        const std::vector<club>* clubs = pair.second.getliste();

        if(clubs) {
            for(const auto& c : *clubs) {
                QJsonObject clubObj;
                clubObj["nom"] = QString::fromStdString(c.getnom());
                clubObj["points"] = c.getnbpoint();
                clubObj["gagne"] = c.get_gagne();
                clubObj["nul"] = c.get_nul();
                clubObj["perdu"] = c.get_perdu();
                clubObj["bp"] = c.get_bp();
                clubObj["bc"] = c.get_bc();
                
                clubsArray.append(clubObj);
            }
        }
        leagueObj["clubs"] = clubsArray;
        leaguesArray.append(leagueObj);
    }

    root["championnats"] = leaguesArray;

    QFile file("savegame.json");
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Erreur", "Impossible de créer le fichier de sauvegarde.");
        return;
    }

    QJsonDocument saveDoc(root);
    file.write(saveDoc.toJson());
    file.close();

    QMessageBox::information(this, "Succès", "Partie sauvegardée avec succès !");
}

void MainWindow::onLoadGame()
{
    QFile file("savegame.json");
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Erreur", "Aucune sauvegarde trouvée (savegame.json).");
        return;
    }

    QByteArray saveData = file.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonObject root = loadDoc.object();

    jourActuel = root["jourActuel"].toInt();
    
    // Chargement de la date
    if(root.contains("date")) {
        currentDate = QDate::fromString(root["date"].toString(), Qt::ISODate);
    } else {
        currentDate = QDate(2025, 8, 8); // Valeur par défaut
    }

    QString savedClubName = root["monClub"].toString();
    QString savedLeagueName = root["monChampionnat"].toString();
    long long savedMoney = root["argent"].toVariant().toLongLong();
    
    int savedLvl = root["managerLevel"].toInt();
    int savedXP = root["managerXP"].toInt();
    if(savedLvl == 0) savedLvl = 65; 

    QJsonArray leaguesArray = root["championnats"].toArray();

    for(const auto& lValue : leaguesArray) {
        QJsonObject lObj = lValue.toObject();
        std::string lName = lObj["nom"].toString().toStdString();

        if(map_champ.find(lName) != map_champ.end()) {
            Championnat& champ = map_champ[lName];
            std::vector<club>* clubs = champ.getliste();

            QJsonArray clubsArray = lObj["clubs"].toArray();
            for(const auto& cValue : clubsArray) {
                QJsonObject cObj = cValue.toObject();
                std::string cName = cObj["nom"].toString().toStdString();

                for(auto& realClub : *clubs) {
                    if(realClub.getnom() == cName) {
                        realClub.set_stats(
                            cObj["points"].toInt(),
                            cObj["gagne"].toInt(),
                            cObj["nul"].toInt(),
                            cObj["perdu"].toInt(),
                            cObj["bp"].toInt(),
                            cObj["bc"].toInt()
                        );
                        break;
                    }
                }
            }
        }
    }

    if(map_champ.find(savedLeagueName.toStdString()) != map_champ.end()) {
        currentChamp = &map_champ[savedLeagueName.toStdString()];
        
        std::vector<club>* clubs = currentChamp->getliste();
        for(auto& c : *clubs) {
            if(c.getnom() == savedClubName.toStdString()) {
                monClub = &c;
                break;
            }
        }
    }

    if(monClub && currentChamp) {
        if(maSaison) delete maSaison;
        if(profilJoueur) delete profilJoueur;

        maSaison = new saison(*currentChamp);
        // Nouveau constructeur Manager
        profilJoueur = new profil(savedMoney, monClub, currentChamp, savedLvl, savedXP);
        profilJoueur->changer_saison(*maSaison);

        lblInfoClub->setText(QString::fromStdString(monClub->getnom()));
        refreshDashboard();
        updateClassement();
        onViewSquad();
        updateMarketTable();

        stack->setCurrentWidget(pageGame);
        QMessageBox::information(this, "Chargé", "Partie chargée ! Journée " + QString::number(jourActuel + 1));
    } else {
        QMessageBox::critical(this, "Erreur", "Données de sauvegarde corrompues.");
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if(monClub && tabRight && tabRight->currentIndex() == 1) {
        refreshEffectif();
    }
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    if(monClub) {
        QTimer::singleShot(100, this, [this](){
            if(tabRight && tabRight->currentIndex() == 1) refreshEffectif();
        });
    }
}


void MainWindow::onSimulateToDate() {
    if(!maSaison) return;

    // 1. Création d'une boite de dialogue avec un calendrier
    QDialog dialog(this);
    dialog.setWindowTitle("Simuler jusqu'à une date");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *label = new QLabel("Choisissez la date cible :");
    QCalendarWidget *calendar = new QCalendarWidget();
    calendar->setMinimumDate(currentDate.addDays(7)); // On ne peut pas remonter le temps
    calendar->setMaximumDate(currentDate.addYears(1));
    calendar->setSelectedDate(currentDate.addDays(7));

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    layout->addWidget(label);
    layout->addWidget(calendar);
    layout->addWidget(buttons);

    // 2. Si l'utilisateur valide
    if (dialog.exec() == QDialog::Accepted) {
        QDate targetDate = calendar->selectedDate();
        
        // Barre de chargement pour faire patienter
        QProgressDialog progress("Simulation en cours...", "Annuler", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.show();

        int weeks = 0;
        // On boucle tant qu'on n'a pas atteint la date ET que la saison n'est pas finie
        while (currentDate < targetDate && jourActuel < maSaison->getnbjour()) {
            simulerSemaineRapide(); // Calcul sans animation
            
            weeks++;
            progress.setValue(weeks % 100); // Juste pour bouger la barre
            QCoreApplication::processEvents(); // Garde l'interface réactive
            
            if (progress.wasCanceled()) break;
        }

        // 3. Rafraîchissement final
        refreshDashboard();
        updateClassement();
        if(tabRight->currentIndex() == 3) updateMarketTable(); // Si on est sur l'onglet mercato
        
        QMessageBox::information(this, "Simulation terminée", 
            QString("Simulation terminée !\n%1 semaines simulées.").arg(weeks));
    }
}

void MainWindow::simulerSemaineRapide() {
    // Logique identique à onSimulateMatch mais sans l'animation visuelle
    
    int nb_matchs = maSaison->getnbmatch();
    int start = jourActuel * nb_matchs;
    std::vector<match>& matches = maSaison->getlist();
    std::string monClubNom = (monClub ? monClub->getnom() : "");
    
    // Jouer tous les matchs de la journée
    int points = 0;
    bool played = false;

    for(int i = start; i < start + nb_matchs && i < matches.size(); i++) {
        MatchResult res = matches[i].jouer_match(); 
        
        // Si c'est mon match, je calcule mes gains
        if(res.dom->getnom() == monClubNom || res.ext->getnom() == monClubNom) {
            played = true;
            bool iamHome = (res.dom->getnom() == monClubNom);
            int scoreMoi = iamHome ? res.score_dom : res.score_ext;
            int scoreAdv = iamHome ? res.score_ext : res.score_dom;

            if (scoreMoi > scoreAdv) points = 3;
            else if (scoreMoi == scoreAdv) points = 1;
            else points = 0;
        }
    }

    // Application des récompenses (Copie de la logique de updateMatchAnim)
    if(played && profilJoueur && currentChamp) {
        int tier = currentChamp->get_tier();
        long long gain = 0;
        int multiplier = (tier == 1) ? 10 : (tier == 2 ? 4 : 1);
        
        if(points == 3) gain = 50000 * multiplier;
        else if(points == 1) gain = 15000 * multiplier;
        else gain = 5000 * multiplier;

        profilJoueur->add_argent(gain);
        profilJoueur->update_rating(points);
    }

    if (jourActuel >= maSaison->getnbjour()) {
        passerSaisonSuivante();
        return;
    }

    // Avancer le temps
    jourActuel++;
    currentDate = currentDate.addDays(7);
}

void MainWindow::passerSaisonSuivante() {
    if (numSaison >= MAX_SAISONS) {
        QMessageBox::information(this, "Carrière Terminée", 
            "Félicitations ! Vous êtes allé au bout des 5 saisons de votre contrat.\n"
            "Merci d'avoir joué à Ultimate Football Manager Simulator 2026 !");
        return;
    }

    QString resumeRetraites = "";
    int totalRetraites = 0;

    // 1. Gestion des retraites pour TOUS les championnats
    // On parcourt la map des championnats
    for (auto& [nomChamp, champObj] : map_champ) {
        std::vector<club>* clubs = champObj.getliste();
        if(clubs) {
            for(auto& c : *clubs) {
                // On applique le vieillissement et les retraites
                std::vector<std::string> departs = c.gestion_fin_saison();
                
                // On note quelques noms pour le log (ceux de notre club ou ligue)
                if (&c == monClub || champObj.getnom() == currentChamp->getnom()) {
                    for(const auto& nom : departs) {
                        resumeRetraites += QString::fromStdString(nom) + " (" + QString::fromStdString(c.getnom()) + ")\n";
                    }
                }
                totalRetraites += departs.size();
            }
        }
    }

    // 2. Mise à jour des variables de temps
    numSaison++;
    jourActuel = 0;
    
    // On avance d'un an (exemple : on repart au 8 Août de l'année suivante)
    currentDate = QDate(currentDate.year() + 1, 8, 8);

    // 3. Réinitialisation de la saison (Génération du nouveau calendrier)
    if(maSaison) delete maSaison;
    maSaison = new saison(*currentChamp);
    
    // Mise à jour du lien dans le profil
    if(profilJoueur) profilJoueur->changer_saison(*maSaison);

    // 4. Feedback Interface
    QMessageBox::information(this, "Nouvelle Saison", 
        QString("Fin de la saison %1 !\n\n"
                "%2 joueurs ont pris leur retraite à travers le monde.\n"
                "Voici quelques départs notables dans votre ligue :\n%3\n"
                "Place à la saison %4 !")
                .arg(numSaison - 1)
                .arg(totalRetraites)
                .arg(resumeRetraites.isEmpty() ? "Aucun" : resumeRetraites)
                .arg(numSaison));

    // Reset affichage
    txtLog->clear();
    lblScoreBig->setText("0 - 0");
    lblMatchTime->setText("0'");
    barMatch->setValue(0);
    
    refreshDashboard();
    updateClassement();
    onViewSquad(); // Pour voir qui est parti
}