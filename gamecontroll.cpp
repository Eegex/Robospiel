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
    connect(board,&Board::goalHit,this,&GameControll::nextTarget);
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
    //if(activeUserID.isNull() || userID == activeUserID)
    //{
        if(action & PlayerAction::movement)
        {
			if(currentPhase == Phase::presentation || currentPhase == Phase::freeplay) //If online only let the active user move
            {
                //we subtract movement from action to get a direction (clever enum numbers)
                emit actionTriggered(action);
                return true;
            }
        }
        else if(action & PlayerAction::playerSwitch && (currentPhase == Phase::presentation || currentPhase == Phase::freeplay))
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

void GameControll::activePlayerChanged(int playerNumber) //Brauchen wir das noch? Ich dachte wir machen alles mit der ID? - Also momentan ja...
{
   if(triggerAction(PlayerAction::playerSwitch, ""))
    {
        board->changeActivePlayer(playerNumber);
    }
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
        return true;
    }
    case Phase::search:
    {
        if(currentPhase != Phase::countdown)
        {
            currentPhase = phase;
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
                timeLeft = 2; //60
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
            return true;
        }
        break;
    }
    case Phase::freeplay:
    {
        if(currentPhase == Phase::presentation)
        {
            currentPhase = phase;
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
