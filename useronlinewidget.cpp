#include "useronlinewidget.h"
#include <QDebug>

UserOnlineWidget::UserOnlineWidget(QWidget *parent) : QWidget(parent)
{
    bidBtn->hide();
    nameField->hide();
    name->hide();
    biddingBox->hide();
    listWidget->hide();
    lay->addWidget(nameField);
    lay->addWidget(addBtn);
    lay->addWidget(listWidget);
    nameField->show();
    listWidget->show();
    addBtn->show();nameField->setPlaceholderText("Enter your name");
    addBtn->setText("Save");
    setLayout(lay);

    connect(addBtn,&QPushButton::clicked,this,&UserOnlineWidget::setUser);
    connect(bidBtn,&QPushButton::clicked, this, &UserOnlineWidget::btnPressed);
}
void UserOnlineWidget::setUser()
{
    qDebug()<<"SetUser in UserOnline";
    struct UserData newUser;
    newUser.name = nameField->text();
    // TODO: colour picking
    newUser.colour = QColorConstants::Black;
    emit userAdded(&newUser);
    addBtn->hide();
    nameField->hide();
    name->setText(newUser.name);
    lay->addWidget(name);
    name->show();
    setBidding();
    lay->update();
    // TODO: load & show other users with rank and current bidding
}

void UserOnlineWidget::setBidding()
{
    biddingBox->setMinimum(1);
    biddingBox->setSingleStep(1);
    bidBtn->setText("Accept Bid");
    lay->addWidget(biddingBox);
    lay->addWidget(bidBtn);
    biddingBox->show();
    bidBtn->show();
    lay->update();
}

void UserOnlineWidget::btnPressed()
{
    biddingBox->setMaximum(userBidding = biddingBox->value());
    bidBtn->setText("Bid: "+QString::number(userBidding));
    qDebug()<<"Player changed their bidding to: "<<userBidding;
    emit biddingChangedOnline(userBidding);
}

/* void UserOnlineWidget::loadUsers()
{

} */
