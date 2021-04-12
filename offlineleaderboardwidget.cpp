#include <QDebug>
#include <QUuid>
#include "offlineleaderboardwidget.h"
#include "UserBiddingWidget.h"
#include "UserCreationWidget.h"
#include "user.h"

OfflineLeaderBoardWidget::OfflineLeaderBoardWidget(LeaderBoardWidget *parent) : LeaderBoardWidget(parent)
{
	lay->addWidget(userCreationWidget, numOfUsers, 0); //No need to hide anymore
	setLayout(lay);
}

unsigned int OfflineLeaderBoardWidget::getBiddingWidgetIndexByID(QUuid id){
	qDebug()<<"Called Function getBiddingWidgetByID in LeaderBoardWidget with ID "<<id;
	for(unsigned int i = 0; i<numOfUsers; i++)
	{
		if(users[i]->getId() == id)
		{
			return i;
		}
	}
	qDebug()<<"Couldn't find user with ID "<<id;
	return 0;
}

void OfflineLeaderBoardWidget::updateLayout(){
	qDebug()<<"Called UpdateLayout, number of Users is "<<numOfUsers<<", running in offline mode!";
	for(unsigned int i = 0; i<numOfUsers; i++)
	{
		lay->addWidget(users.at(i), i, 0);
	}
	lay->addWidget(userCreationWidget, numOfUsers, 0);
	if(numOfUsers == MAX_USERS)
	{
		userCreationWidget->hide();
	}
	//New Player Button under all Players except when maximum Number of Players is reached
	lay->update();
}

void OfflineLeaderBoardWidget::addUser(User * newUser)
{
	qDebug()<<"LeaderBoardWidget: AddUser: Add user with name: "<<newUser->getName()<<"and id"<<newUser->getId().toString()<<"and colour "<<(newUser->getColor().isValid()?newUser->getColor().name():"0x000000");
    UserBiddingWidget * newWidget = new UserBiddingWidget(newUser); //Create new BiddingWidget to display
	users.append(newWidget); //Append widget to list of users
	lay->addWidget(users.at(numOfUsers));
	connect(newWidget, &UserBiddingWidget::biddingReset, this,[&]()
	{
		if(numOfUsers < MAX_USERS)
		{
			userCreationWidget->show(); //Bidding has been reset at this point so player addition button is back baby
		}
	});
	numOfUsers++; //Increment number of users, important for correct placement of button
	updateLayout();
}

void OfflineLeaderBoardWidget::updateBidding(QUuid id, int bidding){
    for(UserBiddingWidget* ubw : users){
        if(ubw->getId() == id)
            ubw->updateBidding(bidding);
    }
}

unsigned int OfflineLeaderBoardWidget::getNumOfUsers()
{
	return numOfUsers;
}

QVector<UserBiddingWidget*>* OfflineLeaderBoardWidget::getUsers()
{
	return &users;
}

UserCreationWidget *OfflineLeaderBoardWidget::getUserCreationWidget()
{
	return userCreationWidget;
}
