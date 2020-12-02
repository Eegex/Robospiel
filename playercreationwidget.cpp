#include "PlayerCreationWidget.h"
#include <QWidget>
#include <QVariant>
#include <QGridLayout>

PlayerCreationWidget::PlayerCreationWidget(QWidget *parent) : QWidget(parent)
{
    lay->addWidget(addPlayerBtn);
    lay->addWidget(playerColorPicker);
}

void PlayerCreationWidget::addPlayer()
{
    printf("Addition Request\n");
    //emit playerAdditionRequest();
}



