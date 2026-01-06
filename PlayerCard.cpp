#include "PlayerCard.h"
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QPixmap>

PlayerCard::PlayerCard(const player& p, QNetworkAccessManager* netManager, QWidget* parent) 
    : QFrame(parent) 
{
    setFixedSize(110, 150); 
    setStyleSheet(
        "QFrame { background-color: rgba(20, 20, 20, 0.85); border: 2px solid #d4af37; border-radius: 8px; }"
    );

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(0);

    // 1. Zone Image
    imgLbl = new QLabel();
    imgLbl->setAlignment(Qt::AlignCenter);
    imgLbl->setStyleSheet("background: transparent; border: none;");
    imgLbl->setText("..."); 
    
    // Logique de téléchargement
    QString urlStr = QString::fromStdString(p.geturl());
    
    if (!urlStr.isEmpty() && urlStr.startsWith("http") && netManager) {
        QNetworkRequest req((QUrl(urlStr)));
        req.setRawHeader("User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36");

        QNetworkReply *reply = netManager->get(req);
        
        // Connexion locale via lambda pour gérer la réponse
        QObject::connect(reply, &QNetworkReply::finished, imgLbl, [this, reply](){
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray data = reply->readAll();
                QPixmap pixmap;
                if (pixmap.loadFromData(data)) {
                    imgLbl->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    imgLbl->setText(""); 
                } else {
                    imgLbl->setText("?");
                }
            } else {
                imgLbl->setText("X");
            }
            reply->deleteLater();
        });
    } else {
        imgLbl->setText("No Img");
    }

    // 2. Zone Info
    infoLbl = new QLabel();
    infoLbl->setAlignment(Qt::AlignCenter);
    
    QString color = "#ffffff";
    if(p.getlevel() >= 88) color = "#e74c3c"; 
    else if(p.getlevel() >= 80) color = "#2ecc71"; 

    QString shortName = QString::fromStdString(p.getnom());
    if(shortName.length() > 12) shortName = shortName.left(10) + "..";

    infoLbl->setText(QString("<div style='line-height:110%;'>"
                             "<span style='font-size:16px; font-weight:bold; color:%1;'>%2</span><br>"
                             "<span style='font-size:10px; color:#ddd;'>%3</span>"
                             "</div>")
                     .arg(color)
                     .arg(p.getlevel())
                     .arg(shortName));
    
    infoLbl->setStyleSheet("background: transparent; border: none;");
    
    layout->addWidget(imgLbl, 1); 
    layout->addWidget(infoLbl, 0);
}