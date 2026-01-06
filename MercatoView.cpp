#include "MercatoView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDoubleValidator>
#include <algorithm>

MercatoView::MercatoView(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void MercatoView::setGameContext(std::map<std::string, Championnat>* champs, club* myClub, profil* myProfil) {
    map_champ = champs;
    monClub = myClub;
    profilJoueur = myProfil;
    
    comboMarketChamp->clear();
    comboMarketChamp->addItem("Tous les championnats");
    if(map_champ) {
        for(const auto& pair : *map_champ) {
            comboMarketChamp->addItem(QString::fromStdString(pair.first));
        }
        comboMarketChamp->model()->sort(0);
    }
    updateTable();
}

void MercatoView::refresh(bool isOpen) {
    isMarketOpen = isOpen;
    if(isMarketOpen) {
        lblMercatoStatus->setText("🟢 MERCATO OUVERT");
        lblMercatoStatus->setStyleSheet("background-color: #27ae60; color: white; font-weight: bold; padding: 10px; border-radius: 5px;");
        btnBuyPlayer->setEnabled(true);
        btnBuyPlayer->setText("ACHETER LE JOUEUR SÉLECTIONNÉ 💰");
        btnBuyPlayer->setStyleSheet("background-color: #27ae60; color: white; padding: 15px; font-weight: bold; border-radius: 5px;");
    } else {
        lblMercatoStatus->setText("🔴 MERCATO FERMÉ");
        lblMercatoStatus->setStyleSheet("background-color: #c0392b; color: white; font-weight: bold; padding: 10px; border-radius: 5px;");
        btnBuyPlayer->setEnabled(false);
        btnBuyPlayer->setText("MERCATO FERMÉ 🔒");
        btnBuyPlayer->setStyleSheet("background-color: #7f8c8d; color: #bdc3c7; padding: 15px; font-weight: bold; border-radius: 5px;");
    }
}

void MercatoView::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    lblMercatoStatus = new QLabel("STATUT MERCATO");
    lblMercatoStatus->setAlignment(Qt::AlignCenter);
    
    QGroupBox *boxFilter = new QGroupBox("🔍 Filtres");
    boxFilter->setStyleSheet("QGroupBox { border: 1px solid #444; border-radius: 5px; margin-top: 10px; color:#ccc; }");
    QGridLayout *filterGrid = new QGridLayout(boxFilter);
    
    searchMarket = new QLineEdit(); searchMarket->setPlaceholderText("Nom...");
    comboMarketChamp = new QComboBox(); 
    comboMarketClub = new QComboBox(); comboMarketClub->addItem("Tous les clubs");
    comboMarketPoste = new QComboBox(); comboMarketPoste->addItems({"Tous les postes", "Gardiens", "Défenseurs", "Milieux", "Attaquants"});
    searchMinPrice = new QLineEdit(); searchMinPrice->setPlaceholderText("Min €");
    searchMaxPrice = new QLineEdit(); searchMaxPrice->setPlaceholderText("Max €");
    
    filterGrid->addWidget(searchMarket, 0, 0, 1, 2);
    filterGrid->addWidget(comboMarketChamp, 0, 2); filterGrid->addWidget(comboMarketClub, 0, 3);
    filterGrid->addWidget(comboMarketPoste, 1, 0); 
    filterGrid->addWidget(searchMinPrice, 1, 2); filterGrid->addWidget(searchMaxPrice, 1, 3);

    tableMarket = new QTableWidget();
    tableMarket->setColumnCount(12);
    tableMarket->setHorizontalHeaderLabels({"Nom", "Age", "Poste", "Club", "OVR", "PAC", "SHO", "PAS", "DRI", "DEF", "PHY", "Valeur"});
    tableMarket->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tableMarket->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableMarket->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableMarket->setSortingEnabled(true);

    btnBuyPlayer = new QPushButton("ACHETER");

    layout->addWidget(lblMercatoStatus);
    layout->addWidget(boxFilter);
    layout->addWidget(tableMarket);
    layout->addWidget(btnBuyPlayer);

    connect(comboMarketChamp, &QComboBox::currentTextChanged, this, [this](const QString &text){
        comboMarketClub->blockSignals(true);
        comboMarketClub->clear(); comboMarketClub->addItem("Tous les clubs");
        if (map_champ && text != "Tous les championnats" && map_champ->find(text.toStdString()) != map_champ->end()) {
             std::vector<club>* clubs = (*map_champ)[text.toStdString()].getliste();
             std::vector<std::string> names;
             for(auto& c : *clubs) names.push_back(c.getnom());
             std::sort(names.begin(), names.end());
             for(const auto& name : names) comboMarketClub->addItem(QString::fromStdString(name));
        }
        comboMarketClub->blockSignals(false);
        updateTable();
    });
    connect(comboMarketClub, &QComboBox::currentTextChanged, this, &MercatoView::updateTable);
    connect(comboMarketPoste, &QComboBox::currentTextChanged, this, &MercatoView::updateTable);
    connect(searchMarket, &QLineEdit::textChanged, this, &MercatoView::updateTable);
    connect(btnBuyPlayer, &QPushButton::clicked, this, &MercatoView::onBuyClicked);
}

