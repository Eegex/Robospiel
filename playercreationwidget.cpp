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
    connect(playerColourPicker, &QColorDialog::currentColorChanged, this, &PlayerCreationWidget::pickerColourChanged);
    setLayout(lay);
}

void PlayerCreationWidget::addPlayer()
{
    qDebug()<<"Addition Request of player with name "<<playerNamePicker->text()<<"and colour "<<playerColourPicker->selectedColor().name();

    struct user newUser;
    QString names[2] = {DEFAULTPLAYERNAME, playerNamePicker->text()};
    newUser.name = names[playerNamePicker->text() != nullptr];
    QColor colours[2] = {0xAAAAAA, playerColourPicker->selectedColor()};
    newUser.colour = colours[hasColorChanged];
    qDebug()<<"Current Colour"<<playerColourPicker->selectedColor().name();
    hasColorChanged = false;
    emit playerAdded(&newUser);
}

inline void PlayerCreationWidget::pickerColourChanged(){hasColorChanged = true;}
inline void PlayerCreationWidget::resetButtonColour(){addColourBtn->setStyleSheet("background-color: 0xFFFFFF");}
inline void PlayerCreationWidget::addColour(){playerColourPicker->show();}
