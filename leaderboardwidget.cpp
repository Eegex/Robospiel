#include <QDebug>
#include "LeaderBoardWidget.h"
#include "UserBiddingWidget.h"
#include "UserCreationWidget.h"
#include "user.h"

LeaderBoardWidget::LeaderBoardWidget(QWidget *parent) : QWidget(parent)
{
    userCreationWidget->hide();
    userOnlineWidget->hide();
    addBtn->hide();
    connect(networkView, &NetworkView::leaderboradOnline, this, &LeaderBoardWidget::goOnline);
    if(isOnline == 0)
    {
        lay->addWidget(networkView);
    }
    connect(addBtn,&QPushButton::clicked,this,&LeaderBoardWidget::newUser);
    setLayout(lay);
    //connect(userCreationWidget, &UserCreationWidget::userAdded, this, &LeaderBoardWidget::addUser);
}

void LeaderBoardWidget::sortByBidding()
{
    QVector<UserBiddingWidget*> sortedUsers;
    UserBiddingWidget * minWidget;
    int minBid = 999;
    for(int additionIndex = 0; additionIndex < users.size(); additionIndex++){ //Effectively insertion sort
        for(UserBiddingWidget * user : users)
            if(user->getBidding()<minBid)
                minWidget = user;
        sortedUsers[additionIndex] = minWidget;
        users.remove(users.indexOf(minWidget));
    }
    users = sortedUsers;
}

void LeaderBoardWidget::addUser(User * newUser)
{
    qDebug()<<"LeaderBoardWidget: AddUser: Add user with name: "<<newUser->getName()<<"and id"<<newUser->getId().toString()<<"and colour "<<(newUser->getColor().isValid()?newUser->getColor().name():"0x000000");
    UserBiddingWidget * newWidget = new UserBiddingWidget(this); //Create new BiddingWidget to display
    newWidget->setName(newUser->getName());
    newWidget->setColor(newUser->getColor().name());
    newWidget->setId(newUser->getId());
    users.append(newWidget); //Append widget to list of users
    lay->addWidget(users.at(numOfUsers));
    numOfUsers++; //Increment number of users, important for correct placement of button
    for(unsigned int i = 0; i<numOfUsers; i++){lay->addWidget(users.at(i), i, 0);}
    if(!isOnline){lay->addWidget(addBtn, numOfUsers, 0);} //New User Button under all Users
    lay->update();
}

void LeaderBoardWidget::newUser()
{
    qDebug()<<"NewUser in LeaderBoardWidget";
    userCreationWidget->show(); //Show creation widget
    userCreationWidget->setFocus(); //Set focus to specific window
}

void LeaderBoardWidget::goOnline()
{
    isOnline = 2;
    networkView->hide();
    lay->addWidget(userOnlineWidget);
    userOnlineWidget->show();
    lay->update();
}

void LeaderBoardWidget::goOffline()
{
    isOnline = 1;
    networkView->hide();
    addBtn->setText("Add new User");
    lay->addWidget(addBtn, numOfUsers, 0);
    addBtn->show();
    lay->update();
}

void LeaderBoardWidget::goUndefined()
{
    isOnline = 0;
    if(isOnline == 2) {
        lay->removeWidget(userOnlineWidget);
    } else if(isOnline == 1) {
        for(int i=0; i < lay->count(); i++)
        {
            QWidget * tmp = lay->itemAt(i)->widget();
            if(tmp)
            {
                tmp->hide();
            }
        }
    }
    lay->addWidget(networkView);
    lay->update();
}

unsigned short LeaderBoardWidget::getIsOnline(){return isOnline;}
QVector<UserBiddingWidget*>* LeaderBoardWidget::getUsers(){return &users;}
UserCreationWidget *LeaderBoardWidget::getUserCreationWidget(){return userCreationWidget;}
UserOnlineWidget *LeaderBoardWidget::getUserOnlineWidget(){return userOnlineWidget;}
