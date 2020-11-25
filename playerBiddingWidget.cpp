#include "PlayerBiddingWidget.h"
#include "mainwindow.h"

PlayerBiddingWidget::PlayerBiddingWidget(QWidget *parent) : QWidget(parent)
{
    setLayout(playerLayout);
}

void PlayerBiddingWidget::setBidding(QVariant v)
{
    playerBidding = v.canConvert<QString>()?v.toInt():999;
    emit biddingChanged();
}

void PlayerBiddingWidget::setName(QVariant v)
{
    playerName = v.canConvert<QString>()?v.toString():"Ungültig";
    emit nameChanged();
}

u_int PlayerBiddingWidget::getBidding()
{
    return playerBidding;
}

QString PlayerBiddingWidget::getName()
{
    return playerName;
}
