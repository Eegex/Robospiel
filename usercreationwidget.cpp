#include "UserCreationWidget.h"
#include <QWidget>
#include <QVariant>
#include <QGridLayout>
#include <QDebug>
#include <QString>
#define DEFAULTPLAYERNAME "Neuer Spieler"

UserCreationWidget::UserCreationWidget(QWidget *parent) : QWidget(parent)
{
    connect(addColourBtn,&QPushButton::clicked,this,&UserCreationWidget::addColour);
    userColourPicker->hide(); //Hide Colour Picker as it's not needed yet
    lay->addWidget(userNamePicker, 0, 0); //Left
    lay->addWidget(addColourBtn, 0, 1); //Center
    lay->addWidget(addUserBtn, 0, 2); //Right
    connect(userColourPicker, &QColorDialog::colorSelected, this, [&](const QColor &colour)
    {
       addColourBtn->setStyleSheet("background-color: "+colour.name());
    });
    connect(addUserBtn, &QPushButton::clicked, this, &UserCreationWidget::addUser);
    connect(addUserBtn, &QPushButton::clicked, this, &UserCreationWidget::resetButtonColour);
    setLayout(lay);
}

void UserCreationWidget::addUser()
{
    qDebug()<<"Addition Request of player with name "<<userNamePicker->text()<<"and colour "<<userColourPicker->selectedColor().name();

    struct UserData newUser;
    newUser.name = userNamePicker->text()!=nullptr?userNamePicker->text():DEFAULTPLAYERNAME;
    newUser.colour = userColourPicker->selectedColor();
    qDebug()<<"Current Colour"<<userColourPicker->selectedColor().name();
    delete userColourPicker;
    userColourPicker = new QColorDialog;
    emit userAdded(&newUser);
    connect(userColourPicker, &QColorDialog::colorSelected, this, [&](const QColor &colour)
    {
       addColourBtn->setStyleSheet("background-color: "+colour.name());
    });
}

void UserCreationWidget::resetButtonColour(){addColourBtn->setStyleSheet("background-color: 0xFFFFFF");}
void UserCreationWidget::addColour(){userColourPicker->show();}
