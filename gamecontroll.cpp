#include <QJsonObject>
#include <QUuid>
#include "gamecontroll.h"
#include "server.h"
#include "client.h"
#include "user.h"
#include "onlineleaderboardwidget.h"

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
	connect(&guideTimer,&QTimer::timeout,this,&GameControll::nextGuide);
	r = new QRandomGenerator(QTime::currentTime().msecsSinceStartOfDay());
	player = new QMediaPlayer;

}

/*!
 * \brief GameControll::initializeConnections relies on Server and Client singletons.
 * Initialization of static variables can happen in random order, so this has to wait, until all static variables are present.
 */
void GameControll::initializeConnections()
{
	connect(&Server::getInstance(), &Server::actionReceived, &GameControll::getInstance(), &GameControll::exeQTAction);
	connect(&Client::getInstance(), &Client::actionReceived, &GameControll::getInstance(), &GameControll::exeQTAction);
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
	instance.voteCounter=json.value("voteCounter").toInt();
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
		instance.actionWhenAnimationEnded=nullptr;
		break;
	case 1:
		instance.actionWhenAnimationEnded=&GameControll::calculateWinner;
		break;
	case 2:
		instance.actionWhenAnimationEnded=&GameControll::resetForNextUser;
		break;
	case 3:
		instance.actionWhenAnimationEnded=&GameControll::resetAndNextTarget;
		break;
	}
}

/*!
 * \brief GameControll::sendToServerWithData forwards the message to all clients / triggers the action directly when you are offline
 * \param a Playeraction
 * \param info JSON Object
 */
