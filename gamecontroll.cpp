#include <QFileDialog>
#include <QJsonObject>
#include <QStringList>
#include <QUuid>
#include "gamecontroll.h"
#include "server.h"
#include "client.h"
#include "user.h"
#include "onlineleaderboardwidget.h"
#include "stackwidget.h"

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
	r = new QRandomGenerator(QTime::currentTime().msecsSinceStartOfDay());
	player = new QMediaPlayer;
}

/*!
 * \brief GameControll::initializeConnections relies on Server and Client singletons.
 * Initialization of static variables can happen in random order, so this has to wait, until all static variables are present.
 */
void GameControll::initializeConnections()
{
	connect(&Server::getInstance(), &Server::actionReceived, &instance, &GameControll::exeQTAction);
	connect(&Client::getInstance(), &Client::actionReceived, &instance, &GameControll::exeQTAction);
	connect(&instance.countdown,&QTimer::timeout, &instance,&GameControll::updateTimer);
	connect(&instance, &GameControll::actionTriggeredWithData, &instance, &GameControll::sendToServerWithData);
	connect(&instance, &GameControll::actionTriggered, &instance, &GameControll::sendToServer);
	connect(&instance.guideTimer,&QTimer::timeout, &instance,&GameControll::nextGuide);
}

void GameControll::startNetworkDebugger()
{
	if(!instance.nwModel)
	{
		instance.nwModel = new NetworkModel;
		instance.nwModel->setUsers(&instance.users);
		instance.debugger = new NetworkDebugger(instance.nwModel);
		instance.debugger->show();
	}
	else
	{
		if(!instance.debugger->isVisible())
		{
			instance.debugger->show();
		}
		else
		{
			instance.nwModel->clear();
		}
	}
}

QJsonObject GameControll::toJSON() //TODO make sure the replaced Objects don't get lost
{
	//TODO add guideTimer, r, guideList?
	QJsonObject json = QJsonObject();
	json.insert("currentPhase", static_cast<int>(instance.currentPhase));
	json.insert("board", instance.board->toBinary());
	json.insert("activeUserID", instance.activeUserID.toString());
	json.insert("searchTime", instance.searchTime);
	json.insert("remainingTimerTime", instance.countdown.remainingTime());
	json.insert("timeLeft", instance.timeLeft);
	json.insert("voteCounter", instance.voteCounter);
	json.insert("voteThreshold", instance.voteThreshold);
	json.insert("searchTime", instance.searchTime);
	QJsonArray jsonUsers;
	for(User * user : qAsConst(instance.users))
	{
		jsonUsers.append(user->toJSON());
	}
	json.insert("users", jsonUsers);

	int actionWhenAnimationEndedAsInt=0;
	if(instance.actionWhenAnimationEnded==&GameControll::calculateWinner)
	{
		actionWhenAnimationEndedAsInt=1;
	}
	if(instance.actionWhenAnimationEnded==&GameControll::resetForNextUser)
	{
		actionWhenAnimationEndedAsInt=2;
	}
	if(instance.actionWhenAnimationEnded==&GameControll::resetAndNextTarget)
	{
		actionWhenAnimationEndedAsInt=3;
	}
	json.insert("actionWhenAnimationEndedAsInt", actionWhenAnimationEndedAsInt);
	return json;
}

void GameControll::adaptFromJSON(QJsonObject json)
{
	setBoard(Board::fromBinary(json.value("board").toString()));
	instance.setActiveUserID(QUuid::fromString(json.value("activeUserID").toString()));
	instance.searchTime = json.value("searchTime").toInt();
	instance.timeLeft = json.value("timeLeft").toInt();
	instance.countdown.stop();
	instance.voteCounter=json.value("voteCounter").toInt(); //update of the btn in switchPhase
	instance.voteThreshold=json.value("voteThreshold").toInt();
	if(json.value("remainingTimerTime").toInt()!=-1)
	{
		QTimer::singleShot(json.value("remainingTimerTime").toInt(), &instance, [=]()
		{
			instance.countdown.start();
		});
	}
	instance.users.clear();
	QJsonArray jsonUsers = json.value("users").toArray();
	for(int i=0; i<jsonUsers.size(); i++)
	{
		QJsonObject jsonUser = jsonUsers.at(i).toObject();
		User * user = User::fromJSON(jsonUser);
		instance.users.append(user);
		instance.leaderboard->addUser(user);
	}

	// This should stay down here in order to ensure we have the users before we set the phase to "presentation" (there we need the name of the active user)

	int phase = json.value("currentPhase").toInt();
	instance.setPhase(static_cast<Phase>(phase));

	switch(json.value("actionWhenAnimationEndedAsInt").toInt())
	{
	case 0:
	{
		instance.actionWhenAnimationEnded = nullptr;
		break;
	}
	case 1:
	{
		instance.actionWhenAnimationEnded = &GameControll::calculateWinner;
		break;
	}
	case 2:
	{
		instance.actionWhenAnimationEnded = &GameControll::resetForNextUser;
		break;
	}
	case 3:
	{
		instance.actionWhenAnimationEnded = &GameControll::resetAndNextTarget;
		break;
	}
	}
}

/*!
 * \brief GameControll::sendToServerWithData forwards the message to all clients / triggers the action directly when you are offline
 * \param a Playeraction
 * \param info JSON Object
 */
