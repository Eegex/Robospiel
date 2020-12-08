#include "PlayerCreationWidget.h"
#include <QWidget>
#include <QVariant>
#include <QGridLayout>
#include <QDebug>

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
    setLayout(lay);
}

void PlayerCreationWidget::addPlayer()
{
    printf("Addition Request\n");
    //emit playerAdditionRequest();
}

void PlayerCreationWidget::addColour()
{
    playerColourPicker->show();
}

/*void PlayerCreationWidget::playerAdded()
{
    qDebug()<<"Player Added";

}*/



