#include "mainwindow.h"
#include <QApplication>

// Fonction pour définir le look "Pro Dark Gaming"
void setGameStyle(QApplication &a) {
    QString style = R"(
        /* --- GLOBAL --- */
        QMainWindow {
            background-color: #121212; /* Noir profond */
        }
        QWidget {
            color: #e0e0e0;
            font-family: 'Segoe UI', 'Roboto', sans-serif;
            font-size: 14px;
        }

        /* --- GROUPBOX & FRAMES --- */
        QGroupBox {
            border: 1px solid #333;
            border-radius: 8px;
            margin-top: 20px;
            font-weight: bold;
            background-color: #1e1e1e;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 15px;
            padding: 0 5px;
            color: #2ecc71;
        }

        /* --- BOUTONS --- */
        QPushButton {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2ecc71, stop:1 #27ae60);
            color: #ffffff;
            border: none;
            padding: 10px 24px;
            border-radius: 6px;
            font-weight: bold;
            font-size: 15px;
        }
        QPushButton:hover {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2ecc71, stop:1 #2ecc71);
            border: 1px solid #45f78a;
        }
        QPushButton:pressed {
            background-color: #1e8449;
            padding-top: 12px; /* Effet d'enfoncement */
        }
        QPushButton:disabled {
            background-color: #34495e;
            color: #7f8c8d;
        }

        /* --- INPUTS --- */
        QLineEdit, QComboBox {
            background-color: #252526;
            border: 1px solid #3e3e42;
            padding: 8px;
            color: white;
            border-radius: 4px;
            selection-background-color: #2ecc71;
        }
        QComboBox::drop-down {
            border: none;
            background: #252526;
        }

        /* --- LISTES & LOGS --- */
        QListWidget, QTextEdit {
            background-color: #1e1e1e;
            border: 1px solid #333;
            color: #ecf0f1;
            border-radius: 6px;
            padding: 5px;
        }

        /* --- TABLEAUX (CLASSEMENT) --- */
        QTableWidget {
            background-color: #1e1e1e;
            border: 1px solid #333;
            gridline-color: #2c2c2c;
            selection-background-color: #2ecc71;
            selection-color: black;
            alternate-background-color: #252526; /* Lignes alternées */
        }
        QHeaderView::section {
            background-color: #2d2d30;
            color: #2ecc71;
            padding: 5px;
            border: none;
            border-bottom: 2px solid #2ecc71;
            font-weight: bold;
        }

        /* --- BARRE DE PROGRESSION --- */
        QProgressBar {
            border: none;
            background-color: #2d2d30;
            border-radius: 4px;
            text-align: center;
            color: white;
            font-weight: bold;
        }
        QProgressBar::chunk {
            background-color: #e74c3c;
            border-radius: 4px;
        }

        /* --- ONGLETS --- */
        QTabWidget::pane {
            border: 1px solid #333;
            background: #1e1e1e;
        }
        QTabBar::tab {
            background: #2b2b2b;
            color: #aaa;
            padding: 10px 20px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
            margin-right: 2px;
        }
        QTabBar::tab:selected {
            background: #1e1e1e;
            color: #2ecc71;
            border-bottom: 2px solid #2ecc71;
        }
        
        /* --- SCROLLBARS (Modernes) --- */
        QScrollBar:vertical {
            border: none;
            background: #121212;
            width: 10px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background: #444;
            min-height: 20px;
            border-radius: 5px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
    )";
    a.setStyleSheet(style);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setGameStyle(a); 

    MainWindow w;
    w.show();
    return a.exec();
}