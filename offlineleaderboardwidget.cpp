#include <QDebug>
#include <QUuid>
#include "offlineleaderboardwidget.h"
#include "UserBiddingWidget.h"
#include "UserCreationWidget.h"
#include "user.h"

OfflineLeaderBoardWidget::OfflineLeaderBoardWidget()
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
	connect(newWidget,&UserBiddingWidget::biddingChanged,this,[&](int bid, unsigned long time, QUuid id){
		emit biddingAccepted(id, bid);
	});
	connect(newWidget->getUser(), &User::biddingChanged, this, &OfflineLeaderBoardWidget::updateBidding);
	updateLayout();
}

void OfflineLeaderBoardWidget::updateBidding(QUuid id, int bidding)
{
	qDebug()<<"Called Function UpdateBidding in Offline LeaderBoard Widget, update points to "<<bidding;
	for(UserBiddingWidget* ubw : users)
	{
		if(ubw->getId() == id)
		{
			ubw->updateBidding(bidding);
		}
	}
}

void OfflineLeaderBoardWidget::updateName(QUuid id, QString name)
{
	for(UserBiddingWidget* ubw : users)
	{
		if(ubw->getId() == id)
		{
			ubw->updateName(name);
		}
	}
}

void OfflineLeaderBoardWidget::updateColour(QUuid id, QColor colour)
{
	for(UserBiddingWidget* ubw : users)
	{
		if(ubw->getId() == id)
		{
			ubw->updateColour(colour);
		}
	}
}

void OfflineLeaderBoardWidget::deactivateInput()
{
	for(UserBiddingWidget* ubw : users)
		ubw->deactivateBtn();
}

void OfflineLeaderBoardWidget::activateInput()
{
	for(UserBiddingWidget* ubw : users)
		ubw->activateBtn();
}

void OfflineLeaderBoardWidget::updateAllUsers()
{
	qDebug()<<"Called Function UpdateAllUsers in OfflineLeaderBoardWidget\n";
	for(UserBiddingWidget* ubw : users)
	{
		updateBidding(ubw->getUser()->getId(), ubw->getUser()->getBidding());
		updateName(ubw->getUser()->getId(), ubw->getUser()->getName());
		updateColour(ubw->getUser()->getId(), ubw->getUser()->getColor());
	}
	QVector<UserBiddingWidget*> temp;
	for(User* u : *GameControll::getInstance().getUsers())
	{
		for(UserBiddingWidget* ubw : users)
		{
			if(u->getId() == ubw->getId())
			{
				temp.append(ubw);
			}
		}
	}
	qDebug()<<"Sorted Users!\n";
	for(unsigned int i = 0; i<numOfUsers; i++)
	{
		qDebug()<<"Position: "<<i<<" "<<users[i]->getUser()->getName();
	}
	users = temp;
	updateLayout();
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