void GameControll::sendToServerWithData(PlayerAction a, QJsonObject info)
{
	qDebug()<<"Send to server with data" << a << info;
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
	connect(instance.settings, &SettingsDialog::usercolorChanged, &instance, [=](QColor color){
		if(Server::isActive() || Client::isActive())
		{
			triggerActionWithData(PlayerAction::changedUserColor, {{"id", getLocalUser()->getId().toString()},{"color", color.name()}});
		}

	});
	connect(instance.settings, &SettingsDialog::usernameChanged, &instance, [=](QString name){
		if(Server::isActive() || Client::isActive())
		{
			triggerActionWithData(PlayerAction::changedUsername, {{"id", getLocalUser()->getId().toString()},{"name", name}});
		}
	});

	connect(instance.settings, &SettingsDialog::timertimeChanged, &instance, [=](int length){
		if(Server::isActive())
		{
			triggerActionWithData(PlayerAction::changedTimerTime, {{"length", length}});
		}
	});


	if(instance.board)
	{
		instance.board->updateColors(instance.settings->getBackground(), instance.settings->getWallcolor(), instance.settings->getGridcolor(), instance.settings->getPlayerColorLow(), instance.settings->getPlayerColorHigh());
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
	if(instance.board)
	{
		instance.board->deleteLater();
	}
	instance.board = newBoard;
	if(instance.settings)
	{
		instance.board->updateColors(instance.settings->getBackground(), instance.settings->getWallcolor(), instance.settings->getGridcolor(), instance.settings->getPlayerColorLow(), instance.settings->getPlayerColorHigh());
	}
	connect(instance.board, &Board::playerMoved, &GameControll::getInstance(), [&](int, int) // Please note that this connect has to come BEFORE the one that is below, otherwise we will have problem with reverting to the thrid to last positions once the steps are all used up :(!
	{
		emit instance.updateMoves(instance.board->getMoves());
	});
	connect(instance.board, &Board::playerMoved, &GameControll::getInstance(), &GameControll::calculateGameStatus); // Please note that this connect has to come AFTER the one that is above!
	emit instance.newBoard(instance.board);
	return instance.board;
}

/*!
 * \brief GameControll::exeQTAction executes different actions, because they were send by the server / triggered directly when you are offline
 * \param data
 */
void GameControll::exeQTAction(QJsonObject data)
{
	qDebug() << "GameControll::exeQTAction(QJsonObject " << data << ")";
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
	case newUser:
	{
		user = User::fromJSON(data);
		addUser(user);
		break;
	}
	case changeActiveUser:
	{
		letUserPlayFree(data.value("userId").toString());
		break;
	}
	case completeUpdate:
	{
		adaptFromJSON(data);
		break;
	}
	case editBoard:
	{
		//disableBoard();//TODO
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
		voteCounter++;
		emit updateActionButtonText();

		switch (currentPhase) {
		case Phase::countdown: //voting to skip the remaining countdown phase
		{
			if(Server::isActive()||Client::isActive())
			{
				QString path = QDir::currentPath();
				qDebug()<<path;
				player->setMedia(QUrl::fromLocalFile(path + "/../Robospiel/Sounds/rick.mp3"));
				player->setVolume(50);
				if(voteCounter==voteThreshold-1 && !instance.hasSkipped)
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
			//Aussagenlogik: online ->vC>=vT ist !online v vC>=vT
			if(!(Server::isActive()||Client::isActive()) || voteCounter>=voteThreshold)
			{
				GameControll::triggerAction(PlayerAction::nextTarget);
			}
			break;
		case Phase::presentation:
			break;


		}
		break;
	}
	case PlayerAction::nextTarget:
	{
		nextTarget();
		break;
	}
	case userLeft:
	{
		user = User::fromJSON(data.value("user").toObject());

		//TODO fix skip


		for(int i=0; i<users.size(); i++)
		{
			if(users.at(i)->getId()==user->getId())
			{
				if(activeUserID==user->getId())
				{
					board->setMoves(INT32_MAX);
					calculateGameStatus();
				}

				users.removeAt(i);
				//delete
				leaderboard->updateAllUsers();
				break;
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
		break;
	}
	case changedTimerTime:
	{
		searchTime = data.value("length").toInt();
		break;
	}
	case giveUp:
	{
		GameControll::getInstance().handleUserGivingUp();
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
	qDebug()<<"Called function TriggerAction with parameters "<<action;
	if(instance.localUserIsActiveUser() && action & PlayerAction::movement)
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
			qDebug()<<"playerSwitch is called in triggerAction with isRevert=False!";
			emit instance.actionTriggeredWithData(action, {{"isRevert", false}});
			return;
		}
	}
	else if(action & PlayerAction::bidding) //TODO submit biddingValue
	{
		Q_ASSERT_X(false, "GameControll::triggerAction", "use triggerActionWithData to send biddings!");
		//qDebug()<<"Currently in GameControl: triggerAction -> bidding, current Phase is "<<static_cast<int>(instance.currentPhase);
		//if(instance.currentPhase == Phase::search || instance.currentPhase == Phase::countdown)
		//{
		//	emit instance.actionTriggered(action);
		//	return;
		//}
	}
	else if(action & PlayerAction::other) //TODO overly complicated ifs? Remove or explain!
	{
		if(instance.currentPhase == Phase::presentation || instance.currentPhase == Phase::freeplay || action == PlayerAction::vote)
		{
			emit instance.actionTriggered(action);
			return;
		}
	}
	else if(action & PlayerAction::menuAction)
	{
		emit instance.actionTriggered(action);
		return;
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
	qDebug() << "GameControll::triggerActionWithData(PlayerAction " << action << ", QJsonObject " << data << ")";
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
	default:
		break;
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
			if(board->getMoves() >= getUserById(activeUserID)->getBidding())
			{
				//TODO: Flag um anzuzeigen, dass der Spieler das Ziel erreicht hat?
				qDebug()<<"User couldn't end the round in the specified bid of "<< getUserById(activeUserID)->getBidding()<<", the next user is being drawn";
				GameControll::triggerAction(revert);
				User * user = instance.users.first();
				const QString& username = user->getName();
				showGuide({tr("You are out of steps, looser!")+ "[]", tr("Getting frustrated? Maybe pay attention next time you bit!")+ "[]", tr("Buhuuu! All your steps are up, ") + username + "!" + "[]", tr("No steps, no luck, I guess, your stuck!")+ "[]", tr("Somebody doesn't know how to count!")+ "[]"});
			}
		}
	}
}

void GameControll::handleUserGivingUp(){
	User* nextUser = getNextUser(activeUserID);
	if(nextUser)//Not at last player yet, noch haben nicht alle versagt
	{
		GameControll::resetForNextUser();

		qDebug()<<"actionWhenAnimationEnded = resetFornextuser";
	}
	else //Alles Versager
	{
		qDebug()<<"No User could end the round in their specified bid.";
		switchPhase(Phase::freeplay);
		//GameControll::resetAndNextTarget();
		qDebug()<<"actionWhenAnimationEnded = resetAndNextTarget";
	}
	//if(nextUser)//Not at last player yet, noch haben nicht alle versagt
	//{
	//    actionWhenAnimationEnded = &GameControll::resetForNextUser;

	//    qDebug()<<"actionWhenAnimationEnded = resetFornextuser";
	//}
	//else //Alles Versager
	//{
	//    qDebug()<<"No User could end the round in their specified bid.";
	//    actionWhenAnimationEnded = &GameControll::resetAndNextTarget;
	//    qDebug()<<"actionWhenAnimationEnded = resetAndNextTarget";
	//}
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
	qDebug()<<"Acquiring next User";
	User* user = getNextUser(activeUserID); //Liste ist bereits sortiert (siehe oben), daher ist der nächste User in der Liste der User mit dem nächsthöheren Bidding
	Q_ASSERT_X(user,"GameControll::resetForNextUser","User is nullptr");
	showGuide({ tr("Present your solution, ") + user->getName() + "[]",tr("Your turn, ") + user->getName() + "[]" });
	setActiveUserID(user->getId()); //Setze nächsten Spieler als aktiv
	emit enableActionBtn(localUserIsActiveUser());
	getBoard()->revertToBeginning(); //Setze Spielerpositionen zurück
	emit updateMoves(0);
	qDebug() << "Active User is now " <<user->getName();
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
	bool isActive[instance.users.size()];
	for(int i = 0; i<instance.users.size(); i++)
	{
		isActive[i] = 1; //Set all users to be active
	}
	if(strategy == bid)
	{
		//qDebug()<<"Called sortByBidding";
		User * minUser = nullptr;
		int minIndex = 0;
		int minBid;
		for(int additionIndex = 0; additionIndex < instance.users.size(); additionIndex++)
		{
			//qDebug()<<"AdditionIndex: "<<additionIndex;
			minBid = User::maxBid;
			minTimeStamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
			for(User* user : qAsConst(instance.users))
			{
				//qDebug()<<"SortByBidding: USER "<<user->getName()<<" with bidding "<<user->getBidding()<<"and timestamp: "<<user->getTimeStamp();
				if(user->getBidding() <= minBid && isActive[instance.users.indexOf(user)])//If User has a lower bid than the currently lowest bid
				{
					//qDebug()<<"User: "<<user->getName()<<" has lower bid than the current Minimum";
					if(user->getBidding() == minBid)
					{
						if(user->getTimeStamp() <= minTimeStamp) //check the timestamp
						{
							//qDebug()<<"Bidding is the same, timestamp is earlier, user is "<<user->getName();
							minUser = user; //Set the Widget to add to the new list to the user
							minIndex = instance.users.indexOf(user); //Set the index needed for deactivating the user to the current index
							minBid = user->getBidding(); //Set the newest lowest bid to the current user as there can be users after that one with lower bids
							minTimeStamp = user->getTimeStamp(); //Set User Timestamp to the current user value
						}
					}
					else
					{
						//qDebug()<<"Bidding of user "<<user->getName()<<" with bidding "<<user->getBidding()<<" is smaller than minimum bid of "<<minBid;
						minUser = user; //Set the Widget to add to the new list to the user
						minIndex = instance.users.indexOf(user); //Set the index needed for deactivating the user to the current index
						minBid = user->getBidding(); //Set the newest lowest bid to the current user as there can be users after that one with lower bids
						minTimeStamp = user->getTimeStamp(); //Set User Timestamp to the current user value
					}
				}
				//qDebug()<<("\n");
			}
			isActive[minIndex] = false; //Deactivate user
			sortedUsers.append(minUser);
		}
		instance.users = sortedUsers;
		for(int i = 0; i<instance.users.size(); i++)
		{
			qDebug()<<"SortedUsers (Bidding): User "<<i<<": "<<instance.users[i]->getName()<<" with points: "<<instance.users[i]->getPoints()<<" and timestamp "<<instance.users[i]->getTimeStamp();
			//isActive[i] = true;
		}
	}
	if(strategy == points)
	{
		//qDebug()<<"Called sortByPoints";
		User * maxUser = nullptr;
		int maxIndex = 0;
		int maxPts;
		for(int additionIndex = 0; additionIndex < instance.users.size(); additionIndex++)
		{
			maxPts = 0;
			for(User * user : qAsConst(instance.users))
			{
				//qDebug()<<"SortByPoints: USER "<<user->getName()<<" with points "<<user->getPoints()<<"and timestamp: "<<user->getTimeStamp();
				if(user->getPoints() >= maxPts && isActive[instance.users.indexOf(user)])//If User has more points than the current maximum
				{
					//qDebug()<<"Points of user "<<user->getName()<<" with points "<<user->getPoints()<<" is larger than maximum amount of "<<maxPts;
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
		for(int i = 0; i<instance.users.size(); i++)
		{
			qDebug()<<"SortedUsers: User "<<i<<": "<<instance.users[i]->getName()<<" with points: "<<instance.users[i]->getPoints()<<" and timestamp "<<instance.users[i]->getTimeStamp();
			//isActive[i] = true;
		}
	}

	if(instance.getSettingsDialog()->getFairModeOn())
	{ // && (Server::isActive() || Client::isActive())){
		qDebug() << "Local User: " << getLocalUser()->getName() << " First User: " << instance.users.at(0)->getName();
		if(getLocalUser() == instance.users.at(0))
		{
			QString path = QDir::currentPath();
			player->setMedia(QUrl::fromLocalFile(path + "/../Robospiel/Sounds/count.mp3"));
			player->setVolume(50);
			player->play();
		}
	}
	else
	{
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
	qDebug() << "GameControll::addUser(User* " << user->getId() << ")" << "(" + user->getName() +")";
	//	if(Client::isActive() || Server::isActive())
	//	{
	//		OnlineLeaderboardWidget * ol = static_cast<OnlineLeaderboardWidget*>(instance.leaderboard);
	//		if(ol->getLocalUser() && ol->getLocalUser()->getId() == user->getId())
	//		{
	//			return;
	//		}
	//	}
	for(User * u: qAsConst(instance.users))
	{
		if(u->getId() == user->getId())
		{
			delete user;
			return;
		}
	}
	instance.users.append(user);
	instance.leaderboard->addUser(user);
	instance.leaderboard->updateAllUsers();
	instance.updateVoteNumbers();
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
	qDebug()<<"User "<<username<<" has successfully ended the round with "<<board->getMoves()<<" moves, their current points are "<<activeUser->getPoints()<<". \nI think the move counter is already reset for the next round at this point...";
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
	for(User* u: qAsConst(users))
	{
		if(u->getId()==id)
		{
			return u;
		}
	}
	Q_ASSERT_X(false,"GameControll::getUserById","User not found");
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
	Q_ASSERT_X(false,"GameControll::changeBidding","User not found");
}

/*!
 * \brief GameControll::initializeUser current user of the system is initialised (only when server or client starts)
 * \return
 */
User * GameControll::initializeUser()
{
	User * u = new User(instance.getSettingsDialog()->getUsername(), instance.getSettingsDialog()->getUsercolor());
	qDebug()<<"initializeUser with id: "<<u->getId();
	triggerActionWithData(PlayerAction::registerClient, u->toJSON());
	triggerActionWithData(PlayerAction::newUser, u->toJSON());
	return u;
}

/*!
 * \brief GameControll::setLeaderboard
 * \param value
 */
void GameControll::setLeaderboard(LeaderBoardWidget * value)
{
	//neu
	instance.leaderboard = value;
	connect(&GameControll::getInstance(), &GameControll::biddingDone, &GameControll::getInstance(), [=]()
	{
		instance.leaderboard->deactivateInput();
		instance.sortBy(bid);
		User * user = instance.users.first();
		Q_ASSERT_X(user,"GameControll::setLeaderboard","User is nullptr");
		instance.setActiveUserID(user->getId());
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
	qDebug()<<"next target!!";

	board->setSavedStateToCurrent();
	if(switchPhase(Phase::search))
	{
		sortBy(points);
		//reset all biddings
		for(User* u: qAsConst(users))
		{
			u->setBidding(User::maxBid);
			leaderboard->newRound();
		}
		emit updateMoves(0);
		leaderboard->activateInput();
		board->startNewRound();
	}
}

void GameControll::setPhase(GameControll::Phase phase){ //TODO: once it turns out the phases word like this (with switchPhase) please delete all the commented sections and move "currentPhase = phase;" and "updateVoteNumbers();" before the switch-case

	switch(phase)
	{
	case Phase::idle:
	{
		currentPhase = phase;
		//voteCounter=0;
		updateVoteNumbers();

		showGuide({tr("boooring")+ "[]",tr("i am not creative")+ "[2000]" + tr("at all")+ "[2000]" + tr("fuck you") + "[]", tr("We are in idle now!")+ "[]", tr("Lets do some idling!")+ "[]", tr("Okay, so you aren't capable of dealing with a real mode, are you?")+ "[2000]" +tr("We are in idle.")+ "[]", tr("Too dumb for a real game!")+ "[2000]" +tr("We are in idle.")+ "[]", tr("Idle again? Are we ever going to PLAY?")+ "[2000]" +tr("We are in idle.")+ "[]"});
		emit enableIdleBtn(false);
		emit enableMenus(true);
		instance.hasSkipped = 0;
		settings->enableTimerChange(true);
		break;
	}
	case Phase::search:
	{
		currentPhase = phase;
		//voteCounter=0;
		updateVoteNumbers();

		instance.leaderboard->setBiddingFocus();
		showGuide({tr("Start bidding")+ "[]",tr("Let's go! Bid!")+ "[]", tr("You can bid now!")+ "[]",  tr("Lets do some bidding!")+ "[]", tr("I bet you wont find anything! But you can try to...")+ "[2000]" +tr("Make your biddings!")+ "[]", tr("Make your biddings! Well if you find anything...")+ "[]"});
		emit enableIdleBtn(false);
		emit enableMenus(false);
		instance.hasSkipped = 0;


		break;
	}
	case Phase::countdown:
	{

		currentPhase = phase;
		//voteCounter=0;
		updateVoteNumbers();

		instance.leaderboard->setBiddingFocus();
		showGuide({tr("Counting down")+ "[]", tr("Stressed yet? The Timer is running!")+ "[]", tr("You will never find anything in a minute!")+ "[]" });
		//        timeLeft = searchTime; //60
		//        emit time(timeLeft);
		//        countdown.start();
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
		/*TODO: Set user who just made a point as active user.
			 Save the state that the game had right when the point was scored.*/
		board->saveCurrentPositionOfPlayers();

		currentPhase = phase;
		letUserPlayFree(activeUserID);

		//voteCounter=0;
		updateVoteNumbers();

		showGuide({tr("Freeplay")+ "[2000]"+ tr("time to show off")+ "[]"});
		emit enableMenus(false);
		emit enableIdleBtn(true);
		instance.hasSkipped = 0;
		emit focusBoard();
		emit enableActionBtn(true); // TODO: does this make sense here?
		break;
	}
	}

}

bool GameControll::switchPhase(GameControll::Phase phase) //TODO: once it turns out the phases word like this (with setPhase) please delete all the commented sections
{
	qDebug() << "GameControll::switchPhase(GameControll::Phase " << static_cast<int>(phase) << ")";
	switch(phase)
	{
	case Phase::idle:
	{

		if(currentPhase == Phase::freeplay || currentPhase == Phase::idle)
		{
			voteCounter=0;
			setPhase(phase);
			//            currentPhase = phase;
			//            voteCounter=0;
			//            updateVoteNumbers();
			//            showGuide({tr("boooring")+ "[]",tr("i am not creative")+ "[2000]" + tr("at all")+ "[2000]" + tr("fuck you") + "[]", tr("We are in idle now!")+ "[]", tr("Lets do some idling!")+ "[]", tr("Okay, so you aren't capable of dealing with a real mode, are you?")+ "[2000]" +tr("We are in idle.")+ "[]", tr("Too dumb for a real game!")+ "[2000]" +tr("We are in idle.")+ "[]", tr("Idle again? Are we ever going to PLAY?")+ "[2000]" +tr("We are in idle.")+ "[]"});
			//            emit enableMenus(true);
			//            instance.hasSkipped = 0;
			//            settings->enableTimerChange(true);
			return true;
		}
		break;
	}
	case Phase::search:
	{
		if(currentPhase == Phase::freeplay || currentPhase == Phase::idle || currentPhase == Phase::search)
		{
			voteCounter=0;
			setPhase(phase);
			//            currentPhase = phase;

			//            voteCounter=0;
			//            updateVoteNumbers();

			//            instance.leaderboard->setBiddingFocus();
			//            showGuide({tr("Start bidding")+ "[]",tr("Let's go! Bid!")+ "[]", tr("You can bid now!")+ "[]",  tr("Lets do some bidding!")+ "[]", tr("I bet you wont find anything! But you can try to...")+ "[2000]" +tr("Make your biddings!")+ "[]", tr("Make your biddings! Well if you find anything...")+ "[]"});
			//            emit enableMenus(false);
			//            instance.hasSkipped = 0;

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
			voteCounter=0;
			setPhase(phase);
			//            currentPhase = phase;

			//            voteCounter=0;
			//            updateVoteNumbers();

			//            instance.leaderboard->setBiddingFocus();
			//            showGuide({tr("Counting down")+ "[]", tr("Stressed yet? The Timer is running!")+ "[]", tr("You will never find anything in a minute!")+ "[]" });
			//            timeLeft = searchTime; //60
			//            emit time(timeLeft);
			//            countdown.start();
			//            emit enableMenus(false);
			//            instance.hasSkipped = 0;
			//            settings->enableTimerChange(false);

			return true;
		}
		break;
	}
	case Phase::presentation:
	{
		if(currentPhase == Phase::countdown)
		{
			//emit biddingDone();
			voteCounter=0;
			setPhase(phase);
			//            currentPhase = phase;

			//            voteCounter=0; //not used. Fill with different values to implement (voting to skip the presentation phase)
			//            updateVoteNumbers();

			//            //Set Player to player with minimum bid, aka first player after being sorted
			//            emit biddingDone();
			//            emit enableMenus(false);
			//            instance.hasSkipped = 0;
			//            emit focusBoard();
			//            emit enableActionBtn(localUserIsActiveUser()); // TODO: does this make sense here?
			//            settings->enableTimerChange(true);

			return true;
		}
		break;
	}
	case Phase::freeplay:
	{
		if(currentPhase == Phase::presentation)
		{
			//TODO: muss hier voteCounter=0 rein?
			setPhase(phase);
			//            /*TODO: Set user who just made a point as active user.
			//             Save the state that the game had right when the point was scored.*/
			//            board->saveCurrentPositionOfPlayers();

			//            currentPhase = phase;
			//            letUserPlayFree(activeUserID);

			//            voteCounter=0;
			//            updateVoteNumbers();

			//            showGuide({tr("Freeplay")+ "[2000]"+ tr("time to show off")+ "[]"});
			//            emit enableMenus(false);
			//            instance.hasSkipped = 0;
			//            emit focusBoard();
			//            emit enableActionBtn(true); // TODO: does this make sense here?
			return true;
		}
		break;
	}
	}
	qDebug()<< "switchPhase was called, but there could not be a switch to phase" << (int) phase;
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
	qDebug() << "GameControll::letUserPlayFree(const QUuid & userId)";
	if(currentPhase == Phase::freeplay)
	{
		// set user as active user.
		setActiveUserID(userId);
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
	else
	{
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

void GameControll::showGuide(const QStringList & texts)
{
	QString text = texts.at(instance.r->bounded(texts.size())); //TODO: synchronize? (...d?)
	assert(text.endsWith("]"));
	QStringList list = text.split(QRegularExpression("[\\[\\]]"),Qt::KeepEmptyParts);
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
		instance.guideList.append({list.at(i),duration});
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
	qDebug()<<"actionWhenAnimationEnded was set to"<<function;
}

void GameControll::disableAnnoyingSounds(){
	instance.player->stop();
	instance.hasSkipped = 1;
}

int GameControll::getVoteCounter() {
	return instance.voteCounter;
}

int GameControll::getVoteThreshold() {
	return instance.voteThreshold;
}

void GameControll::updateVoteNumbers()
{
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
	qDebug()<<"Updated voting numbers"<<(int)currentPhase<<voteThreshold<<users.length();
	emit updateActionButtonText();
	//TODO: There is one case where this function is called (because of a new user?) and the emit updateActionButtonText(); leads to the "GIVE UP" String in presentation being set enabled, though it shouldn't be. Checking this here is super ugly, but I don't know where else...
	if(currentPhase == Phase::presentation && !localUserIsActiveUser()){
		emit enableActionBtn(false);
	}
}
