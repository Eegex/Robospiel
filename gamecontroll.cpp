#include "gamecontroll.h"

#include "server.h"
#include "client.h"
#include "user.h"
#include <QJsonObject>
#include <QUuid>
#include <QUuid>

using namespace std::chrono_literals;

GameControll::GameControll(QObject *parent) : QObject(parent)
{
	countdown.setSingleShot(false);
	countdown.setInterval(1s);
	connect(&countdown,&QTimer::timeout,this,&GameControll::updateTimer);

	connect(this, &GameControll::actionTriggeredWithData, this, &GameControll::sendToServerWithData); //TODO test the connection with both signals
	connect(this, &GameControll::actionTriggered, this, &GameControll::sendToServer);
	connect(&Client::getInstance(), &Client::actionReceived, this, &GameControll::exeQTAction);
	connect(&Server::getInstance(), &Server::actionReceived, this, &GameControll::exeQTAction);

}

QJsonObject GameControll::toJSON() //TODO test both ways of JSON
{
	QJsonObject json = QJsonObject();
	json.insert("currentPhase", static_cast<int>(currentPhase));
	json.insert("board", board->toJSON());
	json.insert("activeUserID", activeUserID.toString());
	json.insert("remainingTimerTime", countdown.remainingTime());
	json.insert("timeLeft", timeLeft);
	return json;
}

GameControll* GameControll::fromJSON(QJsonObject json)
{
	GameControll* g = new GameControll();
	g->currentPhase=static_cast<Phase>(json.value("currentPhase").toInt());
	g->setBoard(Board::fromJSON(json.value("board").toObject()));
	g->setActiveUserID(QUuid::fromString(json.value("activeUserID").toString()));
	g->timeLeft = json.value("timeLeft").toInt();
	if(json.value("remainingTimerTime").toInt()!=-1)
	{
		QTimer::singleShot(json.value("remainingTimerTime").toInt(), g, [=]()->void{
			g->countdown.start();
		});

	}
	return g;
}

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
	settings = new SettingsDialog(mapping);
	settings->load();
	connect(settings,&SettingsDialog::colorsChanged,this,[&](){ board->updateColors(settings->getBackground(),settings->getWallcolor(),settings->getGridcolor()); });
	connect(settings,&SettingsDialog::newMapping,this,[&](QVector<KeyMapping*> mapping){ this->mapping = mapping; });
	if(board)
	{
		board->updateColors(settings->getBackground(),settings->getWallcolor(),settings->getGridcolor());
	}
	this->mapping = settings->getMapping();
}

void GameControll::showSettings()
{
	settings->exec();
}

Board * GameControll::setBoard(Board* newBoard)
{
	if(board)
	{
		board->deleteLater();
		board = nullptr;
	}
	board = newBoard;
	if(settings)
	{
		board->updateColors(settings->getBackground(),settings->getWallcolor(),settings->getGridcolor());
	}
	return board;
}

void GameControll::exeQTAction(QJsonObject data) //TODO maybe the bool return was needed?
{
	qDebug()<<"execute"<<data;
	PlayerAction a = static_cast<PlayerAction>(data.take("action").toInt());
	User* user;
	switch(a)
	{
		case newUser:
			user = new User(data.value("username").toString(), QColor(data.value("usercolor").toString()), QUuid(data.value("id").toString()), this);
			emit newOnlineUser(user);
			break;
		case movePlayerEast:
		case movePlayerNorth:
		case movePlayerSouth:
		case movePlayerWest:
				board->moveActivePlayer(static_cast<Direction>(a - PlayerAction::movement));
				break;
		case switchPlayerEast:
		case switchPlayerNorth:
		case switchPlayerSouth:
		case switchPlayerWest:
			board->switchPlayer(static_cast<Direction>(a-PlayerAction::playerSwitch));
			break;
		case sendBidding:
			if(a == PlayerAction::sendBidding)//If timer has not been started, start the dödöööö FINAL COUNTDOWN dödödödö dödödödödö
			{
				switchPhase(Phase::countdown);
			}

			break;
		case revert:
			board->revert();
			break;
		case revertToBeginning:
			board -> revertToBeginning();
			break;
	}

}

void GameControll::triggerActionsWithData(PlayerAction action, User* user)
{
	if(action==PlayerAction::newUser)
	{
		QJsonObject data = QJsonObject();
		data.insert("username", user->getName());
		data.insert("usercolor", user->getColor().name());
		data.insert("id", user->getId().toString());
		emit actionTriggeredWithData(action, data);
	}
}

