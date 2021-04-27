#include "gamecontroll.h"

#include "server.h"
#include "client.h"
#include "user.h"
#include <QJsonObject>
#include <QUuid>
#include <QUuid>

using namespace std::chrono_literals;
GameControll GameControll::instance;

GameControll& GameControll::getInstance()
{
	return instance;
}

GameControll::GameControll(QObject *parent) : QObject(parent)
{
	countdown.setSingleShot(false);
	countdown.setInterval(1s);
	connect(&countdown,&QTimer::timeout,this,&GameControll::updateTimer);
	connect(this, &GameControll::actionTriggeredWithData, this, &GameControll::sendToServerWithData);
	connect(this, &GameControll::actionTriggered, this, &GameControll::sendToServer);
}

//This relies on Server and Client singletons.
//Initialization of static variables can happen in random order, so this has to wait, until all static variables are present.
void GameControll::initializeConnections()
{
	connect(&Server::getInstance(), &Server::actionReceived, &GameControll::getInstance(), &GameControll::exeQTAction);
	connect(&Client::getInstance(), &Client::actionReceived, &GameControll::getInstance(), &GameControll::exeQTAction);
}

QJsonObject GameControll::toJSON() //TODO test both ways of JSON
{
	QJsonObject json = QJsonObject();
	json.insert("currentPhase", static_cast<int>(instance.currentPhase));
	json.insert("board", instance.board->toJSON());
	json.insert("activeUserID", instance.activeUserID.toString());
	json.insert("remainingTimerTime", instance.countdown.remainingTime());
	json.insert("timeLeft", instance.timeLeft);
	QJsonArray jsonUsers;
	for(User * user : qAsConst(instance.users))
	{
		jsonUsers.append(user->toJSON());
	}
	json.insert("users", jsonUsers);
	return json;
}

void GameControll::adaptFromJSON(QJsonObject json)
{
	instance.currentPhase=static_cast<Phase>(json.value("currentPhase").toInt());
	setBoard(Board::fromJSON(json.value("board").toObject()));
	instance.setActiveUserID(QUuid::fromString(json.value("activeUserID").toString()));
	instance.timeLeft = json.value("timeLeft").toInt();
	instance.countdown.stop();
	if(json.value("remainingTimerTime").toInt()!=-1)
	{
		QTimer::singleShot(json.value("remainingTimerTime").toInt(), &instance, [=]()->void{
			instance.countdown.start();
		});

	}

	instance.users.clear();
	QJsonArray jsonUsers = json.value("users").toArray();
	for(int i=0; i<jsonUsers.size(); i++)
	{
		QJsonObject jsonUser = jsonUsers.at(i).toObject();
        User* user = User::fromJSON(jsonUser);
        instance.leaderboard->addUser(user);
	}
}

//forwards the message to all clients / triggers the action directly when you are offline
void GameControll::sendToServerWithData(PlayerAction a, QJsonObject info)
{
	qDebug()<<"Send to server with data"<<a<<info;
	info.insert("action", a);
	if(Server::getInstance().isActive())
	{
		//you are the server
		Server::getInstance().sendMessageToClients(info);
	}
	else if (Client::getInstance().isActive())
	{
		//you are the client
		Client::getInstance().sendMessageToServer(info);
	}
	else
	{
		//you are offline
		exeQTAction(info);
	}
}

//forwards the message to all clients / triggers the action directly when you are offline
void GameControll::sendToServer(PlayerAction a)
{
	qDebug()<<"Send to server"<<a;
	QJsonObject json;
	json.insert("action", a);
	if(Server::getInstance().isActive())
	{
		//you are the server
		Server::getInstance().sendMessageToClients(json);
	}
	else if (Client::getInstance().isActive())
	{
		//you are the client
		Client::getInstance().sendMessageToServer(json);
	}
	else
	{
		//you are offline
		exeQTAction(json);
	}
}

