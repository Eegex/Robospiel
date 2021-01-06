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

void LeaderBoardWidget::updateLayout(){
    for(unsigned int i = 0; i<numOfPlayers; i++){lay->addWidget(users.at(i), i, 0);}
    if(!isOnline){lay->addWidget(addBtn, numOfPlayers, 0);} //New Player Button under all Players
    lay->update();
}
void LeaderBoardWidget::sortByBidding()
{
    qDebug()<<"Called sortByBidding";
    QVector<UserBiddingWidget*> sortedUsers;
    UserBiddingWidget * minWidget;
    int minIndex = 0;
    int minBid;
    for(int additionIndex = 0; additionIndex < users.size(); additionIndex++){
        minBid = MAX_BID;
        for(UserBiddingWidget * user : users){
            qDebug()<<"SortByBidding: USER "<<user->getName()<<" with bidding "<<user->getBidding();
            if(user->getBidding() <= minBid && user->active){//If User has a lower bid than the currently lowest bid
                qDebug()<<"Bidding of user "<<user->getName()<<" with bidding "<<user->getBidding()<<" is smaller than minimum bid of "<<minBid;
                minWidget = user; //Set the Widget to add to the new list to the user
                minIndex = users.indexOf(user); //Set the index needed for deactivating the user to the current index
                minBid = user->getBidding(); //Set the newest lowest bid to the current user as there can be users after that one with lower bids
            }
        }
        users[minIndex]->active = false; //Deactivate user
        sortedUsers.append(minWidget);
    }
    users = sortedUsers;
    for(int i = 0; i<users.size(); i++){
        qDebug()<<"SortedUsers: User "<<i<<": "<<users[i]->getName()<<" with bidding: "<<users[i]->getBidding();
        users[i]->active = true;
    }
    updateLayout();
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
    updateLayout();
    /*for(unsigned int i = 0; i<numOfPlayers; i++){lay->addWidget(users.at(i), i, 0);}
    if(!isOnline){lay->addWidget(addBtn, numOfPlayers, 0);} //New Player Button under all Players
    lay->update();*/
}

void LeaderBoardWidget::newPlayer()
{
    qDebug()<<"NewPlayer in LeaderBoardWidget";
    userCreationWidget->show(); //Show creation widget
    userCreationWidget->setFocus(); //Set focus to specific window
}

QVector<UserBiddingWidget*>* LeaderBoardWidget::getUsers(){return &users;}
UserCreationWidget *LeaderBoardWidget::getUserCreationWidget(){return userCreationWidget;}