bool GameControll::triggerAction(PlayerAction action, QUuid userID)
{
	qDebug()<<"Called function TriggerAction with parameters "<<action<<" and User ID "<<userID;
	//if(activeUserID.isNull() || userID == activeUserID) // not possible or don't care?
	//{
		if(action & PlayerAction::movement)
		{
			if(currentPhase == Phase::presentation || currentPhase == Phase::freeplay) //If online only let the active user move
			{
				//we subtract movement from action to get a direction (clever enum numbers)
				calculateGameStatus();
				emit actionTriggered(action);
				return true;
			}
		}
		else if(action & PlayerAction::playerSwitch && (currentPhase == Phase::presentation || currentPhase == Phase::freeplay)) //???
		{
			if(action != PlayerAction::playerSwitch) //if we don't want a real PlayerSwitch (withDirection), just check if we can do one (by clicking on a player)
			{

				//board->switchPlayer(static_cast<Direction>(action-PlayerAction::playerSwitch));
				emit actionTriggered(action); // should this be outside the if?

			}
			return true;
		}
		else if(action & PlayerAction::bidding) //TODO submit biddingValue
		{
			qDebug()<<"Currently in GameControl: triggerAction -> bidding, current Phase is "<<static_cast<int>(currentPhase);
			if(currentPhase == Phase::search || currentPhase == Phase::countdown)
			{
				emit actionTriggered(action); //TODO maybe inside the if?
				return true;
			}
		}
		else if(action & PlayerAction::other)
		{
			if(currentPhase == Phase::presentation || currentPhase == Phase::freeplay)
			{
				emit actionTriggered(action);
				return true;
			}
		}
	//}
	return false;
}

void GameControll::calculateGameStatus()
{
	qDebug()<<"Current Moves are: "<< board->getMoves();
	QUuid currentPlayer = getActiveUserID();
	unsigned int activeUserIndex = leaderboard->getBiddingWidgetIndexByID(currentPlayer);
	if(board->getMoves() >= leaderboard->getUsers()->at(activeUserIndex)->getBidding()) //Needs to be GEQ because otherwise the player could make an extra move as the "reached goal" signal is overriding the failure
	{
		qDebug()<<"User couldn't end the round in the specified bid of "<< leaderboard->getUsers()->at(activeUserIndex)->getBidding()<<", the next user is being drawn";
		board->resetMoves();
		if(leaderboard->getNumOfUsers() > 1 && activeUserIndex < leaderboard->getNumOfUsers()-1)//Not at last player yet, noch haben nicht alle versagt
		{
			UserBiddingWidget* user = leaderboard->getUsers()->at(++activeUserIndex); //Liste ist bereits sortiert (siehe oben), daher ist der nächste User in der Liste der User mit dem nächsthöheren Bidding
			setActiveUserID(user->getId()); //Setze nächsten Spieler als aktiv
			getBoard()->revertToBeginning(); //Setze Spielerpositionen zurück
			qDebug()<<"Active User is now "<<user->getName();
		}
		else //Alles Versager
		{
			qDebug()<<"No User could end the round in their specified bid.";
			leaderboard->sortBy(points);
			nextTarget();
		}
	}
}

//this method is only for offline-users
void GameControll::addUser(struct UserData * newUser)
{
	qDebug()<<"adduser in MainWidget";
	User *u = new User(newUser->name, newUser->colour, this);
	users.append(u);
	qDebug()<< u->getName();
	// adds new user in the frontend
	leaderboard->addUser(u);
	connect(this, &GameControll::newRound, leaderboard->getUsers()->last(), &UserBiddingWidget::resetBidding);
	connect(this, &GameControll::biddingDone, leaderboard->getUsers()->last(), &UserBiddingWidget::deactivateBtn);
	connect(leaderboard->getUsers()->last(), &UserBiddingWidget::biddingChanged, this, &GameControll::changeBidding);
	connect(leaderboard->getUsers()->last(), &UserBiddingWidget::biddingReset, this, &GameControll::changeBidding);
	connect(leaderboard->getUsers()->last(), &UserBiddingWidget::biddingChanged, this, [&](const int, const QUuid id) //Connect the biddingChanged Signal to triggerAction with appropriate argument
	{
		triggerAction(PlayerAction::sendBidding, id);
	});
}

void GameControll::addExistingUser(User* user)
{
	qDebug()<<"addExistingUser";
	bool b = false;
	for(User * u: qAsConst(users))
	{
		if(u->getId()==user->getId())
		{
			b = true;
		}
	}
	if(b == false)
	{
		qDebug()<<"add "<<user->getId()<< " into list";
		users.append(user);
		triggerAction(PlayerAction::newUser, user->getId());
		leaderboard->getUserOnlineWidget()->addUserToList(user);
		leaderboard->getUserOnlineWidget()->updateUserList();
	}

}