void MercatoView::updateTable() {
    if(!map_champ) return;
    
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

    for (auto const& [nomChamp, champObj] : *map_champ) {
        if (filterChamp != "Tous les championnats" && QString::fromStdString(nomChamp) != filterChamp) continue;

        std::vector<club>* clubs = champObj.getliste();
        if(!clubs) continue;

        for (const auto& c : *clubs) {
            std::string clubName = c.getnom();
            if (filterClub != "Tous les clubs" && QString::fromStdString(clubName) != filterClub) continue;
            
            float clubScore = c.getscore();

            for (const auto& p : c.getEffectif()) {
                QString qNom = QString::fromStdString(p.getnom());
                std::string pPosStr = p.getposte();
                // CORRECTION ICI : Conversion en QString pour utiliser .contains()
                QString qPos = QString::fromStdString(pPosStr);

                if (!search.isEmpty() && !qNom.contains(search, Qt::CaseInsensitive)) continue;
                
                bool matchPoste = (filterPoste == "Tous les postes");
                if (!matchPoste) {
                    if (filterPoste == "Gardiens" && qPos == "GK") matchPoste = true;
                    // Utilisation de qPos (QString) au lieu de pPosStr (std::string)
                    else if (filterPoste == "Défenseurs" && (qPos.contains("B") || qPos == "DC")) matchPoste = true;
                    else if (filterPoste == "Milieux" && qPos.contains("M")) matchPoste = true;
                    else if (filterPoste == "Attaquants" && (qPos == "ST" || qPos == "CF" || qPos.contains("W"))) matchPoste = true;
                }
                if (!matchPoste) continue;

                double val = p.calculate_value(clubScore);
                if (val < minPrice || val > maxPrice) continue;

                tableMarket->insertRow(row);
                QTableWidgetItem *itNom = new QTableWidgetItem(qNom);
                itNom->setData(Qt::UserRole, p.getid());
                tableMarket->setItem(row, 0, itNom);
                tableMarket->setItem(row, 1, new QTableWidgetItem(QString::number(p.getage())));
                tableMarket->setItem(row, 2, new QTableWidgetItem(qPos));
                tableMarket->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(clubName)));
                tableMarket->setItem(row, 4, new QTableWidgetItem(QString::number(p.getlevel())));
                
                tableMarket->setItem(row, 5, new QTableWidgetItem(QString::number(p.get_pac())));
                tableMarket->setItem(row, 6, new QTableWidgetItem(QString::number(p.get_sho())));
                tableMarket->setItem(row, 7, new QTableWidgetItem(QString::number(p.get_pas())));
                tableMarket->setItem(row, 8, new QTableWidgetItem(QString::number(p.get_dri())));
                tableMarket->setItem(row, 9, new QTableWidgetItem(QString::number(p.get_def())));
                tableMarket->setItem(row, 10, new QTableWidgetItem(QString::number(p.get_phy())));

                QTableWidgetItem *itVal = new QTableWidgetItem(formatMoney(val));
                itVal->setData(Qt::UserRole, val);
                tableMarket->setItem(row, 11, itVal);
                row++;
                if (row > 500) break;
            }
            if (row > 500) break;
        }
        if (row > 500) break;
    }
    tableMarket->setSortingEnabled(true);
}

void MercatoView::onBuyClicked() {
    if(!isMarketOpen) {
        QMessageBox::warning(this, "Fermé", "Mercato fermé."); return;
    }
    if (!tableMarket->currentItem() || !monClub || !profilJoueur) return;

    int row = tableMarket->currentRow();
    int idJoueur = tableMarket->item(row, 0)->data(Qt::UserRole).toInt();
    QString nomClubSource = tableMarket->item(row, 3)->text();
    double prix = tableMarket->item(row, 11)->data(Qt::UserRole).toDouble();

    if (nomClubSource == QString::fromStdString(monClub->getnom())) {
        QMessageBox::information(this, "Info", "Déjà chez vous."); return;
    }
    if (!profilJoueur->spend_argent((long long)prix)) {
        QMessageBox::critical(this, "Erreur", "Fonds insuffisants."); return;
    }

    club* clubVendeur = nullptr;
    for (auto& [nom, champ] : *map_champ) {
        std::vector<club>* liste = champ.getliste();
        for (auto& c : *liste) {
            if (c.getnom() == nomClubSource.toStdString()) {
                clubVendeur = &c; break;
            }
        }
        if (clubVendeur) break;
    }

    if (clubVendeur) {
        player joueurTransfere;
        bool found = false;
        for (const auto& p : clubVendeur->getEffectif()) {
            if (p.getid() == idJoueur) {
                joueurTransfere = p; found = true; break;
            }
        }
        if (found) {
            clubVendeur->remove_player(idJoueur);
            monClub->ajouter_joueur(joueurTransfere);
            QMessageBox::information(this, "Succès", "Joueur acheté !");
            updateTable();
            emit playerBought();
        }
    }
}

QString MercatoView::formatMoney(double value) {
    if (value >= 1000000) return QString::number(value / 1000000.0, 'f', 1) + " M€";
    else if (value >= 1000) return QString::number(value / 1000.0, 'f', 0) + " K€";
    return QString::number(value) + " €";
}