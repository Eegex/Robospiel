#include <string.h>
#include "PlayerBiddingWidget.h"
#include "mainwindow.h"

PlayerBiddingWidget::PlayerBiddingWidget(QWidget *parent) : QWidget(parent)
{
	playerLayout->addWidget(labelName, 0, 0);
	playerLayout->addWidget(lSpinBox, 0, 1);
	playerLayout->addWidget(accept, 0, 2);
	setLayout(playerLayout);
	connect(accept,&QPushButton::clicked,this,&PlayerBiddingWidget::btnPressed);
	lSpinBox->setMinimum(1);
	lSpinBox->setSingleStep(1);
	accept->setText("Accept Bid");
}

void PlayerBiddingWidget::setBidding(QVariant v)
{
	playerBidding = v.canConvert<QString>()?v.toInt():999;
	emit biddingChanged(playerBidding);
}

void PlayerBiddingWidget::setName(QVariant v)
{
	playerName = v.canConvert<QString>()?v.toString():"Ungültig";
	labelName->setText(playerName);
	//update();
	emit nameChanged();
}

void PlayerBiddingWidget::btnPressed()
{
	playerBidding = lSpinBox->value();
	hasBid = true;
	accept->setEnabled(false); //Disable Button
	lSpinBox->setEnabled(false); //Disable SpinBox
	accept->setText("Bid: "+QString::number(playerBidding));
	//labelName->setText("Btn Pressed");
}

unsigned int PlayerBiddingWidget::getBidding()
{
	return playerBidding;
}

QString PlayerBiddingWidget::getName()
{
	return playerName;
}