void GameControll::calculateWinner(int moves) //This function is being called when the first player has reached their goal
{
	QUuid currentPlayer = getActiveUserID();
	unsigned int activeUserIndex = leaderboard->getBiddingWidgetIndexByID(currentPlayer);
	nextTarget(); //Generate a new target, this should reset the current LeaderBoardWidget
	leaderboard->getUsers()->at(activeUserIndex)->incrementPoints(); //Increment the number of points of the player that's hit their goal
	leaderboard->sortBy(points);
	qDebug()<<"User "<<leaderboard->getUsers()->at(activeUserIndex)->getName()<<" has successfully ended the round with "<<moves<<" moves, their current points are "<<leaderboard->getUsers()->at(activeUserIndex)->getPoints();
	board->resetMoves(); //This needs to be done because the actionTriggered will set it to 1 immediately after a goal is hit so the next player has 1 fewer move which would be bad
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

void GameControll::changeOnlyBidding(int bidding)
{
	qDebug()<<"changeOnly Bidding to"<<bidding;
	users.at(0)->setBidding(bidding);
	triggerAction(PlayerAction::enterBidding, users.at(0)->getId());
}

// current user of the system is initialzed
void GameControll::initializeUser()
{
	qDebug()<<"initializeUser: ";
	User *u = new User(leaderboard->getUsername(), leaderboard->getUsercolor(), this);
	qDebug()<<"username: "<<leaderboard->getUsername()<<" and usercolor: "<< leaderboard->getUsercolor();
	users.append(u);
	triggerActionsWithData(PlayerAction::newUser, u);
}

void GameControll::activePlayerChanged(int playerNumber) //Brauchen wir das noch? Ich dachte wir machen alles mit der ID? - Also momentan ja...
{
   if(triggerAction(PlayerAction::playerSwitch, ""))
	{
		board->changeActivePlayer(playerNumber);
	}
}

void GameControll::setLeaderboard(LeaderBoardWidget * value)
{
	leaderboard = value;
	connect(leaderboard->getUserCreationWidget(), &UserCreationWidget::userAdded, this, &GameControll::addUser);
	connect(this, &GameControll::biddingDone, this, [&]()
	{
		leaderboard->sortBy(bid);
		setActiveUserID(leaderboard->getUsers()->first()->getId());
		qDebug()<<"Bidding is done, Users are sorted, initial player is: "<<leaderboard->getUsers()->first()->getName()<<" with id "<< getActiveUserID();
	});
	connect(leaderboard->getUserOnlineWidget(), &UserOnlineWidget::userAdded, this, &GameControll::addUser);
	connect(leaderboard->getUserOnlineWidget(), &UserOnlineWidget::biddingChangedOnline,this,&GameControll::changeOnlyBidding);
	connect(settings, &SettingsDialog::usernameChanged, leaderboard, &LeaderBoardWidget::setUsername);
	connect(settings, &SettingsDialog::usercolorChanged, leaderboard, &LeaderBoardWidget::setUsercolor);
	connect(leaderboard->getNetworkView(), &NetworkView::leaderboradOnline, this, &GameControll::initializeUser);
}

User * GameControll::getMinBid()
{
	User * minBid = users.first();
	for(User * u:users)
	{
		if(u->getBidding() < 99)
		{
			if(u->getBidding() < minBid->getBidding() || (u->getBidding() == minBid->getBidding() && u->getLastBiddingTime() < minBid->getLastBiddingTime()))
			{
				minBid = u;
			}
		}
	}
	return minBid;
}

GameControll::Phase GameControll::getCurrentPhase() const
{
	return currentPhase;
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
		return true;
	}
	case Phase::search:
	{
		if(currentPhase != Phase::countdown)
		{
			currentPhase = phase;
			emit updateGuide(tr("start bidding"));
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
	if(!settings)
	{
		load();
	}
	return &mapping;
}

void GameControll::setMapping(QVector<KeyMapping*> mapping)
{
	this->mapping = mapping;
}

Board * GameControll::getBoard() const
{
	return board;
}

QUuid GameControll::getActiveUserID(){return activeUserID;}

void GameControll::setActiveUserID(QUuid id){activeUserID = id;}

void GameControll::updateTimer()
{
	if(--timeLeft <= 0)
	{
		countdown.stop();
		switchPhase(Phase::presentation);
	}
	emit time(timeLeft);
}

SettingsDialog * GameControll::getSettingsDialog()
{
	return settings;
}

bool GameControll::showTopBidding()
{
	if(!settings)
	{
		load();
	}
	return settings->getShowTopBidding();
}
