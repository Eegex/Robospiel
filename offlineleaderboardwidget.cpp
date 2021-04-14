#include <QDebug>
#include "offlineleaderboardwidget.h"
#include "UserBiddingWidget.h"
#include "UserCreationWidget.h"
#include "user.h"

OfflineLeaderBoardWidget::OfflineLeaderBoardWidget(QWidget *parent) : QWidget(parent)
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

void OfflineLeaderBoardWidget::sortBy(unsigned int strategy) //This is being called after the countdown has done down to 0, you cannot add a new player in this phase
{
	QVector<UserBiddingWidget*> sortedUsers;
	unsigned long minTimeStamp = QDateTime::currentMSecsSinceEpoch();
	bool isActive[numOfUsers];
	for(unsigned int i = 0; i<numOfUsers; i++)
	{
		isActive[i] = 1; //Set all users to be active
	}
	userCreationWidget->hide(); //This is just for convenience
	if(strategy == bid)
	{
		//qDebug()<<"Called sortByBidding";
		UserBiddingWidget * minWidget;
		int minIndex = 0;
		int minBid;
		for(int additionIndex = 0; additionIndex < users.size(); additionIndex++)
		{
			minBid = MAX_BID + 1;
			for(UserBiddingWidget * user : users)
			{
				//qDebug()<<"SortByBidding: USER "<<user->getName()<<" with bidding "<<user->getBidding()<<"and timestamp: "<<user->getTimeStamp();
				if(user->getBidding() <= minBid && isActive[users.indexOf(user)])//If User has a lower bid than the currently lowest bid
				{
					if(user->getBidding() == minBid)
					{
						if(user->getTimeStamp() < minTimeStamp) //check the timestamp
						{
							//qDebug()<<"Bidding is the same, timestamp is earlier";
							minWidget = user; //Set the Widget to add to the new list to the user
							minIndex = users.indexOf(user); //Set the index needed for deactivating the user to the current index
							minBid = user->getBidding(); //Set the newest lowest bid to the current user as there can be users after that one with lower bids
							minTimeStamp = user->getTimeStamp(); //Set User Timestamp to the current user value
						}
					}
					else
					{
						//qDebug()<<"Bidding of user "<<user->getName()<<" with bidding "<<user->getBidding()<<" is smaller than minimum bid of "<<minBid;
						minWidget = user; //Set the Widget to add to the new list to the user
						minIndex = users.indexOf(user); //Set the index needed for deactivating the user to the current index
						minBid = user->getBidding(); //Set the newest lowest bid to the current user as there can be users after that one with lower bids
						minTimeStamp = user->getTimeStamp(); //Set User Timestamp to the current user value
					}
				}
			}
			isActive[minIndex] = false; //Deactivate user
			sortedUsers.append(minWidget);
		}
		users = sortedUsers;
	}
	if(strategy == points)
	{
		//qDebug()<<"Called sortByPoints";
		UserBiddingWidget * maxWidget;
		int maxIndex = 0;
		int maxPts;
		for(int additionIndex = 0; additionIndex < users.size(); additionIndex++)
		{
			maxPts = 0;
			for(UserBiddingWidget * user : qAsConst(users))
			{
				//qDebug()<<"SortByPoints: USER "<<user->getName()<<" with points "<<user->getPoints()<<"and timestamp: "<<user->getTimeStamp();
				if(user->getPoints() >= maxPts && isActive[users.indexOf(user)])//If User has more points than the current maximum
				{
					//qDebug()<<"Points of user "<<user->getName()<<" with points "<<user->getPoints()<<" is larger than maximum amount of "<<maxPts;
					maxWidget = user; //Set the Widget to add to the new list to the user
					maxIndex = users.indexOf(user); //Set the index needed for deactivating the user to the current index
					maxPts = user->getPoints(); //Set the newest largest number of points to the current user as there can be users after that one with larger number of points
					minTimeStamp = user->getTimeStamp(); //Set the User Time stamp to the current user value
				}
			}
			isActive[maxIndex] = false; //Deactivate user
			sortedUsers.append(maxWidget);
		}
		users = sortedUsers;
		for(int i = 0; i<users.size(); i++)
		{
			//qDebug()<<"SortedUsers: User "<<i<<": "<<users[i]->getName()<<" with points: "<<users[i]->getPoints()<<" and timestamp "<<users[i]->getTimeStamp();
			//isActive[i] = true;
		}
	}
	updateLayout();
}

void OfflineLeaderBoardWidget::addUser(User * newUser)
{
	qDebug()<<"LeaderBoardWidget: AddUser: Add user with name: "<<newUser->getName()<<"and id"<<newUser->getId().toString()<<"and colour "<<(newUser->getColor().isValid()?newUser->getColor().name():"0x000000");
	UserBiddingWidget * newWidget = new UserBiddingWidget(this); //Create new BiddingWidget to display
	newWidget->setName(newUser->getName());
	newWidget->setColor(newUser->getColor().name());
	newWidget->setId(newUser->getId());
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