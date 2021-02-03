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
	setLeaderboard(new LeaderBoardWidget());
	QJsonArray jsonUsers = json.value("users").toArray();
	for(int i=0; i<jsonUsers.size(); i++)
	{
		QJsonObject jsonUser = jsonUsers.at(i).toObject();
		if(instance.leaderboard->getOnlineState()==online)
		{
			addOnlineUser(User::fromJSON(jsonUser));
		}
		else if(instance.leaderboard->getOnlineState()==offline)
		{
			qDebug()<<"Called function AdaptFromJson in GameControl with an offline state, attempting to add an offline user!";
			UserData userData;
			//some data present in the inline version is "missing" here,
			//but it shouldn't be a problem, because it is created in addOfflineUser()
			//and it doesn't have to be present yet, because it isn't shared via internet
			userData.name = jsonUser.value("name").toString();
			userData.colour = QColor(jsonUser.value("color").toString());
			instance.addOfflineUser(&userData);
		}
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
	qDebug()<<"execute"<<data;
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
			board->changeActivePlayer(data.value("playerNumber").toInt());
		case sendBidding:
			switchPhase(Phase::countdown);
			if (leaderboard->getOnlineState()==state::online)
			{
				leaderboard->getUserOnlineWidget()->setBidding(QUuid(data.value("id").toString()),data.value("bidding").toInt());
			}

			break;
		case revert:
			board->revert();
			break;
		case revertToBeginning:
			board -> revertToBeginning();
			break;
		case newUser:
			if(leaderboard->getOnlineState()==state::online)
			{
				//TODO not testet yet, is not in use
				user = User::fromJSON(data);
				addOnlineUser(user);
				emit newOnlineUser(user);
			}
			else if(leaderboard->getOnlineState()==state::offline)
			{
				qDebug()<<"Called Function ExeQTAction with an offline state, attempting to add an offline user!";
				userData.name = data.value("name").toString();
				userData.colour = QColor(data.value("color").toString());
				addOfflineUser(&userData);
			}
			break;
		case completeUpdate:
			adaptFromJSON(data);
	}
}

//
//Don't write actual functionality here! It only sends the actions to the server. Functionality is in exeQTdata()!!!
//TODO what is the meaning of the return value?
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
			emit instance.actionTriggered(action);
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
	if(leaderboard->getOnlineState() == offline)
	{
		qDebug()<<"Current Moves are: "<< board->getMoves();
		QUuid currentPlayer = getActiveUserID();
		unsigned int activeUserIndex = leaderboard->getOfflineLeaderBoardWidget()->getBiddingWidgetIndexByID(currentPlayer);
		if(board->getMoves() >= leaderboard->getOfflineLeaderBoardWidget()->getUsers()->at(activeUserIndex)->getBidding()) //Needs to be GEQ because otherwise the player could make an extra move as the "reached goal" signal is overriding the failure
		{
			if(board->goalHit) //Spieler hat gewonnen, die Runde ist zuende
			{
				calculateWinner(board->getMoves());
			}
			else
			{
				//TODO: Flag um anzuzeigen, dass der Spieler das Ziel erreicht hat?
				qDebug()<<"User couldn't end the round in the specified bid of "<< leaderboard->getOfflineLeaderBoardWidget()->getUsers()->at(activeUserIndex)->getBidding()<<", the next user is being drawn";
				if(leaderboard->getOfflineLeaderBoardWidget()->getNumOfUsers() > 1 && activeUserIndex < leaderboard->getNumOfUsers()-1)//Not at last player yet, noch haben nicht alle versagt
				{
					UserBiddingWidget* user = leaderboard->getOfflineLeaderBoardWidget()->getUsers()->at(++activeUserIndex); //Liste ist bereits sortiert (siehe oben), daher ist der nächste User in der Liste der User mit dem nächsthöheren Bidding
					setActiveUserID(user->getId()); //Setze nächsten Spieler als aktiv
					getBoard()->revertToBeginning(); //Setze Spielerpositionen zurück
					qDebug()<<"Active User is now "<<user->getName();
				}
				else //Alles Versager
				{
					qDebug()<<"No User could end the round in their specified bid.";
					leaderboard->getOfflineLeaderBoardWidget()->sortBy(points);
					nextTarget();
				}
			}
		}
	}
}

//this method is only for offline-users
//called by exeQTActionWithData when the action is newUser and it is offline
void GameControll::addOfflineUser(struct UserData * newUser)
{
	qDebug()<<"Called function adduser in GameControl";
	if(leaderboard->getOnlineState() == offline){
		qDebug()<<"Running in Offline Mode, adding an Offline User!";
		User *u = new User(newUser->name, newUser->colour, this);
		users.append(u);
		qDebug()<<"New User Name: "<<u->getName();
		// adds new user in the frontend
		leaderboard->getOfflineLeaderBoardWidget()->addUser(u);
		connect(this, &GameControll::newRound, leaderboard->getOfflineLeaderBoardWidget()->getUsers()->last(), &UserBiddingWidget::resetBidding);
		connect(this, &GameControll::biddingDone, leaderboard->getOfflineLeaderBoardWidget()->getUsers()->last(), &UserBiddingWidget::deactivateBtn);
		connect(leaderboard->getOfflineLeaderBoardWidget()->getUsers()->last(), &UserBiddingWidget::biddingChanged, this, &GameControll::changeBidding);
		connect(leaderboard->getOfflineLeaderBoardWidget()->getUsers()->last(), &UserBiddingWidget::biddingReset, this, &GameControll::changeBidding);
		connect(leaderboard->getOfflineLeaderBoardWidget()->getUsers()->last(), &UserBiddingWidget::biddingChanged, this, [&](const int, const QUuid id) //Connect the biddingChanged Signal to triggerAction with appropriate argument
		{
			triggerAction(PlayerAction::sendBidding, id);
		});
	}
}