void GameControll::sendToServerWithData(PlayerAction a, QJsonObject info)
{
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

/*!
 * \brief GameControll::sendToServer forwards the message to all clients / triggers the action directly when you are offline
 * \param a Playeraction to forward to clients
 */
void GameControll::sendToServer(PlayerAction a)
{
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
		instance.board->updateColors(instance.settings->getBackground(), instance.settings->getCheckerboardColor(), instance.settings->getWallcolor(), instance.settings->getGridcolor(), instance.settings->getPlayerColorLow(), instance.settings->getPlayerColorHigh());
	});
	connect(instance.settings, &SettingsDialog::newMapping, &GameControll::getInstance(),[&](QVector<KeyMapping*> mapping){ instance.mapping = mapping; });
	connect(instance.settings, &SettingsDialog::usercolorChanged, &instance, [=](QColor color)
	{
		if(Server::isActive() || Client::isActive())
		{
			triggerActionWithData(PlayerAction::changedUserColor, {{"id", getLocalUser()->getId().toString()},{"color", color.name()}});
		}
	});
	connect(instance.settings, &SettingsDialog::usernameChanged, &instance, [=](QString name)
	{
		if(Server::isActive() || Client::isActive())
		{
			triggerActionWithData(PlayerAction::changedUsername, {{"id", getLocalUser()->getId().toString()},{"name", name}});
		}
	});
	connect(instance.settings, &SettingsDialog::timertimeChanged, &instance, [=](int length)
	{
		if(Server::isActive())
		{
			triggerActionWithData(PlayerAction::changedTimerTime, {{"length", length}});
		}
	});
	if(instance.board)
	{
		instance.board->updateColors(instance.settings->getBackground(), instance.settings->getCheckerboardColor(), instance.settings->getWallcolor(), instance.settings->getGridcolor(), instance.settings->getPlayerColorLow(), instance.settings->getPlayerColorHigh());
	}
	instance.mapping = instance.settings->getMapping();
	instance.searchTime = instance.settings->getTimerTime();
	//instance.settings->giveServerTimerTimeToEveryone(); //Wir sagen einfach jedem Client er soll seine Zeit schicken und dabei wird dann automatisch die des Servers genommen
}

void GameControll::showSettings()
{
	settings->exec();
}

Board * GameControll::setBoard(Board* newBoard)
{
	instance.killSolver();
	if(instance.board)
	{
		instance.board->deleteLater();
	}
	instance.board = newBoard;
	if(instance.settings)
	{
		instance.board->updateColors(instance.settings->getBackground(), instance.settings->getCheckerboardColor(), instance.settings->getWallcolor(), instance.settings->getGridcolor(), instance.settings->getPlayerColorLow(), instance.settings->getPlayerColorHigh());
	}
	connect(instance.board, &Board::playerMoved, &GameControll::getInstance(), [&](int, int) // Please note that this connect has to come BEFORE the one that is below, otherwise we will have a problem with reverting to the thrid to last positions once the steps are all used up :(!
	{
		emit instance.updateMoves(instance.board->getMoves());
	});
	connect(instance.board, &Board::playerMoved, &GameControll::getInstance(), &GameControll::calculateGameStatus); // Please note that this connect has to come AFTER the one that is above!
	connect(instance.board, &Board::boardChanged, &GameControll::getInstance(), &GameControll::syncBoard);
	emit instance.newBoard(instance.board);
	return instance.board;
}

/*!
 * \brief GameControll::exeQTAction executes different actions, because they were send by the server / triggered directly when you are offline
 * \param data
 */
void GameControll::exeQTAction(QJsonObject data)
{
	PlayerAction a = static_cast<PlayerAction>(data.take("action").toInt());
	User * user = nullptr;
	switch(a)
	{
	case movePlayerEast:
	case movePlayerNorth:
	case movePlayerSouth:
	case movePlayerWest:
	{
		//we subtract movement from action to get a direction (clever enum numbers)
		board->moveActivePlayer(static_cast<Direction>(a - PlayerAction::movement));
		break;
	}
	case switchPlayerEast:
	case switchPlayerNorth:
	case switchPlayerSouth:
	case switchPlayerWest:
	{
		board->switchPlayer(static_cast<Direction>(a - PlayerAction::playerSwitch));
		break;
	}
	case playerSwitch:
	{
		board->changeActivePlayer(data.value("playerNumber").toInt(), data.value("isRevert").toBool());
		break;
	}
	case sendBidding:
	{
		user = getUserById(QUuid(data.value("userId").toString()));
		user->setBidding(data.value("bidding").toInt());
		switchPhase(Phase::countdown);
		break;
	}
	case revert:
	{
		board->revert();
		break;
	}
	case revertToBeginning:
	{
		board->revertToBeginning();
		break;
	}
	case resetPoints:
	{
		for(User* u : *(instance.getUsers()))
		{
			u->resetPoints();
		}
		break;
	}
	case giveUp:
	{
		GameControll::getInstance().handleUserGivingUp();
		break;
	}
	case newUser:
	{
		user = User::fromJSON(data);
		addUser(user);
		if(localUserIsServer()){
			if(getLocalUser()){

				triggerActionWithData(PlayerAction::sendServerID, {{"id", getLocalUser()->getId().toString()}});
			}
		}
		break;
	}
	case changeActiveUser:
	{
		letUserPlayFree(QUuid(data.value("userId").toString()));
		break;
	}
	case completeUpdate:
	{
		adaptFromJSON(data);
		break;
	}
	case blockBoard:
	{
		boardBlocked = true;
		emit setBoardEnabled(false);
		break;
	}
	case editBoard:
	{
		setBoard(Board::fromBinary(data.value("board").toString()));
		emit setBoardEnabled(true);
		boardBlocked = false;
		break;
	}
	case setIdle:
	{
		switchPhase(Phase::idle);
		break;
	}
	case syncRandomGenerators:
	{
		updateRandomGenerator(data.value("Seed").toInt());
		break;
	}
	case vote:
	{
		if(!getUserById(QUuid(data.value("userId").toString()))->getHasVoted())
		{
			voteCounter++;
		}
		getUserById(QUuid(data.value("userId").toString()))->setHasVoted(true);
		updateVoteNumbers();
		break;
	}
	case PlayerAction::nextTarget:
	{
		nextTarget();
		break;
	}
	case userLeft:
	{
		QUuid id = QUuid(data.value("userId").toString());
		user = getUserById(id);

		if(activeUserID==id)
		{
			handleUserGivingUp();
		}
		calculateGameStatus();

		users.remove(users.indexOf(user));
		leaderboard->updateAllUsers();

		if(user->getHasVoted())
		{
			voteCounter--;
		}
		updateVoteNumbers();

		if(!GameControll::votekickActive()) //if the leaving user was the third one, votekick is now disabled.
		// -> Resetting votes
		{
			for(User* u : users)
			{
				u->resetVotekick();
			}
		}
		break;
	}
	case changedUserColor:
	{
		getUserById(QUuid(data.value("id").toString()))->setColor(QColor(data.value("color").toString()));
		leaderboard->updateColour(QUuid(data.value("id").toString()), QColor(data.value("color").toString()));
		break;
	}
	case changedUsername:
	{
		getUserById(QUuid(data.value("id").toString()))->setName(data.value("name").toString());
		leaderboard->updateName(QUuid(data.value("id").toString()), data.value("name").toString());
		if(instance.localUserIsServer())
		{
			if(getLocalUser())
			{
				triggerActionWithData(PlayerAction::sendServerID, {{"id", getLocalUser()->getId().toString()}});
			}
		}
		break;
	}
	case changedTimerTime:
	{
		searchTime = data.value("length").toInt();
		break;
	}
	case switchServer:
	{
		QUuid id = QUuid(data.value("id").toString());
		QString ip = data.value("ip").toString();
		int port = data.value("port").toInt();

		if(id == static_cast<OnlineLeaderboardWidget*>(leaderboard)->getLocalUser()->getId()) //we are the new server
		{
			//this user has to be the new server
			Client::closeClient();
			Server::startServer(ip, port);

			//connections between leaderboard and Users are lost, since the leaderboard got replaced
			//make new connections
			for(User* u:users)
			{
				leaderboard->addUser(u);
			}

			QTimer::singleShot(2000, this, &Server::cleanupUsers);


		}
		else //we will now be a client
		{
			if(!localUserIsServer()) // we used to be a client and are still only a client
			{
				Client::closeClient();
			}
			else
			{
				bool restart = data.value("restartOldServer").toBool();
				if(!restart) {
					StackWidget::disconnectFromServer();
					return;
				}
				Server::closeServer();
			}
			Client::getInstance().startClient(ip, port);
			emit instance.enableServerSwitchBtn(instance.localUserIsServer()); //TODO: warum ist das nötig? Sollte eigentlich von Server 116 emit clientsChanged(connections.length()); geregelt werden.
		}
		//updateVoteNumbers();
		emit updateActionButtonText();
		instance.leaderboard->updateServerName(id, getUserById(id)->getName());
		break;
	}
	case sendServerID:
	{
		QUuid id = QUuid(data.value("id").toString());
		instance.leaderboard->updateServerName(id, getUserById(id)->getName());
		break;

	}
	case votekick:
	{
		User* from = getUserById(QUuid(data.value("fromUser").toString()));
		User* to = getUserById(QUuid(data.value("toUser").toString()));
		from->setVotekick(to->getId(), true);
		to->receiveVotekick();
		break;
	}
	}
}

