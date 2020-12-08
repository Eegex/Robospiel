#ifndef PLAYERCREATIONWIDGET_H
#define PLAYERCREATIONWIDGET_H

#include <QWidget>
#include <QVariant>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QColor>
#include <QColorDialog>
#include <QLineEdit>

class PlayerCreationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerCreationWidget(QWidget *parent = nullptr);

private:
    QGridLayout * lay = new QGridLayout(this);
    QLabel * lDisplay = new QLabel("PlayerCreationWidget", this);
    QPushButton * addPlayerBtn = new QPushButton("Spieler hinzufügen", this); //Translate-Funktion
    QPushButton * addColourBtn = new QPushButton("Farbe hinzufügen", this);
    QColor * playerColor = new QColor();
    QColorDialog * playerColourPicker = new QColorDialog();
    QLineEdit * playerNamePicker = new QLineEdit();
    u_int8_t numOfPlayers = 0;

public slots:
    void addPlayer();
    void addColour();
signals:
    void playerAdded(); //Argument: Nutzer
};
#endif // PLAYERCREATIONWIDGET_H
