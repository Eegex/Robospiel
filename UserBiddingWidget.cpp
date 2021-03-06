#include <string.h>
#include "UserBiddingWidget.h"
#include "mainwindow.h"

UserBiddingWidget::UserBiddingWidget(User * u, QWidget *parent) : QWidget(parent)
{
	user = u;
	userLayout->addWidget(labelName, 0, 0); //Left
	userLayout->addWidget(lSpinBox, 0, 1); //Center
	userLayout->addWidget(accept, 0, 2); //Right
	userLayout->addWidget(pointsName, 0, 3); //Far Right (AfD)
	setLayout(userLayout);
	connect(accept,&QPushButton::clicked,this,&UserBiddingWidget::btnPressed); //When player has accepted bid, disable the button
	connect(user, &User::biddingChanged, this, [&](QUuid id, int bid)
	{
		if(id == user->getId())
		{
			updateBidding(bid);
		}
	});
	connect(user, &User::biddingChanged, this, [&](QUuid, int)
	{
		updateLayout();
	});
	lSpinBox->setMinimum(MIN_BID);
	lSpinBox->setSingleStep(1);
	lSpinBox->setValue(user->getBidding());
	updateLayout();
	qDebug()<<"In Creation of UserBiddingWidget, Name is: "<<labelName<<" points are "<<lSpinBox->value();
}

/**
 * @brief UserBiddingWidget::deactivateBtn
 * This function deactivates the bid button. What this function also does is it sets the bidding timestamp for all the people that haven't bid this round.
 * This is needed for the edge case where someone legitimately bids the maximum amount but then has to wait for all the people whose bid was set to the maximum amount since technically
 * those people bid before the legitimate person.
 */
void UserBiddingWidget::deactivateBtn()
{
	if(!user->getHasBid())
	{
		user->setTimeStamp(QDateTime::currentMSecsSinceEpoch());
	}
	qDebug()<<"Called Function Deactivate Button in UserBiddingWidget";
	accept->setEnabled(false);
}

void UserBiddingWidget::activateBtn(){
	qDebug()<<"Called Function Activate Button in UserBiddingWidget";
	accept->setEnabled(true);
	lSpinBox->setEnabled(true);
}

/**
 * @brief UserBiddingWidget::updateLayout
 * This function updates all the layout components associated with a user.
 */
void UserBiddingWidget::updateLayout()
{
	labelName->setText(user->getName());
	labelName->setStyleSheet("color: "+user->getColor().name());
	pointsName->setText("Points: "+QString::number(user->getPoints()));
	lSpinBox->setMaximum(user->getBidding());
	if(!user->getHasBid())
	{
		accept->setText(BID_BTN_TEXT);
	}
	else
	{
		accept->setText("Bid: "+QString::number(user->getBidding()));
	}
}

void UserBiddingWidget::updateBidding(int bidding){
	lSpinBox->setValue(bidding);
	updateLayout();
}

void UserBiddingWidget::updateName(QString name){
	labelName->setText(name);
	updateLayout();
}

void UserBiddingWidget::updateColour(QColor colour){
	labelName->setStyleSheet("color: "+colour.name());
	updateLayout();
}

void UserBiddingWidget::btnPressed()
{
	if(lSpinBox->value() > MIN_BID){
		emit biddingChanged(lSpinBox->value(), QDateTime::currentMSecsSinceEpoch(), user->getId());
		qDebug()<<"Player "<<user->getName()<<" changed their bidding to: "<<lSpinBox->value();
	}else{
		qDebug()<<"Someone has tried to bid No Bid";
	}
}

User* UserBiddingWidget::getUser()
{
	return user;
}

QUuid UserBiddingWidget::getId()
{
	return user->getId();
}