/*!
 * \brief GameControll::triggerAction
 * \param action
 * \warning Don't write actual functionality here! It only sends the actions to the server. Functionality is in exeQTdata()!!!
 */
void GameControll::triggerAction(PlayerAction action)
{
	if(action & PlayerAction::movement && instance.localUserIsActiveUser())
	{
		if((instance.currentPhase == Phase::presentation || instance.currentPhase == Phase::freeplay)) //If online only let the active user move
		{
			emit instance.actionTriggered(action);
			return;
		}
	}
	else if(action & PlayerAction::playerSwitch && (instance.currentPhase == Phase::presentation || instance.currentPhase == Phase::freeplay))
	{
		if(instance.localUserIsActiveUser() && action != PlayerAction::playerSwitch)
		{
			emit instance.actionTriggeredWithData(action, {{"isRevert", false}});
			return;
		}
	}
	else if(action & PlayerAction::bidding) //TODO submit biddingValue
	{
		Q_ASSERT_X(false, "GameControll::triggerAction", "use triggerActionWithData to send biddings!");
		//if(instance.currentPhase == Phase::search || instance.currentPhase == Phase::countdown)
		//{
		//	emit instance.actionTriggered(action);
		//	return;
		//}
	}
	else if(action & PlayerAction::other) //TODO overly complicated ifs? Remove or explain!
	{
		if(instance.currentPhase == Phase::presentation || instance.currentPhase == Phase::freeplay)
		{
			if(action == PlayerAction::revert || action == PlayerAction::revertToBeginning){ //you can only revert if you are the active player
				if(instance.localUserIsActiveUser()){
					emit instance.actionTriggered(action);
				}
			}
			else{
				emit instance.actionTriggered(action);
				return;
			}
		}
	}
	else if(action & PlayerAction::menuAction)
	{
		if(action == PlayerAction::setIdle && instance.currentPhase == Phase::search && !Server::isActive())
		{ //if any client manages to press idle even though they are in search (and it should be disabled) its caught here
			return;
		}
		emit instance.actionTriggered(action);
		return;
	}
	else if (action==PlayerAction::resetPoints)
	{
		emit instance.actionTriggered(action);
	}
	else if(action == PlayerAction::vote)
	{
		emit instance.triggerActionWithData(PlayerAction::vote,{{"userId", GameControll::getLocalUser()->getId().toString()}});
	}
	return;
}

void GameControll::addTransmission(QJsonObject transmission)
{
	if(instance.nwModel)
	{
		instance.nwModel->addTransmission(transmission);
	}
}

/*!
 * \brief GameControll::triggerActionWithData used for every syncronization over the network
 * \param action
 * \param data
 */
void GameControll::triggerActionWithData(PlayerAction action, QJsonObject data)
{
	switch (action)
	{
	case playerSwitch:
	{
		if(!((instance.currentPhase == Phase::presentation && instance.localUserIsActiveUser()) || instance.currentPhase == Phase::freeplay))
		{
			return;
		}
		break;
	}
	case sendBidding:
	{
		if(!(instance.currentPhase == Phase::search || instance.currentPhase == Phase::countdown))
		{
			return;
		}
		break;
	}
	case changeActiveUser:
	{
		if(!instance.localUserIsActiveUser() || instance.activeUserID == QUuid(data.value("userId").toString()) || instance.currentPhase != Phase::freeplay)
		{
			return;
		}
		break;
	}
	case changedTimerTime:
	{
		if(instance.currentPhase == Phase::countdown)
		{
			return;
		}
		break;
	}
	case editBoard:
	{
		if(!instance.boardBlocked)
		{
			return;
		}
		break;
	}
	default:
	{
		break;
	}
	}
	emit instance.actionTriggeredWithData(action, data);
}

