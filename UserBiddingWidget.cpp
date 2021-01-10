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
    lSpinBox->setValue(MAX_BID);
	lSpinBox->setSingleStep(1);
    accept->setText(BID_BTN_TEXT);
}

void UserBiddingWidget::setId(QUuid id)
{
    userId = id;
}

void UserBiddingWidget::deactivateBtn(){
    qDebug()<<"Deactivate Button";
    accept->setEnabled(false);
}

void UserBiddingWidget::resetBidding()
{
    qDebug("Reset Bidding");
    userBidding = MAX_BID;
    accept->setText(BID_BTN_TEXT);
    accept->setEnabled(true);
    lSpinBox->setValue(MAX_BID);
    emit biddingReset(MAX_BID, userId);
}

void UserBiddingWidget::setBidding(int bidding)
{
    userBidding = bidding;
    emit biddingChanged(userBidding, userId);
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
    emit biddingChanged(userBidding, userId);
}

QUuid UserBiddingWidget::getId(){return userId;}
int UserBiddingWidget::getBidding(){return userBidding;}
QString UserBiddingWidget::getName(){return userName;}
