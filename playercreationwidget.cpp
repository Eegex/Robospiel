#include "PlayerCreationWidget.h"
#include <QWidget>
#include <QVariant>
#include <QGridLayout>
#include <QDebug>
#include <QString>
#define DEFAULTPLAYERNAME "Neuer Spieler"

PlayerCreationWidget::PlayerCreationWidget(QWidget *parent) : QWidget(parent)
{
    connect(addColourBtn,&QPushButton::clicked,this,&PlayerCreationWidget::addColour);
    playerColourPicker->hide(); //Hide Colour Picker as it's not needed yet
    lay->addWidget(playerNamePicker, 0, 0); //Left
    lay->addWidget(addColourBtn, 0, 1); //Center
    lay->addWidget(addPlayerBtn, 0, 2); //Right
    connect(playerColourPicker, &QColorDialog::colorSelected, this, [&](const QColor &colour)
    {
       addColourBtn->setStyleSheet("background-color: "+colour.name());
    });
    connect(addPlayerBtn, &QPushButton::clicked, this, &PlayerCreationWidget::addPlayer);
    connect(addPlayerBtn, &QPushButton::clicked, this, &PlayerCreationWidget::resetButtonColour);
    setLayout(lay);
}

void PlayerCreationWidget::addPlayer()
{
    qDebug()<<"Addition Request of player with name "<<playerNamePicker->text()<<"and colour "<<playerColourPicker->selectedColor().name();

    struct user newUser;
    newUser.name = playerNamePicker->text()!=nullptr?playerNamePicker->text():DEFAULTPLAYERNAME;
    newUser.colour = playerColourPicker->selectedColor();
    qDebug()<<"Current Colour"<<playerColourPicker->selectedColor().name();
    delete playerColourPicker;
    playerColourPicker = new QColorDialog;
    emit playerAdded(&newUser);
    connect(playerColourPicker, &QColorDialog::colorSelected, this, [&](const QColor &colour)
    {
       addColourBtn->setStyleSheet("background-color: "+colour.name());
    });
}

void PlayerCreationWidget::resetButtonColour(){addColourBtn->setStyleSheet("background-color: 0xFFFFFF");}
void PlayerCreationWidget::addColour(){playerColourPicker->show();}
