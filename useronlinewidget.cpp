#include "useronlinewidget.h"
#include <QDebug>

UserOnlineWidget::UserOnlineWidget(QWidget *parent) : QWidget(parent)
{
    bidBtn->hide();
    name->hide();
    biddingBox->hide();
    listWidget->hide();
    lay->addWidget(listWidget);
    listWidget->show();
    setOnlineWidget();
    setLayout(lay);
    name->setText(username);
    name->show();
    connect(bidBtn,&QPushButton::clicked, this, &UserOnlineWidget::btnPressed);
}

void UserOnlineWidget::setOnlineWidget()
{
    biddingBox->setMinimum(1);
    biddingBox->setSingleStep(1);
    bidBtn->setText(BID_BTN_TEXT);
    lay->addWidget(biddingBox);
    lay->addWidget(bidBtn);
    biddingBox->show();
    bidBtn->show();
    lay->update();
}

void UserOnlineWidget::setBidding(int bidding)
{
    userBidding = bidding;
    emit biddingChangedOnline(userBidding);
}

void UserOnlineWidget::btnPressed()
{
    biddingBox->setMaximum(userBidding = biddingBox->value());
    bidBtn->setText("Bid: "+QString::number(userBidding));
    qDebug()<<"Player changed their bidding to: "<<userBidding;
    emit biddingChangedOnline(userBidding);
}

void UserOnlineWidget::updateName(QString newName)
{
    username = newName;
    name->setText(username);
    lay->update();
}

/* void UserOnlineWidget::loadUsers()
{

} */