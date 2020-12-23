#include <string.h>
#include "PlayerBiddingWidget.h"
#include "mainwindow.h"

PlayerBiddingWidget::PlayerBiddingWidget(QWidget *parent) : QWidget(parent)
{
    playerLayout->addWidget(labelName, 0, 0); //Left
    playerLayout->addWidget(lSpinBox, 0, 1); //Center
    playerLayout->addWidget(accept, 0, 2); //Right
	setLayout(playerLayout);
    connect(accept,&QPushButton::clicked,this,&PlayerBiddingWidget::btnPressed); //When player has accepted bid, disable the button
	lSpinBox->setMinimum(1);
	lSpinBox->setSingleStep(1);
	accept->setText("Accept Bid");
}

void PlayerBiddingWidget::setBidding(int bidding)
{
    playerBidding = bidding;
	emit biddingChanged(playerBidding);
}

void PlayerBiddingWidget::setName(QString name)
{
    playerName = name;
    labelName->setText(playerName);
	emit nameChanged();
}

void PlayerBiddingWidget::setColor(QColor colour)
{
    playerColor = colour;
    labelName->setStyleSheet("color: "+playerColor.name());
    qDebug()<<"Player "<<playerName<<" set their colour to"<<playerColor.name();
    emit colourChanged();
}

void PlayerBiddingWidget::btnPressed()
{
    lSpinBox->setMaximum(playerBidding = lSpinBox->value());
    accept->setText("Bid: "+QString::number(playerBidding));
    qDebug()<<"Player "<<playerName<<" changed their bidding to: "<<playerBidding;
    emit biddingChanged(playerBidding);
}

int PlayerBiddingWidget::getBidding(){return playerBidding;}
QString PlayerBiddingWidget::getName(){return playerName;}
