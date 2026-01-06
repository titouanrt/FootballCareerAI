#ifndef PLAYERCARD_H
#define PLAYERCARD_H

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QtNetwork/QNetworkAccessManager>
#include "Player.h"

class PlayerCard : public QFrame {
    Q_OBJECT
public:
    PlayerCard(const player& p, QNetworkAccessManager* netManager, QWidget* parent = nullptr);

private:
    QLabel* imgLbl;
    QLabel* infoLbl;
};

#endif // PLAYERCARD_H