void GameControll::load()
{
	instance.settings = new SettingsDialog(instance.mapping);
	instance.settings->load();
	connect(instance.settings, &SettingsDialog::colorsChanged, &GameControll::getInstance(),[&]()
	{
		instance.board->updateColors(instance.settings->getBackground(), instance.settings->getWallcolor(), instance.settings->getGridcolor(), instance.settings->getPlayerColorLow(), instance.settings->getPlayerColorHigh());
	});
	connect(instance.settings, &SettingsDialog::newMapping, &GameControll::getInstance(),[&](QVector<KeyMapping*> mapping){ instance.mapping = mapping; });
	if(instance.board)
	{
		instance.board->updateColors(instance.settings->getBackground(), instance.settings->getWallcolor(), instance.settings->getGridcolor(), instance.settings->getPlayerColorLow(), instance.settings->getPlayerColorHigh());
	}
	instance.mapping = instance.settings->getMapping();
}

void GameControll::showSettings()
{
	settings->exec();
}

Board * GameControll::setBoard(Board* newBoard)
{
	if(instance.board)
	{
		instance.board->deleteLater();
		instance.board = nullptr;
	}
	instance.board = newBoard;
	if(instance.settings)
	{
		instance.board->updateColors(instance.settings->getBackground(), instance.settings->getWallcolor(), instance.settings->getGridcolor(), instance.settings->getPlayerColorLow(), instance.settings->getPlayerColorHigh());
	}
	connect(instance.board, &Board::playerMoved, &GameControll::getInstance(), &GameControll::calculateGameStatus);
	return instance.board;
}

// executes different actions, because they were send by the server / triggered directly when you are offline
void GameControll::exeQTAction(QJsonObject data) //TODO maybe the bool return was needed?
{
//	qDebug()<<"execute"<<data;
	PlayerAction a = static_cast<PlayerAction>(data.take("action").toInt());
	User* user;
	UserData userData;
	switch(a)
	{
//		case none:
//			break;
//		case movement:
//			break;
//		case bidding:
//			break;
//		case enterBidding:
//			break;
//		case clearBidding:
//			break;
//		case other:
//			break;
//		case giveUp:
//			break;
//		case freePlay:
//			break;
//		case PlayerAction::user:
//			break;
//		case changedUsername:
//			break;
//		case changedUserColor:
//			break;
//		case update:
//			break;
//		case boardUpdate:
//			break;
		case movePlayerEast:
		case movePlayerNorth:
		case movePlayerSouth:
		case movePlayerWest:
			//we subtract movement from action to get a direction (clever enum numbers)
			board->moveActivePlayer(static_cast<Direction>(a - PlayerAction::movement));
			break;
		case switchPlayerEast:
		case switchPlayerNorth:
		case switchPlayerSouth:
		case switchPlayerWest:
			board->switchPlayer(static_cast<Direction>(a-PlayerAction::playerSwitch));
			break;
		case playerSwitch:
            board->changeActivePlayer(data.value("playerNumber").toInt(), data.value("isRevert").toBool());
			break;
		case sendBidding:
			switchPhase(Phase::countdown); 
            getUserById(QUuid(data.value("userId").toString()))->setBidding(data.value("bidding").toInt());
			break;
		case revert:
			board->revert();
			break;
		case revertToBeginning:
			board -> revertToBeginning();
			break;
		case newUser:
            user = User::fromJSON(data);
            addUser(user);
			break;
		case completeUpdate:
			adaptFromJSON(data);
	}
}