/*!
 * \brief GameControll::calculateGameStatus called after each movement of a player (and when reverting, ...)
 * This function calculates whether the game has ended, either with the player hitting the goal or the player running out of moves.
 * In the latter case, the next player is being drawn
 */
void GameControll::calculateGameStatus()
{
	//TODO: Maybe we also want there to be some kind of message when we "win" in freeplay
	if(currentPhase == Phase::presentation){
		/*if(board->getMoves() < getUserById(activeUserID)->getBidding())
	{*/
		if(board->goalHit) //Spieler hat gewonnen, die Runde ist zuende
		{
			actionWhenAnimationEnded = &GameControll::calculateWinner;
		}
		//}
		else
		{
			if(board->getMoves() >= getUserById(activeUserID)->getBidding())
			{
				//TODO: Flag um anzuzeigen, dass der Spieler das Ziel erreicht hat?
				GameControll::triggerAction(revert);
				User * user = instance.users.first();
				const QString& username = user->getName();
				showGuide({tr("You are out of steps, looser!")+ "[]", tr("Getting frustrated? Maybe pay attention next time you bit!")+ "[]", tr("Buhuuu! All your steps are up, ") + username + "!" + "[]", tr("No steps, no luck, I guess, your stuck!")+ "[]", tr("Somebody doesn't know how to count!")+ "[]"});
			}
		}
	}
}

void GameControll::handleUserGivingUp()
{
	User* nextUser = getNextUser(activeUserID);
	if(nextUser)//Not at last player yet, noch haben nicht alle versagt
	{
		GameControll::resetForNextUser();
	}
	else //Alles Versager
	{
		//No one reached the goal so after freeplay it should load the reverted state
		instance.board->revertToBeginning();
		switchPhase(Phase::freeplay);
		//GameControll::resetAndNextTarget();
	}
}

void GameControll::resetAndNextTarget()
{
	getBoard()->revertToBeginning();
	emit updateMoves(0);
	nextTarget();
}

bool GameControll::localUserIsActiveUser()
{
	if(Server::isActive() || Client::isActive())
	{
		return activeUserID == static_cast<OnlineLeaderboardWidget*>(leaderboard)->getLocalUser()->getId();
	}
	return true;
}

void GameControll::updateRandomGenerator(int seed)
{
	if(instance.r)
	{
		delete instance.r;
	}
	instance.r = new QRandomGenerator(seed);
	if(instance.board)
	{
		instance.board->updateRandomGenerator(seed + 1);
	}
}

/*!
 * \brief GameControll::resetForNextUser reset after failed try and loads next user
 */
void GameControll::resetForNextUser()
{
	User* user = getNextUser(activeUserID); //Liste ist bereits sortiert (siehe oben), daher ist der nächste User in der Liste der User mit dem nächsthöheren Bidding
	Q_ASSERT_X(user,"GameControll::resetForNextUser","User is nullptr");
	showGuide({ tr("Present your solution, ") + user->getName() + "[]",tr("Your turn, ") + user->getName() + "[]" });
	setActiveUserID(user->getId()); //Setze nächsten Spieler als aktiv
	instance.leaderboard->updatePlayerInPower(user->getId(), user->getName());
	emit enableActionBtn(localUserIsActiveUser());
	getBoard()->revertToBeginning(); //Setze Spielerpositionen zurück
	emit updateMoves(0);
}

int GameControll::getUserIndexById(QUuid id)
{
	for(int i=0; i<instance.users.size(); i++)
	{
		if(instance.users.at(i)->getId()==id)
		{
			return i;
		}
	}
	return -1;
}

/*!
 * \brief GameControll::getNextUser
 * \param lastUserId
 * \return
 * \warning works ONLY if the users are sorted by bidding before the method call
 */
