#include "PlayerCreationWidget.h"
#include <QWidget>
#include <QVariant>
#include <QGridLayout>
#include <QDebug>
#include <QString>
#include "user.h"

PlayerCreationWidget::PlayerCreationWidget(QWidget *parent) : QWidget(parent)
{
    connect(addColourBtn,&QPushButton::clicked,this,&PlayerCreationWidget::addColour);
    playerColourPicker->hide();
    //lay->addWidget(playerColorPicker, 0, 0);
    lay->addWidget(addPlayerBtn, 0, 0);
    lay->addWidget(playerNamePicker, 0, 1);
    lay->addWidget(addColourBtn, 0, 2);
    connect(playerColourPicker, &QColorDialog::colorSelected, this, [&](const QColor &colour)
    {
       addColourBtn->setStyleSheet("background-color: "+colour.name());
    });
    connect(addPlayerBtn, &QPushButton::clicked, this, &PlayerCreationWidget::addPlayer);

    setLayout(lay);
}

void PlayerCreationWidget::addPlayer()
{
    qDebug()<<"Addition Request";
    newUser = new User(playerNamePicker->text(), playerColourPicker->selectedColor(), nullptr); //Name, Colour, Parent
    emit playerAdded(newUser);
}

void PlayerCreationWidget::addColour()
{
    qDebug()<<"AddColour";
    playerColourPicker->show();
}