//
//Don't write actual functionality here! It only sends the actions to the server. Functionality is in exeQTdata()!!!
//
void GameControll::triggerAction(PlayerAction action, QUuid userID)
{
	qDebug()<<"Called function TriggerAction with parameters "<<action<<" and User ID "<<userID;
	if(action & PlayerAction::movement)
	{
		if(instance.currentPhase == Phase::presentation || instance.currentPhase == Phase::freeplay) //If online only let the active user move
		{
			emit instance.actionTriggered(action);
			return;
		}
	}
	else if(action & PlayerAction::playerSwitch && (instance.currentPhase == Phase::presentation || instance.currentPhase == Phase::freeplay)) //???
	{
		if(action != PlayerAction::playerSwitch)
		{
            QJsonObject json;
            json.insert("isRevert", false);
            qDebug()<<"playerSwitch is called in triggerAction with isRevert=False!";
            emit instance.actionTriggeredWithData(action, json);
			return;
		}
	}
	else if(action & PlayerAction::bidding) //TODO submit biddingValue
	{
		qDebug()<<"Currently in GameControl: triggerAction -> bidding, current Phase is "<<static_cast<int>(instance.currentPhase);
		if(instance.currentPhase == Phase::search || instance.currentPhase == Phase::countdown)
		{
			emit instance.actionTriggered(action);
			return;
		}
	}
	else if(action & PlayerAction::other)
	{
		if(instance.currentPhase == Phase::presentation || instance.currentPhase == Phase::freeplay)
		{
			emit instance.actionTriggered(action);
			return;
		}
	}
	return;
}

void GameControll::triggerActionsWithData(PlayerAction action, QJsonObject data)
{
	emit instance.actionTriggeredWithData(action, data);
	// Annalenas version:
	//if(action==PlayerAction::newUser)
	//{
	//	QJsonObject data = QJsonObject();
	//	data.insert("username", user->getName());
	//	data.insert("usercolor", user->getColor().name());
	//	data.insert("id", user->getId().toString());
	//	emit actionTriggeredWithData(action, data);
	//} else if (action == PlayerAction::sendBidding)
	//{
	//    qDebug()<<"Currently in GameControl: triggerActionWithData -> bidding, current Phase is "<<static_cast<int>(currentPhase);
	//    if(currentPhase == Phase::search || currentPhase == Phase::countdown)
	//    {
	//        QJsonObject data = QJsonObject();
	//        data.insert("bidding", user->getBidding());
	//        data.insert("id", user->getId().toString());
	//        emit actionTriggeredWithData(action, data);
	//    }
	//}
}

//called after each movement of a player (and when reverting, ...)
void GameControll::calculateGameStatus()
{

	qDebug()<<"Current Moves are: "<< board->getMoves()<<", User Bidding is "<<getUserById(activeUserID)->getBidding();
	/*if(board->getMoves() < getUserById(activeUserID)->getBidding())
	{*/
		if(board->goalHit) //Spieler hat gewonnen, die Runde ist zuende
		{
            actionWhenAnimationEnded = &GameControll::calculateWinner;
            qDebug()<<"actionWhenAnimationEnded = calculateWinner";
		}
	//}
    else
	{
		if(board->getMoves() >= getUserById(activeUserID)->getBidding()){
			//TODO: Flag um anzuzeigen, dass der Spieler das Ziel erreicht hat?
			qDebug()<<"User couldn't end the round in the specified bid of "<< getUserById(activeUserID)->getBidding()<<", the next user is being drawn";
			if(getNextUser(activeUserID))//Not at last player yet, noch haben nicht alle versagt
			{
                actionWhenAnimationEnded = &GameControll::resetForNextUser;
                qDebug()<<"actionWhenAnimationEnded = resetFornextuser";
			}
			else //Alles Versager
			{
				qDebug()<<"No User could end the round in their specified bid.";
                actionWhenAnimationEnded = &GameControll::resetAndNextTarget;
                qDebug()<<"actionWhenAnimationEnded = resetAndNextTarget";
			}
		}
	}
}

void GameControll::resetAndNextTarget()
{
    getBoard()->revertToBeginning();
    nextTarget();
}

void GameControll::resetForNextUser()
{
    qDebug()<<"Acquiring next User";
    User* user = getNextUser(activeUserID); //Liste ist bereits sortiert (siehe oben), daher ist der nächste User in der Liste der User mit dem nächsthöheren Bidding
    setActiveUserID(user->getId()); //Setze nächsten Spieler als aktiv
    getBoard()->revertToBeginning(); //Setze Spielerpositionen zurück
    qDebug()<<"Active User is now "<<user->getName();
}

