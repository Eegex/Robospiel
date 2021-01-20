#include "useronlinewidget.h"
#include <QDebug>

UserOnlineWidget::UserOnlineWidget(QWidget *parent) : QWidget(parent)
{
    bidBtn->hide();
    name->hide();
    biddingBox->hide();
    listWidget->hide();
    lay->addWidget(listWidget);
    listWidget->hide();
    setOnlineWidget();
    setLayout(lay);
    connect(bidBtn,&QPushButton::clicked, this, &UserOnlineWidget::btnPressed);
}

void UserOnlineWidget::setOnlineWidget()
{
    biddingBox->setMinimum(1);
    biddingBox->setSingleStep(1);
    bidBtn->setText(BID_BTN_TEXT);
    lay->addWidget(biddingBox);
    lay->addWidget(bidBtn);
    name->setText(username);
    name->show();

    listWidget->setRowCount(2);
    listWidget->setColumnCount(2);
    QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg((2)*(2)));
    newItem->setText(username);
    listWidget->setItem(0, 0, newItem);
    QTableWidgetItem *newItem2 = new QTableWidgetItem(tr("%1").arg((2)*(3)));
    newItem2->setText(QString::number(userBidding));
    listWidget->setItem(0, 1, newItem2);

    listWidget->show();
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

void UserOnlineWidget::addUserToList(User *u)
{
    users.append(u);
}

void UserOnlineWidget::updateUserList()
{

}

/* void UserOnlineWidget::loadUsers()
{

} */