User* GameControll::getNextUser(QUuid lastUserId) //TODO fix leaving clients
{
	for(int i=0; i<users.size(); i++)
	{
		if(users.at(i)->getId() == lastUserId)
		{
			if(i+1<users.size() && users.at(i+1)->getHasBid())
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

/*!
 * \brief GameControll::sortBy
 * \param strategy
 * This function sorts the users in the backend by a predefined Strategy:
 * BID: The users are being sorted in ascending order based on the bid they've sent
 * POINTS: The users are being sorted in descending order based on the number of points
 * It's important to note that the users are also being sorted according to the time they've sent their move, if two players bid the same thing, the player that has bid first will come on first
 * so it's a FIFO-Scheme.
 */
void GameControll::sortBy(strategy strategy)
{
	QVector<User*> sortedUsers;
	unsigned long minTimeStamp = QDateTime::currentMSecsSinceEpoch();
	QBitArray isActive(instance.users.size());
	for(int i = 0; i<instance.users.size(); i++)
	{
		isActive[i] = 1; //Set all users to be active
	}
	if(strategy == bid)
	{
		User * minUser = nullptr;
		int minIndex = 0;
		int minBid;
		for(int additionIndex = 0; additionIndex < instance.users.size(); additionIndex++)
		{
			minBid = User::maxBid;
			minTimeStamp = QDateTime::currentMSecsSinceEpoch();
			for(User* user : qAsConst(instance.users))
			{
				if(user->getBidding() <= minBid && isActive[instance.users.indexOf(user)])//If User has a lower bid than the currently lowest bid
				{
					if(user->getBidding() == minBid)
					{
						if(user->getTimeStamp() <= minTimeStamp) //check the timestamp
						{
							minUser = user; //Set the Widget to add to the new list to the user
							minIndex = instance.users.indexOf(user); //Set the index needed for deactivating the user to the current index
							minBid = user->getBidding(); //Set the newest lowest bid to the current user as there can be users after that one with lower bids
							minTimeStamp = user->getTimeStamp(); //Set User Timestamp to the current user value
						}
					}
					else
					{
						minUser = user; //Set the Widget to add to the new list to the user
						minIndex = instance.users.indexOf(user); //Set the index needed for deactivating the user to the current index
						minBid = user->getBidding(); //Set the newest lowest bid to the current user as there can be users after that one with lower bids
						minTimeStamp = user->getTimeStamp(); //Set User Timestamp to the current user value
					}
				}
			}
			isActive[minIndex] = false; //Deactivate user
			sortedUsers.append(minUser);
		}
		instance.users = sortedUsers;
	}
	if(strategy == points)
	{
		User * maxUser = nullptr;
		int maxIndex = 0;
		int maxPts;
		for(int additionIndex = 0; additionIndex < instance.users.size(); additionIndex++)
		{
			maxPts = 0;
			for(User * user : qAsConst(instance.users))
			{
				if(user->getPoints() >= maxPts && isActive[instance.users.indexOf(user)])//If User has more points than the current maximum
				{
					maxUser = user; //Set the Widget to add to the new list to the user
					maxIndex = instance.users.indexOf(user); //Set the index needed for deactivating the user to the current index
					maxPts = user->getPoints(); //Set the newest largest number of points to the current user as there can be users after that one with larger number of points
					minTimeStamp = user->getTimeStamp(); //Set the User Time stamp to the current user value
				}
			}
			isActive[maxIndex] = false; //Deactivate user
			sortedUsers.append(maxUser);
		}
		instance.users = sortedUsers;
	}

	if(instance.getSettingsDialog()->getFairModeOn() && (Server::isActive() || Client::isActive())){
		if(getLocalUser()->getId() == instance.users.at(0)->getId()){
			QString path = QDir::currentPath();
			#if QT_VERSION_MAJOR == 5
				player->setMedia(QUrl::fromLocalFile(path + "/../Robospiel/Sounds/count.mp3"));
				player->setVolume(50);
			#elif QT_VERSION_MAJOR == 6
				player->setAudioOutput(audioOutput);
				player->setSource(QUrl::fromLocalFile(path + "/../Robospiel/Sounds/count.mp3"));
				audioOutput->setVolume(50);
			#endif

			player->play();
		}
	}
	else{
		player->stop();
	}

	instance.leaderboard->updateAllUsers();
}

/*!
 * \brief GameControll::addUser called by exeQTActionWithData when the action is newUser
 * \param user
 */
void GameControll::addUser(User* user)
{
	for(User * u: qAsConst(instance.users))
	{
		if(u->getId() == user->getId())
		{
			delete user;
			return;
		}
	}
	for(User* u: instance.users)
	{
		u->setVotekick(user->getId(), false);
	}
	instance.users.append(user);
	instance.leaderboard->addUser(user);
	instance.leaderboard->updateAllUsers();
	instance.updateVoteNumbers();

	//See comment in GameControll::initializeUser
	if(getLocalUser() && user->getId()==getLocalUser()->getId())
	{
		static_cast<OnlineLeaderboardWidget*>(instance.leaderboard)->setLocalUser(user);
	}
}

/*!
 * \brief GameControll::calculateWinner This function is being called when the first player has reached their goal
 */
void GameControll::calculateWinner()
{
	QUuid activeUserId = getActiveUserID();
	User* activeUser = getUserById(activeUserId);

	activeUser->incrementPoints();
	const QString username = activeUser->getName();
	showGuide({ tr("Goal has been hit by %1").arg(username) + "[2000]" + "[]"}); //TODO: because of reasons this text is not shown
	switchPhase(Phase::freeplay);
}

/*!
 * \brief GameControll::getUserById get pointer to User from UserId returns nullptr if user not found
 * \param id
 * \return
 */
User* GameControll::getUserById(QUuid id)
{
	for(User* u: qAsConst(instance.users))
	{
		if(u->getId()==id)
		{
			return u;
		}
	}
	//Q_ASSERT_X(false,"GameControll::getUserById","User not found");
	qDebug() << "GameControll::getUserById User not found";
	return nullptr; //nullptr return used in initialization of OnlineLeaderboard. See comment in GameControll::initializeUser.
}

void GameControll::changeBidding(int bidding, QUuid id)
{
	for (User *u: qAsConst(users))
	{
		if (u->getId() == id)
		{
			u->setBidding(bidding);
			break;
		}
	}
	Q_ASSERT_X(false,"GameControll::changeBidding","User not found");
}

/*!
 * \brief GameControll::initializeUser current user of the system is initialised (only when server or client starts)
 * \return
 */
User * GameControll::initializeUser(User* u)
{
	if(!u)
	{
		u  = new User(instance.getSettingsDialog()->getUsername(), instance.getSettingsDialog()->getUsercolor());
	}
	//emit instance.enableServerSwitchBtn(instance.localUserIsServer());

	triggerActionWithData(PlayerAction::registerClient, u->toJSON());
	triggerActionWithData(PlayerAction::newUser, u->toJSON());

	/*
	Problem: The new User is transmitted via JSON -> is a different object from u.
	The local user in OnlineLeaderboardWidget and the user in Gamecontroll::users should be the same object.

	If this is in the server, the triggerActionWithData is synchronous, the new local user can be found in the if-statement below
	and the new object can be used for initializing the Onlineleaderboard.

	If this is in a client, the newUser-player action takes time to be executed. In this case, the replacement is handled in GameControll::addUser.
	 */

	if(instance.getUserById(u->getId()))
	{
	   return instance.getUserById(u->getId());
	}
	return u;
}

/*!
 * \brief GameControll::setLeaderboard
 * \param value
 */
void GameControll::setLeaderboard(LeaderBoardWidget * value)
{
	instance.leaderboard = value;
	connect(&GameControll::getInstance(), &GameControll::biddingDone, &GameControll::getInstance(), [=]()
	{
		instance.leaderboard->deactivateInput();
		instance.sortBy(bid);
		User * user = instance.users.first();
		Q_ASSERT_X(user,"GameControll::setLeaderboard","User is nullptr");
		instance.setActiveUserID(user->getId());
		instance.leaderboard->updatePlayerInPower(user->getId(), user->getName());
		const QString& username = user->getName();
		showGuide({ tr("Present your solution, ") + username + "[]",tr("Your turn, ") + username + "[]" });

	});
	connect(instance.leaderboard, &LeaderBoardWidget::biddingAccepted, &GameControll::getInstance(), [=](QUuid userId, int bidding)
	{
		triggerActionWithData(PlayerAction::sendBidding, {{"userId",userId.toString()},{"bidding",bidding}});
	});
	connect(instance.leaderboard, &LeaderBoardWidget::userWasClicked, &GameControll::getInstance(), [&](QUuid userId)
	{
		triggerActionWithData(PlayerAction::changeActiveUser, {{"userId",userId.toString()}});
	});

	instance.switchPhase(instance.currentPhase); //correct initializiation, can't happen earlier, because MainWidget has to be initialized first
}

const User * GameControll::getMinBid()
{
	const User * minBid = instance.users.first();
	for(const User * u:qAsConst(instance.users))
	{
		if(u->getBidding() < User::maxBid)
		{
			if(*u < minBid)
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
	board->setCurrentToSavedState();
	if(switchPhase(Phase::search))
	{
		sortBy(points);
		//reset all biddings
		for(User* u: qAsConst(users))
		{
			u->resetVotekick();
			u->setBidding(User::maxBid);
			leaderboard->newRound();
		}
		emit updateMoves(0);
		leaderboard->activateInput();
		killSolver();
		board->startNewRound();
		if(localUserIsServer())
		{
			solverThread = QThread::create(GameControll::solveBoard,getInstance().board,&getInstance().solution);
			connect(instance.solverThread,&QThread::finished,&instance,&GameControll::presentSolution);
			solverThread->start();
			qDebug() << "Started Thread" << solverThread;
		}
	}
}

void GameControll::setPhase(GameControll::Phase phase) //TODO: once it turns out the phases word like this (with switchPhase) please delete all the commented sections and move "currentPhase = phase;" and "updateVoteNumbers();" before the switch-case
{
	switch(phase)
	{
	case Phase::idle:
	{
		currentPhase = phase;
		updateVoteNumbers();
		instance.leaderboard->noPlayerInPower(); //TODO: Am I breaking anything with this QUuid construction? I basically just need a null there...
		instance.leaderboard->setFreeplayButtonsVisible(false);
		showGuide({tr("boooring")+ "[]",tr("i am not creative")+ "[2000]" + tr("at all")+ "[2000]" + tr("fuck you") + "[]", tr("We are in idle now!")+ "[]", tr("Lets do some idling!")+ "[]", tr("Okay, so you aren't capable of dealing with a real mode, are you?")+ "[2000]" +tr("We are in idle.")+ "[]", tr("Too dumb for a real game!")+ "[2000]" +tr("We are in idle.")+ "[]", tr("Idle again? Are we ever going to PLAY?")+ "[2000]" +tr("We are in idle.")+ "[]"});
		emit enableIdleBtn(false);
		emit enableMenus(true);
		instance.hasSkipped = 0;
		settings->enableTimerChange(true);
		break;
	}
	case Phase::search:
	{
		//emit enableServerSwitchBtn(localUserIsServer());
		currentPhase = phase;
		updateVoteNumbers();
		instance.leaderboard->noPlayerInPower();
		instance.leaderboard->setFreeplayButtonsVisible(false);
		instance.leaderboard->setBiddingFocus();
		showGuide({tr("Start bidding")+ "[]",tr("Let's go! Bid!")+ "[]", tr("You can bid now!")+ "[]",  tr("Lets do some bidding!")+ "[]", tr("I bet you wont find anything! But you can try to...")+ "[2000]" +tr("Make your biddings!")+ "[]", tr("Make your biddings! Well if you find anything...")+ "[]"});
		emit enableIdleBtn(false);
		if(Server::isActive())
		{
			emit enableIdleBtn(true);
		}
		emit enableMenus(false);
		instance.hasSkipped = 0;
		break;
	}
	case Phase::countdown:
	{
		currentPhase = phase;
		updateVoteNumbers();
		instance.leaderboard->noPlayerInPower();
		instance.leaderboard->setBiddingFocus();
		showGuide({tr("Counting down")+ "[]", tr("Stressed yet? The Timer is running!")+ "[]", tr("You will never find anything in a minute!")+ "[]" });
		emit enableIdleBtn(false);
		emit enableMenus(false);
		instance.hasSkipped = 0;
		settings->enableTimerChange(false);
		break;
	}
	case Phase::presentation:
	{

		currentPhase = phase;

		//voteCounter=0; //not used. Fill with different values to implement (voting to skip the presentation phase)
		updateVoteNumbers();

		//Set Player to player with minimum bid, aka first player after being sorted
		emit biddingDone();
		emit enableIdleBtn(false);
		emit enableMenus(false);
		instance.hasSkipped = 0;
		emit focusBoard();
		emit enableActionBtn(localUserIsActiveUser());
		settings->enableTimerChange(true);
		break;
	}
	case Phase::freeplay:
	{
		//TODO: Set user who just made a point as active user.
		currentPhase = phase;
		letUserPlayFree(activeUserID);
		updateVoteNumbers();
		instance.leaderboard->setFreeplayButtonsVisible(localUserIsActiveUser());
		showGuide({tr("Freeplay")+ "[2000]"+ tr("time to show off")+ "[]"});
		emit enableMenus(false);
		emit enableIdleBtn(true);
		instance.hasSkipped = 0;
		emit focusBoard();
		emit enableActionBtn(true); // TODO: does this make sense here?
		break;
	}
	}
	player->stop();
}

bool GameControll::switchPhase(GameControll::Phase phase) //TODO: once it turns out the phases word like this (with setPhase) please delete all the commented sections
{
	switch(phase)
	{
	case Phase::idle:
	{

		if(currentPhase == Phase::freeplay || currentPhase == Phase::search)/* || currentPhase == Phase::idle*/ //I allow going to idle from search here, but this is only supposed to work when the server wants it, this should be managed in switchPhase be disabling the button for everyone else
		{
			resetVotes();
			setPhase(phase);
			return true;
		}
		break;
	}
	case Phase::search:
	{
		if(currentPhase == Phase::freeplay || currentPhase == Phase::idle)/* || currentPhase == Phase::search*/
		{
			resetVotes();
			setPhase(phase);
			return true;
		}
		break;
	}
	case Phase::countdown:
	{
		if(currentPhase == Phase::search)
		{
			timeLeft = searchTime; //60
			emit time(timeLeft);
			countdown.start();
			resetVotes();
			setPhase(phase);
			return true;
		}
		break;
	}
	case Phase::presentation:
	{
		if(currentPhase == Phase::countdown)
		{
			resetVotes();
			setPhase(phase);
			return true;
		}
		break;
	}
	case Phase::freeplay:
	{
		if(currentPhase == Phase::presentation)
		{
			//Save the state that the game had right when the point was scored.
			/*Don't put this in setPhase:
			When a client joins, only setPhase is called.
			If saveCurrentPositionOfPlayers would be called, this would overwrite data with the freeplay-state.
			For the next target, the board would be reset to a too late version of the board (possibly from the middle of freeplay phase)*/
			board->saveCurrentPositionOfPlayers();

			//TODO: muss hier voteCounter=0 rein?
			setPhase(phase);
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

void GameControll::letUserPlayFree(const QUuid & userId)
{
	if(currentPhase == Phase::freeplay)
	{
		// set user as active user.
		setActiveUserID(userId);
		// show active user in UI
		instance.leaderboard->updatePlayerInPower(userId, getUserById(userId)->getName());
		instance.leaderboard->setFreeplayButtonsVisible(localUserIsActiveUser());
		// reset the board to the state in the last search phase
		board->revertToBeginning();
		// Set the steps counter to 0
		emit updateMoves(0);
	}
}

void GameControll::setMapping(QVector<KeyMapping*> mapping)
{
	this->mapping = mapping;
}

Board * GameControll::getBoard()
{
	return instance.board;
}

QUuid GameControll::getActiveUserID()
{
	return activeUserID;
}

void GameControll::setActiveUserID(const QUuid & id)
{

	activeUserID = id;
}

void GameControll::saveBoard()
{
	QDir saveDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
	if(!saveDir.exists())
	{
		saveDir.mkpath(saveDir.path());
	}
	QFile boardfile(QFileDialog::getSaveFileName(nullptr,"Board speichern",saveDir.absolutePath(),"*.brd"));
	if(boardfile.open(QIODevice::WriteOnly))
	{
		boardfile.write(instance.board->toBinary().toUtf8());
		boardfile.close();
	}
}

void GameControll::loadBoard()
{
	QDir saveDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
	if(!saveDir.exists())
	{
		saveDir.mkpath(saveDir.path());
	}
	QFile boardfile(QFileDialog::getOpenFileName(nullptr,"Board laden",saveDir.absolutePath(),"*.brd"));
	if(boardfile.open(QIODevice::ReadOnly))
	{
		QString data = QString::fromUtf8(boardfile.readAll());
		boardfile.close();
		GameControll::triggerAction(PlayerAction::blockBoard);
		setBoard(Board::fromBinary(data));
		GameControll::triggerActionWithData(PlayerAction::editBoard,{{"board", instance.board->toBinary()}});
		GameControll::triggerActionWithData(PlayerAction::syncRandomGenerators,{{"Seed",QTime::currentTime().msecsSinceStartOfDay()}});
	}
}

void GameControll::presentSolution()
{
	qDebug() << "solved in " << solution.length() << " moves!!!";
	for(ZugKnoten::Zug zug:solution)
	{
		QString string;
		switch(zug.d)
		{
			case Direction::none:
				string="ERROR";
				break;
			case Direction::north:
				string="north";
				break;
			case Direction::east:
				string="east";
				break;
			case Direction::south:
				string="south";
				break;
			case Direction::west:
				string="west";
				break;
		}
		qDebug() << zug.player << string;
	}
}

LeaderBoardWidget* GameControll::getLeaderboard()
{
	return instance.leaderboard;
}

void GameControll::updateTimer()
{
	if(--timeLeft <= 0)
	{
		endTimer();
	}
	emit time(timeLeft);
}

/*!
 * \brief GameControll::endTimer skips timer
 */
void GameControll::endTimer()
{
	if(instance.currentPhase == Phase::countdown)
	{
		emit instance.time(0);
		instance.countdown.stop();
		instance.switchPhase(Phase::presentation);
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

void GameControll::addDefaultUsers()
{
	QJsonObject anna = QJsonObject();
	anna.insert("name", "Annalena");
	anna.insert("id",QUuid::createUuid().toString());
	anna.insert("color", "#5CAD52");
	triggerActionWithData(PlayerAction::newUser, anna);
	QJsonObject doro = QJsonObject();
	doro.insert("name", "Dorothee");
	doro.insert("id",QUuid::createUuid().toString());
	doro.insert("color", "#8A1BC1");
	triggerActionWithData(PlayerAction::newUser, doro);
	QJsonObject jan = QJsonObject();
	jan.insert("name", "Jan");
	jan.insert("id",QUuid::createUuid().toString());
	jan.insert("color", "#FF7300");
	triggerActionWithData(PlayerAction::newUser, jan);
	QJsonObject luca = QJsonObject();
	luca.insert("name", "Luca");
	luca.insert("id",QUuid::createUuid().toString());
	luca.insert("color", "#FF6700");
	triggerActionWithData(PlayerAction::newUser, luca);
	QJsonObject nora = QJsonObject();
	nora.insert("name", "Nora");
	nora.insert("id",QUuid::createUuid().toString());
	nora.insert("color", "#2A656B");
	triggerActionWithData(PlayerAction::newUser, nora);
}

void GameControll::clearUsers()
{
	while(!instance.users.isEmpty())
	{
		delete instance.users.takeFirst();
	}
}

void GameControll::showGuide(const QStringList & texts)
{
	QString text = texts.at(instance.r->bounded(texts.size())); //TODO: synchronize? (...d?)
	assert(text.endsWith("]"));
	static QRegularExpression regex("[\\[\\]]");
	QStringList list = text.split(regex,Qt::KeepEmptyParts);
	list.removeLast();
	assert(list.size() % 2 == 0);
	for(int i = 0;i < list.size() - 1;i += 2)
	{
		bool ok = false;
		int duration = list.at(i+1).toInt(&ok);
		if(!ok)
		{
			duration = 1000;
		}

		instance.guideList.append({phaseAsString(instance.currentPhase)+": "+ list.at(i),duration});
	}
	instance.nextGuide();
}

QString GameControll::getLocalUserName()
{
	if(Server::isActive() || Client::isActive())
	{
		User * u = static_cast<OnlineLeaderboardWidget*>(instance.leaderboard)->getLocalUser();
		if(u)
		{
			return u->getName();
		}
	}
	return "Client";
}

User* GameControll::getLocalUser()
{
	if(Server::isActive() || Client::isActive())
	{
		return static_cast<OnlineLeaderboardWidget*>(instance.leaderboard)->getLocalUser();
	}
	Q_ASSERT_X(false, "GameControll::getLocalUser", "no local user in gamecontroll");
	//nur zum debuggen.
	if(Server::isActive() || Client::isActive())
	{
		return static_cast<OnlineLeaderboardWidget*>(instance.leaderboard)->getLocalUser();
	}
	return nullptr;

}

void GameControll::nextGuide()
{
	if(!guideList.isEmpty())
	{
		const GuideLine & gl = guideList.takeFirst();
		emit updateGuide(gl.line);
		guideTimer.start(gl.duration);
	}
}

void GameControll::syncBoard()
{
	triggerActionWithData(PlayerAction::editBoard,{{"board",instance.board->toBinary()}});
}

QVector<User*>* GameControll::getUsers()
{
	return &instance.users;
}

GameControll::functionPointer GameControll::getActionWhenAnimationEnded()
{
	return instance.actionWhenAnimationEnded;
}

void GameControll::setActionWhenAnimationEnded(functionPointer function)
{
	instance.actionWhenAnimationEnded=function;
}

void GameControll::disableAnnoyingSounds()
{
	instance.player->stop();
	instance.hasSkipped = 1;
}

int GameControll::getVoteCounter()
{
	return instance.voteCounter;
}

int GameControll::getVoteThreshold()
{
	return instance.voteThreshold;
}

void GameControll::updateVoteNumbers()
{
	//get current thresholds
	switch(currentPhase)
	{
	case Phase::idle:
		voteThreshold = users.length(); //all players have to agree to start playing again
		break;
	case Phase::search:
		voteThreshold = ceil(users.length()*1.0/2); //half of the players have to agree to skip the current target
		break;
	case Phase::countdown:
		voteThreshold = users.length(); //all players have to agree to skip the remaining countdown
		break;
	case Phase::presentation:
		voteThreshold = users.length();
		break;
	case Phase::freeplay:
		voteThreshold = users.length(); //all players have to agree to start playing again
		break;
	}
	voteThreshold = std::max(voteThreshold, 1);
	emit updateActionButtonText();
	//TODO: There is one case where this function is called (because of a new user?) and the emit updateActionButtonText(); leads to the "GIVE UP" String in presentation being set enabled, though it shouldn't be. Checking this here is super ugly, but I don't know where else...
	if(currentPhase == Phase::presentation && !localUserIsActiveUser())
	{
		emit enableActionBtn(false);
	}

	//check for consequences
	switch (currentPhase)
	{
	case Phase::countdown: //voting to skip the remaining countdown phase
	{
		if(Server::isActive()||Client::isActive())
		{
			QString path = QDir::currentPath();
			#if QT_VERSION_MAJOR == 5
				player->setMedia(QUrl::fromLocalFile(path + "/../Robospiel/Sounds/rick.mp3"));
				player->setVolume(50);
			#elif QT_VERSION_MAJOR == 6
				player->setAudioOutput(audioOutput);
				player->setSource(QUrl::fromLocalFile(path + "/../Robospiel/Sounds/rick.mp3"));
				audioOutput->setVolume(50);
			#endif

			if(voteCounter==voteThreshold-1 && !instance.hasSkipped && voteCounter>=1)
			{
				player->play();
			}
			if(voteCounter>=voteThreshold)
			{
				endTimer();
				player->stop();
			}
		}
		else
		{
			endTimer();
		}
		break;
	}
	case Phase::search:
	case Phase::freeplay:
	case Phase::idle: //voting to start new round
		//Aussagenlogik: online ->VC>=VT ist !online v VC>=vT
		if(!(Server::isActive()||Client::isActive()) || (voteCounter>=voteThreshold && Server::isActive()))
		{
			GameControll::triggerAction(PlayerAction::setIdle);
			GameControll::triggerAction(PlayerAction::nextTarget);
		}
		break;
	case Phase::presentation:
		break;
	}

}

/**
 * @brief GameControll::initiateServerSwitch
 * @param restart determines if the old server reconnects after the switch
 * @return true if a serverSwitch was performed
 */
bool GameControll::initiateServerSwitch(bool restart)
{
	if(instance.localUserIsServer())
	{
		Server::switchServer(restart);
		return true;
	}
	return false;
}

bool GameControll::localUserIsServer()
{
	return Server::isActive();
}

bool GameControll::localUserIsClient()
{
	return Client::isActive();
}

void GameControll::resetVotes()
{
	voteCounter=0;
	for(User* u : users)
	{
		u->setHasVoted(false);
	}
}

void GameControll::solveBoard(Board* board, QVector<ZugKnoten::Zug>* solution)
{
	Solver s;
	s.solveBoard(board,*solution);
}

void GameControll::killSolver()
{
	QThread* thread = solverThread;
	if(thread)
	{
		if(thread->isRunning())
		{
			thread->terminate();
			qDebug() << "waiting";
			thread->wait(QDeadlineTimer::Forever);
			qDebug() << "waiting finished";
		}
		qDebug() << "killed" << thread << solverThread;
		thread->deleteLater();
		if(solverThread == thread)
		{
			solverThread = nullptr;
		}
	}
}

QString GameControll::phaseAsString(Phase phase)
{
	return QStringList({tr("idle"),tr("search"), tr("countdown"), tr("presentation"),tr("freeplay")}).at(static_cast<int>(phase));
}

bool GameControll::votekickActive()
{
	return (Server::isActive() || Client::isActive()) && instance.users.size()>=2;
}

void GameControll::requestDisconnect()
{
	emit instance.disconnectFromServer();
}