int GameControll::getUserIndexById(QUuid id)
{
    for(int i=0; i<users.size(); i++)
    {
        if(users.at(i)->getId()==id)
        {
            return i;
        }
    }
    return -1;
}

//works ONLY if the users are sorted by bidding before the method call
User* GameControll::getNextUser(QUuid lastUserId)
{
    for(int i=0; i<users.size(); i++)
    {
        if(users.at(i)->getId()==lastUserId)
        {
            if(i+1<users.size())
            {
                return users.at(i+1);
            }
            else
            {
                return nullptr;
            }
        }
    }
    return nullptr;
}

void GameControll::sortBy(strategy strategy)
{
    QVector<User*> sortedUsers;
    unsigned long minTimeStamp = QDateTime::currentMSecsSinceEpoch();
    bool isActive[users.size()];
    for(int i = 0; i<users.size(); i++)
    {
        isActive[i] = 1; //Set all users to be active
    }
    if(strategy == bid)
    {
        //qDebug()<<"Called sortByBidding";
        User * minUser;
        int minIndex = 0;
        int minBid;
        for(int additionIndex = 0; additionIndex < users.size(); additionIndex++)
        {
            minBid = MAX_BID + 1;
            for(User* user : users)
            {
                //qDebug()<<"SortByBidding: USER "<<user->getName()<<" with bidding "<<user->getBidding()<<"and timestamp: "<<user->getTimeStamp();
                if(user->getBidding() <= minBid && isActive[users.indexOf(user)])//If User has a lower bid than the currently lowest bid
                {
                    if(user->getBidding() == minBid)
                    {
                        if(user->getTimeStamp() < minTimeStamp) //check the timestamp
                        {
                            //qDebug()<<"Bidding is the same, timestamp is earlier";
                            minUser = user; //Set the Widget to add to the new list to the user
                            minIndex = users.indexOf(user); //Set the index needed for deactivating the user to the current index
                            minBid = user->getBidding(); //Set the newest lowest bid to the current user as there can be users after that one with lower bids
                            minTimeStamp = user->getTimeStamp(); //Set User Timestamp to the current user value
                        }
                    }
                    else
                    {
                        //qDebug()<<"Bidding of user "<<user->getName()<<" with bidding "<<user->getBidding()<<" is smaller than minimum bid of "<<minBid;
                        minUser = user; //Set the Widget to add to the new list to the user
                        minIndex = users.indexOf(user); //Set the index needed for deactivating the user to the current index
                        minBid = user->getBidding(); //Set the newest lowest bid to the current user as there can be users after that one with lower bids
                        minTimeStamp = user->getTimeStamp(); //Set User Timestamp to the current user value
                    }
                }
            }
            isActive[minIndex] = false; //Deactivate user
            sortedUsers.append(minUser);
        }
        users = sortedUsers;
    }
    if(strategy == points)
    {
        //qDebug()<<"Called sortByPoints";
        User * maxUser;
        int maxIndex = 0;
        int maxPts;
        for(int additionIndex = 0; additionIndex < users.size(); additionIndex++)
        {
            maxPts = 0;
            for(User * user : qAsConst(users))
            {
                //qDebug()<<"SortByPoints: USER "<<user->getName()<<" with points "<<user->getPoints()<<"and timestamp: "<<user->getTimeStamp();
                if(user->getPoints() >= maxPts && isActive[users.indexOf(user)])//If User has more points than the current maximum
                {
                    //qDebug()<<"Points of user "<<user->getName()<<" with points "<<user->getPoints()<<" is larger than maximum amount of "<<maxPts;
                    maxUser = user; //Set the Widget to add to the new list to the user
                    maxIndex = users.indexOf(user); //Set the index needed for deactivating the user to the current index
                    maxPts = user->getPoints(); //Set the newest largest number of points to the current user as there can be users after that one with larger number of points
                    minTimeStamp = user->getTimeStamp(); //Set the User Time stamp to the current user value
                }
            }
            isActive[maxIndex] = false; //Deactivate user
            sortedUsers.append(maxUser);
        }
        users = sortedUsers;
        for(int i = 0; i<users.size(); i++)
        {
            //qDebug()<<"SortedUsers: User "<<i<<": "<<users[i]->getName()<<" with points: "<<users[i]->getPoints()<<" and timestamp "<<users[i]->getTimeStamp();
            //isActive[i] = true;
        }
    }
    leaderboard->updateAllUsers();
}

