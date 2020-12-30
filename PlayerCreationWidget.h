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
#include "user.h"

class PlayerCreationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerCreationWidget(QWidget *parent = nullptr);

private:
    QGridLayout * lay = new QGridLayout(this);
    QPushButton * addPlayerBtn = new QPushButton("Spieler hinzufügen", this); //Translate-Funktion
    QPushButton * addColourBtn = new QPushButton("Farbe hinzufügen", this);
    QColor * playerColor = new QColor();
    QColorDialog * playerColourPicker = new QColorDialog();
    QLineEdit * playerNamePicker = new QLineEdit();
    unsigned int numOfPlayers = 0;
    bool hasColorChanged = false;
    User * newUser;

public slots:
    void addPlayer();
    void addColour();
    void resetButtonColour();
    void pickerColourChanged();
signals:
    void playerAdded(User * newUser); //Argument: Nutzer
    //void playerAdded(QString text, QColor selectedColor);
};
#endif // PLAYERCREATIONWIDGET_H
