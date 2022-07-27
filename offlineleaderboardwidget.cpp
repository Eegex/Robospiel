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

/*!
 * \brief OfflineLeaderBoardWidget::getBiddingWidgetIndexByID
 * \param id
 * \return
 * This function returns the List Index of the BiddingWidget with the given ID
 * This is needed because the List of Pointers to BiddingWidgets might not reflect the actual ordering
 */
unsigned int OfflineLeaderBoardWidget::getBiddingWidgetIndexByID(QUuid id){
	for(unsigned int i = 0; i<numOfUsers; i++)
	{
		if(users[i]->getId() == id)
		{
			return i;
		}
	}
	qDebug() << "Couldn't find user with ID "<< id << " in OfflineLeaderBoardWidget::getBiddingWidgetIndexByID";
	return 0;
}

/*!
 * \brief OfflineLeaderBoardWidget::updateLayout
 * This function is just there to update the visual layout
 */
void OfflineLeaderBoardWidget::updateLayout(){
	for(unsigned int i = 0; i < numOfUsers; i++)
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

/*!
 * @brief OfflineLeaderBoardWidget::setBiddingFocus
 * is not implemented in the offline leaderboardwidget,
 * because different input fields (for different users) need to have the focus and it has to be shifted manually by the users.
 */
void OfflineLeaderBoardWidget::setBiddingFocus()
{
	// not implemented
}

/*!
 * \brief OfflineLeaderBoardWidget::addUser
 * \param newUser
 * This function adds a new UserBiddingWidget to the list of user bidding widgets
 * This is needed to properly display the list of Users
 */
void OfflineLeaderBoardWidget::addUser(User * newUser)
{
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
	connect(newWidget->getUser(), &User::pointsChanged, this, &OfflineLeaderBoardWidget::updateAllUsers);
	updateLayout();
}

/*!
 * \brief OfflineLeaderBoardWidget::updateBidding
 * \param id
 * \param bidding
 * This function updates the bidding of the user with the provided ID
 */
void OfflineLeaderBoardWidget::updateBidding(QUuid id, int bidding)
{
	for(UserBiddingWidget* ubw : users)
	{
		if(ubw->getId() == id)
		{
			//If User hasn't bid, show a different number, this prevents the thing from looking like garbage
			if(ubw->getUser()->getHasBid())
				ubw->updateBidding(bidding);
			else
				ubw->updateBidding(-1);
		}
	}
}

void OfflineLeaderBoardWidget::newRound(){
	for(UserBiddingWidget* ubw : users)
	{
			ubw->updateBidding(0);
	}
}

/*!
 * \brief OfflineLeaderBoardWidget::updateName
 * \param id
 * \param name
 * This function updates the name of the user with the provided ID
 */
void OfflineLeaderBoardWidget::updateName(QUuid id, QString name)
{
	for(UserBiddingWidget * ubw : users)
	{
		if(ubw->getId() == id)
		{
			ubw->updateName(name);
		}
	}
}

void OfflineLeaderBoardWidget::updatePlayerInPower(QUuid id, QString name)
{
    //TODO: no idea what should happen here, input would be appreciated!
}
void OfflineLeaderBoardWidget::updateServerName(QUuid id, QString name)
{
    //TODO: no idea what should happen here, input would be appreciated!
}
void OfflineLeaderBoardWidget::noPlayerInPower()
{
    //TODO: no idea what should happen here, input would be appreciated!
}

void OfflineLeaderBoardWidget::updateColour(QUuid id, QColor colour)
{
	for(UserBiddingWidget * ubw : users)
	{
		if(ubw->getId() == id)
		{
			ubw->updateColour(colour);
		}
	}
}

void OfflineLeaderBoardWidget::deactivateInput()
{
	for(UserBiddingWidget * ubw : users)
		ubw->deactivateBtn();
}

void OfflineLeaderBoardWidget::activateInput()
{
	for(UserBiddingWidget* ubw : users)
		ubw->activateBtn();
}

/*!
 * \brief OfflineLeaderBoardWidget::updateAllUsers
 * This function updates the visual Data of all users, it does this by first pulling all the necessary information from the back end and then sorting them
 * according to their placement in the user list in the back end since the sorting function (e.g. by points or bidding) happens in the back end
 */
void OfflineLeaderBoardWidget::updateAllUsers()
{
	for(UserBiddingWidget* ubw : users)
	{
		updateBidding(ubw->getUser()->getId(), ubw->getUser()->getBidding());
		updateName(ubw->getUser()->getId(), ubw->getUser()->getName());
		updateColour(ubw->getUser()->getId(), ubw->getUser()->getColor());
	}
	QVector<UserBiddingWidget*> temp;
	//This creates the sorted view that we want
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


void OfflineLeaderBoardWidget::setFreeplayButtonsVisible(bool visible)
{
}