//called by exeQTActionWithData when the action is newUser
void GameControll::addUser(User* user)
{
    bool b = false;
    for(User * u: qAsConst(instance.users))
    {
        if(u->getId()==user->getId())
        {
            b = true;
        }
    }
    if(b == false)
    {
        leaderboard->addUser(user);
        users.append(user); //TODO sort
    }
}

void GameControll::calculateWinner() //This function is being called when the first player has reached their goal
{
    QUuid activeUserId = getActiveUserID();
    User* activeUser = getUserById(activeUserId);
    nextTarget(); //Generate a new target, this should reset the current LeaderBoardWidget
    activeUser->incrementPoints();
    qDebug()<<"User "<<activeUser->getName()<<" has successfully ended the round with "<<board->getMoves()<<" moves, their current points are "<<activeUser->getPoints();
}

User* GameControll::getUserById(QUuid id)
{
    for(User* u: users)
    {
        if(u->getId()==id)
        {
            return u;
        }
    }
    return nullptr;
}

void GameControll::changeBidding(int bidding, QUuid id)
{
	qDebug()<<"Called Function Change Bidding from "<<id.toString()<< "to" << bidding;
	for (User *u: qAsConst(users))
	{
		if (u->getId() == id)
		{
			u->setBidding(bidding);
			break;
		}
	}
}

void GameControll::changeOnlyBidding(int bidding) //TODO explain! What happens here? In contrast to changeBidding?
{
	qDebug()<<"changeOnly Bidding to"<<bidding;
	users.at(0)->setBidding(bidding);
	triggerActionsWithData(PlayerAction::sendBidding, users.at(0)->toJSON());
}

// current user of the system is initialzed (only when server or client starts)
User* GameControll::initializeUser()
{
    User* u = new User(instance.getSettingsDialog()->getUsername(), instance.getSettingsDialog()->getUsercolor());
    qDebug()<<"initializeUser with id: "<<u->getId();
    //TODO
    triggerActionsWithData(PlayerAction::newUser, u->toJSON());
    return u;
}

void GameControll::setIdle()
{

	switchPhase(Phase::idle);

}

void GameControll::setLeaderboard(LeaderBoardWidget * value)
{
    //neu
    instance.leaderboard = value;
    connect(&GameControll::getInstance(), &GameControll::biddingDone, &GameControll::getInstance(), [=]()
    {
        instance.leaderboard->deactivateInput();
        instance.sortBy(bid);
        instance.setActiveUserID(instance.users.first()->getId());
    });
    connect(instance.leaderboard, &LeaderBoardWidget::biddingAccepted, &GameControll::getInstance(), [=](QUuid userId, int bidding)
    {
        QJsonObject json;
        json.insert("userId", userId.toString());
        json.insert("bidding", bidding);
        triggerActionsWithData(PlayerAction::sendBidding, json);
    });

    //alt
    //TODO set username/color in localUser of onlineWidget
//	connect(instance.settings, &SettingsDialog::usernameChanged, instance.leaderboard, &LeaderBoardWidget::setUsername);
//	connect(instance.settings, &SettingsDialog::usercolorChanged, instance.leaderboard, &LeaderBoardWidget::setUsercolor);
}

User * GameControll::getMinBid()
{
	User * minBid = instance.users.first();
	for(User * u:instance.users)
	{
		if(u->getBidding() < 100)
		{
            if(u->getBidding() < minBid->getBidding() || (u->getBidding() == minBid->getBidding() && u->getTimeStamp() < minBid->getTimeStamp()))
			{
				minBid = u;
			}
		}
	}
	return minBid;
}

