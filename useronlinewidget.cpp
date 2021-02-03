#include "useronlinewidget.h"
#include <QDebug>

UserOnlineWidget::UserOnlineWidget(QWidget *parent) : QWidget(parent)
{
    bidBtn->hide();
    name->hide();
    biddingBox->hide();
    listWidget->hide();
    listWidget->hide();
    setOnlineWidget();
    setLayout(lay);


    connect(bidBtn,&QPushButton::clicked, this, &UserOnlineWidget::btnPressed);
}

void UserOnlineWidget::setOnlineWidget()
{
    qDebug()<<"First things first";
    biddingBox->setMinimum(1);
    biddingBox->setSingleStep(1);
    bidBtn->setText(BID_BTN_TEXT);
    lay->addWidget(name);
    lay->addWidget(biddingBox);
    lay->addWidget(bidBtn);
    lay->addWidget(listWidget);
    name->setText(username);
    name->show();

    setTable();

    listWidget->show();
    biddingBox->show();
    bidBtn->show();
    lay->update();
}

void UserOnlineWidget::setTable()
{
    qDebug()<<"list widget wird erstellt";
    listWidget->setRowCount(1);
    listWidget->setColumnCount(3);
    QStringList header = {tr("Names"),tr("Bidding"),tr("Points")};
    listWidget->setHorizontalHeaderLabels(header);
    // set name column
    QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg((2)*(2)));
    newItem->setText(username);
    newItem->setData(1,userId);
    listWidget->setItem(0, 0, newItem);
    // set bidding column
    QTableWidgetItem *newItem2 = new QTableWidgetItem(tr("%1").arg((2)*(3)));
    newItem2->setText(QString::number(userBidding));
    newItem2->setData(1,userId);
    listWidget->setItem(0, 1, newItem2);
    // set point column
    QTableWidgetItem *newItem3 = new QTableWidgetItem(tr("%1").arg((2)*(4)));
    newItem3->setText(QString::number(0));
    newItem3->setData(1,userId);
    listWidget->setItem(0, 2, newItem3);

}

void UserOnlineWidget::setBidding(QUuid id, int bidding)
{
    qDebug()<<"useronlinewidget setBidding to "<<bidding;
    QString name = "";
    for(User *u : users)
    {
        if(u->getId() == id)
        {
            name = u->getName();
        }
    }
    for(QTableWidgetItem *item : listWidget->findItems(name,0))
    {
        if(item->data(1) == id && item->text() == name)
        {
            listWidget->item(item->row(),item->column()+1)->setText(QString::number(bidding));
            return;
        }
    }
    userBidding = bidding;
    // emit biddingChangedOnline(userBidding);
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

void UserOnlineWidget::setUserID(QUuid id)
{
    userId = id;
}

/* void UserOnlineWidget::loadUsers()
{

} */
