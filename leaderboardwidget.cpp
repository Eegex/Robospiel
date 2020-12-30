#include <QDebug>
#include "LeaderBoardWidget.h"
#include "UserBiddingWidget.h"
#include "UserCreationWidget.h"
#include "user.h"

LeaderBoardWidget::LeaderBoardWidget(QWidget *parent) : QWidget(parent)
{
    userCreationWidget->hide();
    addBtn->hide();
    if(!isOnline)
    {
        addBtn->setText("Add new Player");
        lay->addWidget(addBtn, numOfPlayers, 0);
        addBtn->show();
        connect(addBtn,&QPushButton::clicked,this,&LeaderBoardWidget::newPlayer);
    }
    setLayout(lay);
    //connect(userCreationWidget, &UserCreationWidget::playerAdded, this, &LeaderBoardWidget::addPlayer);
}

void LeaderBoardWidget::sortByBidding()
{
    //TODO: Will be done in back end
}

void LeaderBoardWidget::addPlayer(User * newUser)
{
    qDebug()<<"LeaderBoardWidget: AddPlayer: Add player with name: "<<newUser->getName()<<"and id"<<newUser->getId().toString()<<"and colour "<<(newUser->getColor().isValid()?newUser->getColor().name():"0x000000");
    UserBiddingWidget * newWidget = new UserBiddingWidget(this); //Create new BiddingWidget to display
    newWidget->setName(newUser->getName());
    newWidget->setColor(newUser->getColor().name());
    newWidget->setId(newUser->getId());
    users.append(newWidget); //Append widget to list of players
    lay->addWidget(users.at(numOfPlayers));
    numOfPlayers++; //Increment number of players, important for correct placement of button
    for(unsigned int i = 0; i<numOfPlayers; i++){lay->addWidget(users.at(i), i, 0);}
    if(!isOnline){lay->addWidget(addBtn, numOfPlayers, 0);} //New Player Button under all Players
    lay->update();
}

void LeaderBoardWidget::newPlayer()
{
    qDebug()<<"NewPlayer in LeaderBoardWidget";
    userCreationWidget->show(); //Show creation widget
    userCreationWidget->setFocus(); //Set focus to specific window
}

QVector<UserBiddingWidget*>* LeaderBoardWidget::getUsers(){return &users;}
UserCreationWidget *LeaderBoardWidget::getUserCreationWidget(){return userCreationWidget;}
