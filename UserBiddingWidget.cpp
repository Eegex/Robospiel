#include <string.h>
#include "UserBiddingWidget.h"
#include "mainwindow.h"

UserBiddingWidget::UserBiddingWidget(QWidget *parent) : QWidget(parent)
{
    userLayout->addWidget(labelName, 0, 0); //Left
    userLayout->addWidget(lSpinBox, 0, 1); //Center
    userLayout->addWidget(accept, 0, 2); //Right
	setLayout(userLayout);
    connect(accept,&QPushButton::clicked,this,&UserBiddingWidget::btnPressed); //When player has accepted bid, disable the button
	lSpinBox->setMinimum(1);
	lSpinBox->setSingleStep(1);
	accept->setText("Accept Bid");
}

void UserBiddingWidget::setBidding(int bidding)
{
    userBidding = bidding;
	emit biddingChanged(userBidding);
}

void UserBiddingWidget::setName(QString name)
{
    userName = name;
    labelName->setText(userName);
	emit nameChanged();
}

void UserBiddingWidget::setColor(QColor colour)
{
    userColor = colour;
    labelName->setStyleSheet("color: "+userColor.name());
    qDebug()<<"Player "<<userName<<" set their colour to"<<userColor.name();
    emit colourChanged();
}

void UserBiddingWidget::btnPressed()
{
    lSpinBox->setMaximum(userBidding = lSpinBox->value());
    accept->setText("Bid: "+QString::number(userBidding));
    qDebug()<<"Player "<<userName<<" changed their bidding to: "<<userBidding;
    emit biddingChanged(userBidding);
}

int UserBiddingWidget::getBidding(){return userBidding;}
QString UserBiddingWidget::getName(){return userName;}
