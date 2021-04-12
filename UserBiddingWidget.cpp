#include <string.h>
#include "UserBiddingWidget.h"
#include "mainwindow.h"

UserBiddingWidget::UserBiddingWidget(User* u/*, QWidget *parent*/)// : QWidget(parent)
{
    user = u;
	userLayout->addWidget(labelName, 0, 0); //Left
	userLayout->addWidget(lSpinBox, 0, 1); //Center
	userLayout->addWidget(accept, 0, 2); //Right
    userLayout->addWidget(pointsName, 0, 3); //Far Right (AfD)
	setLayout(userLayout);
	connect(accept,&QPushButton::clicked,this,&UserBiddingWidget::btnPressed); //When player has accepted bid, disable the button
	lSpinBox->setMinimum(MIN_BID);
	lSpinBox->setSingleStep(1);
    updateLayout();
}

void UserBiddingWidget::deactivateBtn(){
    if(user->getBidding() == MAX_BID)
	{
        user->setTimeStamp(QDateTime::currentMSecsSinceEpoch());
	}
	qDebug()<<"Called Function Deactivate Button in UserBiddingWidget";
	accept->setEnabled(false);
}

void UserBiddingWidget::updateLayout()
{
    labelName->setText(user->getName());
    labelName->setStyleSheet("color: "+user->getColor().name());
    pointsName->setText("Points: "+QString::number(user->getPoints()));
    lSpinBox->setMaximum(user->getBidding());
    if(user->getBidding()==MAX_BID)
    {
        accept->setText(BID_BTN_TEXT);
    }
    else
    {
        accept->setText("Bid: "+QString::number(user->getBidding()));
    }
}

void UserBiddingWidget::btnPressed()
{
    emit biddingChanged(lSpinBox->value(), QDateTime::currentMSecsSinceEpoch(), user->getId());
    qDebug()<<"Player "<<user->getName()<<" changed their bidding to: "<<lSpinBox->value();
}

QUuid UserBiddingWidget::getId(){return user->getId();}