GameControll::Phase GameControll::getCurrentPhase()
{
	return instance.currentPhase;
}

void GameControll::nextTarget()
{
    qDebug()<<"next target!!";
    if(switchPhase(Phase::search))
    {
        sortBy(points);
        //reset all biddings
        for(User* u: users)
        {
            u->hasBid = false;
            u->setBidding(MAX_BID);
        }
        leaderboard->activateInput();
        board->startNewRound();
    }
}

bool GameControll::switchPhase(GameControll::Phase phase)
{
	qDebug() << "GameControll::switchPhase(GameControll::Phase " << static_cast<int>(phase) << ")";
	switch(phase)
	{
	case Phase::idle:
	{
		currentPhase = phase;
		emit updateGuide(tr("idling"));
		emit enableMenus(true);
		emit enableTimerSkip(false);
		return true;
	}
	case Phase::search:
	{
		if(currentPhase != Phase::countdown)
		{
			currentPhase = phase;
			emit updateGuide(tr("start bidding"));
			emit enableMenus(false);
			emit enableTimerSkip(false);
			return true;
		}
		break;
	}
	case Phase::countdown:
	{
		{
			if(currentPhase == Phase::search)
			{
				currentPhase = phase;
				emit updateGuide(tr("counting down"));
				timeLeft = 60; //60
				emit time(timeLeft);
				countdown.start();
			}
			emit enableMenus(false);
			emit enableTimerSkip(true);
			return true;
		}
		break;
	}
	case Phase::presentation:
	{
		if(currentPhase == Phase::countdown)
		{
			emit biddingDone();
			//Set Player to player with minimum bid, aka first player after being sorted
			currentPhase = phase;
			emit updateGuide(tr("present your solutions"));
			emit enableMenus(false);
			emit enableTimerSkip(false);
			return true;
		}
		break;
	}
	case Phase::freeplay:
	{
		if(currentPhase == Phase::presentation)
		{
			currentPhase = phase;
			emit updateGuide(tr("time to show off"));
			emit enableMenus(false);
			emit enableTimerSkip(false);
			return true;
		}
		break;
	}
	}
	return false;
}

void GameControll::remakeBoard()
{
	switchPhase(Phase::idle);
	countdown.stop();
}

QVector<KeyMapping*> * GameControll::getMapping()
{
	if(!instance.settings)
	{
		load();
	}
	return &instance.mapping;
}

void GameControll::setMapping(QVector<KeyMapping*> mapping)
{
	this->mapping = mapping;
}

Board * GameControll::getBoard()
{
	return instance.board;
}

QUuid GameControll::getActiveUserID(){return activeUserID;}

void GameControll::setActiveUserID(QUuid id){activeUserID = id;}

void GameControll::updateTimer()
{
	if(--timeLeft <= 0)
	{
		endTimer();
	}
	emit time(timeLeft);
}

void GameControll::endTimer(){
	if(instance.currentPhase == Phase::countdown){
		emit instance.time(0);
		instance.countdown.stop();
		instance.switchPhase(Phase::presentation);
	}
	else{
		qDebug() << "tried to end timer that wasn't running";
	}

}

SettingsDialog * GameControll::getSettingsDialog()
{
	return instance.settings;
}

bool GameControll::showTopBidding()
{
	if(!instance.settings)
	{
		load();
	}
	return instance.settings->getShowTopBidding();
}

QVector<User*>* GameControll::getUsers()
{
	return &users;
}

GameControll::functionPointer GameControll::getActionWhenAnimationEnded()
{
    return instance.actionWhenAnimationEnded;
}
void GameControll::setActionWhenAnimationEnded(functionPointer function)
{
    instance.actionWhenAnimationEnded=function;
    qDebug()<<"actionWhenAnimationEnded was set to"<<function;
}