//called by exeQTActionWithData when the action is newUser and it is online
void GameControll::addOnlineUser(User* user)
{
	qDebug()<<"addExistingUser";
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
		qDebug()<<"add "<<user->getId()<< " into list";
		instance.users.append(user);
		instance.leaderboard->getUserOnlineWidget()->addUserToList(user);
		instance.leaderboard->getUserOnlineWidget()->updateUserList();
	}

}

void GameControll::calculateWinner(int moves) //This function is being called when the first player has reached their goal
{
	QUuid currentPlayer = getActiveUserID();
	if(leaderboard->getOnlineState() == offline){
		qDebug()<<"Goal has been hit!";
		unsigned int activeUserIndex = leaderboard->getOfflineLeaderBoardWidget()->getBiddingWidgetIndexByID(currentPlayer);
		nextTarget(); //Generate a new target, this should reset the current LeaderBoardWidget
		leaderboard->getOfflineLeaderBoardWidget()->getUsers()->at(activeUserIndex)->incrementPoints(); //Increment the number of points of the player that's hit their goal
		leaderboard->getOfflineLeaderBoardWidget()->sortBy(points);
		qDebug()<<"User "<<leaderboard->getOfflineLeaderBoardWidget()->getUsers()->at(activeUserIndex)->getName()<<" has successfully ended the round with "<<moves<<" moves, their current points are "<<leaderboard->getOfflineLeaderBoardWidget()->getUsers()->at(activeUserIndex)->getPoints();
		board->resetMoves(); //This needs to be done because the actionTriggered will set it to 1 immediately after a goal is hit so the next player has 1 fewer move which would be bad
	}
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

// current user of the system is initialzed
void GameControll::initializeUser()
{
	qDebug()<<"initializeUser: ";
	User *u = new User(instance.leaderboard->getUsername(), instance.leaderboard->getUsercolor());
	qDebug()<<"username: "<<instance.leaderboard->getUsername()<<" and usercolor: "<< instance.leaderboard->getUsercolor();
	instance.users.append(u);
	instance.leaderboard->getUserOnlineWidget()->addUserToList(u);
	instance.leaderboard->getUserOnlineWidget()->updateName(u->getName());
	instance.leaderboard->getUserOnlineWidget()->setUserID(u->getId());
	instance.leaderboard->getUserOnlineWidget()->setTable();
	triggerActionsWithData(PlayerAction::newUser, u->toJSON());
}

void GameControll::setIdle()
{

	switchPhase(Phase::idle);

}

void GameControll::setLeaderboard(LeaderBoardWidget * value)
{
	instance.leaderboard = value;
	connect(instance.leaderboard->getOfflineLeaderBoardWidget()->getUserCreationWidget(), &UserCreationWidget::userAdded, &GameControll::getInstance(), [=](UserData* userData)->void
	{
		qDebug()<<"In Connect-Function in GameControl!";
		QJsonObject data = QJsonObject();
		data.insert("name", userData->name);
		data.insert("color", userData->colour.name());
		triggerActionsWithData(PlayerAction::newUser, data);
	});
	connect(&GameControll::getInstance(), &GameControll::biddingDone, &GameControll::getInstance(), [&]()
	{
		instance.leaderboard->getOfflineLeaderBoardWidget()->sortBy(bid);
		qDebug()<<"ActiveUserID is"<<instance.getActiveUserID();
		qDebug()<<"Attempting to switch to new Player, first ID is "<<instance.leaderboard->getOfflineLeaderBoardWidget()->getUsers()->first()->getId();
		instance.setActiveUserID(instance.leaderboard->getOfflineLeaderBoardWidget()->getUsers()->first()->getId());
		qDebug()<<"Bidding is done, Users are sorted, initial player is: "<<instance.leaderboard->getOfflineLeaderBoardWidget()->getUsers()->first()->getName()<<" with id "<< instance.getActiveUserID();
	});
	connect(instance.leaderboard->getUserOnlineWidget(), &UserOnlineWidget::userAdded, &GameControll::getInstance(), &GameControll::addOfflineUser); //????
	connect(instance.leaderboard->getUserOnlineWidget(), &UserOnlineWidget::biddingChangedOnline,&GameControll::getInstance(),&GameControll::changeOnlyBidding);
	connect(instance.settings, &SettingsDialog::usernameChanged, instance.leaderboard, &LeaderBoardWidget::setUsername);
	connect(instance.settings, &SettingsDialog::usercolorChanged, instance.leaderboard, &LeaderBoardWidget::setUsercolor);
	connect(instance.leaderboard, &LeaderBoardWidget::onlineUserAdded, &GameControll::getInstance(), [=](User* user)->void {
				triggerActionsWithData(PlayerAction::newUser, user->toJSON());
	});
	connect(instance.settings, &SettingsDialog::usernameChanged,instance.leaderboard->getUserOnlineWidget(),&UserOnlineWidget::updateName);
	instance.leaderboard->getUserOnlineWidget()->updateName(instance.settings->getUsername());

}

User * GameControll::getMinBid()
{
	User * minBid = instance.users.first();
	for(User * u:instance.users)
	{
		if(u->getBidding() < 100)
		{
			if(u->getBidding() < minBid->getBidding() || (u->getBidding() == minBid->getBidding() && u->getLastBiddingTime() < minBid->getLastBiddingTime()))
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
	if(switchPhase(Phase::search))
	{
		emit newRound();
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
