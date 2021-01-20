#include <QDebug>
#include "LeaderBoardWidget.h"
#include "UserBiddingWidget.h"
#include "UserCreationWidget.h"
#include "user.h"

LeaderBoardWidget::LeaderBoardWidget(QWidget *parent) : QWidget(parent)
{
	userCreationWidget->hide();
	userOnlineWidget->hide();
	lay->addWidget(userCreationWidget, numOfUsers, 0);
	// lay->addWidget(addBtn, numOfUsers, 0);
	// addBtn->hide();
	connect(networkView, &NetworkView::leaderboradOnline, this, &LeaderBoardWidget::goOnline);
	connect(networkView, &NetworkView::leaderboradOffline, this, &LeaderBoardWidget::goOffline);
	if(isOnline == undecided)
		lay->addWidget(networkView);
	// connect(addBtn,&QPushButton::clicked,this,&LeaderBoardWidget::newUser);
	setLayout(lay);
	//connect(userCreationWidget, &UserCreationWidget::userAdded, this, &LeaderBoardWidget::addUser);
}

unsigned int LeaderBoardWidget::getBiddingWidgetIndexByID(QUuid id){
	qDebug()<<"Called Function getBiddingWidgetByID in LeaderBoardWidget with ID "<<id;
	for(unsigned int i = 0; i<numOfUsers; i++)
		if(users[i]->getId() == id)
			return i;
	qDebug()<<"Couldn't find user with ID "<<id;
	return 0;
}

void LeaderBoardWidget::updateLayout(){
	qDebug()<<"Called UpdateLayout, number of Users is "<<numOfUsers<<", isOnline is "<<isOnline;
	for(unsigned int i = 0; i<numOfUsers; i++){lay->addWidget(users.at(i), i, 0);}
	// if(isOnline == offline){lay->addWidget(addBtn, numOfUsers, 0);} //New Player Button under all Players
	if(isOnline == offline){
		lay->addWidget(userCreationWidget, numOfUsers, 0);
		if(numOfUsers == MAX_USERS)
			userCreationWidget->hide();
	} //New Player Button under all Players except when maximum Number of Players is reached
	lay->update();
}

void LeaderBoardWidget::sortByBidding() //This is being called after the countdown has done down to 0, you cannot add a new player in this phase
{
	userCreationWidget->hide(); //This is just for convenience
	qDebug()<<"Called sortByBidding";
	QVector<UserBiddingWidget*> sortedUsers;
	UserBiddingWidget * minWidget;
	int minIndex = 0;
	int minBid;
	for(int additionIndex = 0; additionIndex < users.size(); additionIndex++)
	{
		minBid = MAX_BID;
		for(UserBiddingWidget * user : users)
		{
			qDebug()<<"SortByBidding: USER "<<user->getName()<<" with bidding "<<user->getBidding();
			if(user->getBidding() <= minBid && user->active)//If User has a lower bid than the currently lowest bid
			{
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
	for(int i = 0; i<users.size(); i++)
	{
		qDebug()<<"SortedUsers: User "<<i<<": "<<users[i]->getName()<<" with bidding: "<<users[i]->getBidding();
		users[i]->active = true;
	}
	updateLayout();
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
	connect(newWidget, &UserBiddingWidget::biddingReset, this, [&](){
		if(isOnline == offline && numOfUsers < MAX_USERS)
			userCreationWidget->show(); //Bidding has been reset at this point so player addition button is back baby
	});
	numOfUsers++; //Increment number of users, important for correct placement of button
	updateLayout();
}

void LeaderBoardWidget::newUser()
{
	qDebug()<<"NewUser in LeaderBoardWidget";
	userCreationWidget->show(); //Show creation widget
	userCreationWidget->setFocus(); //Set focus to specific window
}

void LeaderBoardWidget::goOnline()
{
	//TODO if online as client: send your user
	isOnline = online;
	networkView->hide();
	lay->addWidget(userOnlineWidget);
	userOnlineWidget->show();
	lay->update();
}

void LeaderBoardWidget::goOffline()
{
	qDebug()<<"Called Function goOffline";
	isOnline = offline;
	networkView->hide();
	userCreationWidget->show();
	// addBtn->setText("Add new User");
	// addBtn->show();
	lay->update();
}

void LeaderBoardWidget::goUndefined()
{
	isOnline = undecided;
	if(isOnline == online)
		lay->removeWidget(userOnlineWidget);
	else if(isOnline == offline) {
		for(int i = 0; i < lay->count(); i++)
		{
			QWidget * tmp = lay->itemAt(i)->widget();
			if(tmp)
				tmp->hide();
		}
	}
	lay->addWidget(networkView);
	lay->update();
}

void LeaderBoardWidget::setUsername(QString name)
{
	username = name;
	userOnlineWidget->updateName(username);
}

void LeaderBoardWidget::setUsercolor(QColor color){usercolor = color;}
unsigned int LeaderBoardWidget::getNumOfUsers(){return numOfUsers;}
unsigned short LeaderBoardWidget::getIsOnline(){return isOnline;}
QVector<UserBiddingWidget*>* LeaderBoardWidget::getUsers(){return &users;}
UserCreationWidget *LeaderBoardWidget::getUserCreationWidget(){return userCreationWidget;}
UserOnlineWidget *LeaderBoardWidget::getUserOnlineWidget(){return userOnlineWidget;}
