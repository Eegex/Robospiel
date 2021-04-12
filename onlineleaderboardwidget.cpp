#include "onlineleaderboardwidget.h"
#include <QDebug>
#include <QBrush>

OnlineLeaderboardWidget::OnlineLeaderboardWidget()
{
}

void OnlineLeaderboardWidget::initialize() {
    bidBtn->hide();
    lname->hide();
    biddingBox->hide();
    listWidget->hide();
    listWidget->hide();
    setOnlineWidget();
    setLayout(lay);

    connect(bidBtn,&QPushButton::clicked, this, &OnlineLeaderboardWidget::btnPressed);

}

void OnlineLeaderboardWidget::setOnlineWidget()
{
    biddingBox->setMinimum(1);
    biddingBox->setSingleStep(1);
    bidBtn->setText(BID_BTN_TEXT);
    lay->addWidget(lname);
    lay->addWidget(biddingBox);
    lay->addWidget(bidBtn);
    lay->addWidget(listWidget);
    lname->setText(localUser->getName());
    lname->show();

    setTable();

    listWidget->show();
    biddingBox->show();
    bidBtn->show();
    lay->update();
}

void OnlineLeaderboardWidget::setTable()
{
    qDebug()<<"list widget wird erstellt";
    listWidget->setRowCount(1);
    listWidget->setColumnCount(3);
    QStringList header = {tr("Names"),tr("Bidding"),tr("Points")};
    listWidget->setHorizontalHeaderLabels(header);
    // set name column
    QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg((2)*(2)));
    newItem->setText(localUser->getName());
    newItem->setData(1,localUser->getId());
    QBrush brush;
    brush.setColor(localUser->getColor());
    newItem->setForeground(brush);
    listWidget->setItem(0, 0, newItem);
    // set bidding column
    QTableWidgetItem *newItem2 = new QTableWidgetItem(tr("%1").arg((2)*(3)));
    newItem2->setText(QString::number(userBidding));
    newItem2->setData(1,localUser->getId());
    listWidget->setItem(0, 1, newItem2);
    // set point column
    QTableWidgetItem *newItem3 = new QTableWidgetItem(tr("%1").arg((2)*(4)));
    newItem3->setText(QString::number(0));
    newItem3->setData(1,localUser->getId());
    listWidget->setItem(0, 2, newItem3);

}

void OnlineLeaderboardWidget::setLocalUser(User *u)
{
    localUser = u;
}

void OnlineLeaderboardWidget::btnPressed()
{
    biddingBox->setMaximum(userBidding = biddingBox->value());
    bidBtn->setText("Bid: "+QString::number(userBidding));
    qDebug()<<"Player changed their bidding to: "<<userBidding;
    emit biddingAccepted(localUser->getId(), userBidding);
}

void OnlineLeaderboardWidget::updateBidding(QUuid id, int bidding)
{
    qDebug()<<"useronlinewidget updateBidding to "<<bidding;
    User * tmpUser = findUser(id);
    QString name = "";
    if (tmpUser != nullptr)
    {
        name = tmpUser->getName();
        for(QTableWidgetItem *item : listWidget->findItems(name,0))
        {
            if(item->data(1) == id && item->text() == name)
            {
                listWidget->item(item->row(),item->column()+1)->setText(QString::number(bidding));
                return;
            }
        }
    }
    // userBidding = bidding; notwendig?
    // emit biddingChangedOnline(userBidding);
}

void OnlineLeaderboardWidget::updateName(QUuid id, QString newName)
{
    qDebug()<<"useronlinewidget updateName to "<<newName;
    User * tmpUser = findUser(id);
    QString name = "";
    if (tmpUser != nullptr)
    {
        name = tmpUser->getName();
        for(QTableWidgetItem *item : listWidget->findItems(name,0))
        {
            if(item->data(1) == id && item->text() == name)
            {
                listWidget->item(item->row(),item->column())->setText(newName);
                return;
            }
        }
    }
    if (localUser->getId() == id)
    {
        localUser->setName(newName);
        lname->setText(newName);
    }
    lay->update();
}

void OnlineLeaderboardWidget::updateColour(QUuid id, QColor color)
{
    qDebug()<<"useronlinewidget updateColor to "<<color.name();
    User * tmpUser = findUser(id);
    QString name = "";
    if (tmpUser != nullptr)
    {
        name = tmpUser->getName();
        for(QTableWidgetItem *item : listWidget->findItems(name,0))
        {
            if(item->data(1) == id && item->text() == name)
            {
                QBrush brush;
                brush.setColor(color);
                listWidget->item(item->row(),item->column())->setForeground(brush);;
                return;
            }
        }
    }
    if (localUser->getId() == id)
    {
        localUser->setColor(color);
        lname->setStyleSheet("color: "+color.name());
    }
    lay->update();
}

void OnlineLeaderboardWidget::addUser(User *u)
{
    users.append(u);
}

void OnlineLeaderboardWidget::deactivateInput()
{

}

void OnlineLeaderboardWidget::activateInput()
{
	qDebug()<<"Schäm dich annalena";
}

void OnlineLeaderboardWidget::updateAllUsers()
{

}

User * OnlineLeaderboardWidget::findUser(QUuid id)
{
    for(User *u : users)
    {
        if(u->getId() == id)
        {
            return u;
        }
    }
    return nullptr; //bessere Lösung
}
