#include "gamecontroll.h"

using namespace std::chrono_literals;

GameControll::GameControll(QObject *parent) : QObject(parent)
{
	countdown.setSingleShot(false);
	countdown.setInterval(1s);
	connect(&countdown,&QTimer::timeout,this,&GameControll::updateTimer);
	connect(settings, &SettingsDialog::newMapping, this, &GameControll::setMapping);
}

void GameControll::load()
{
	settings = new SettingsDialog(mapping);
	settings->load();
	connect(settings,&SettingsDialog::colorsChanged,this,[&](QColor back, QColor wall, QColor grid){emit colorsChanged(back,wall,grid);});
	connect(settings,&SettingsDialog::newMapping,this,[&](QVector<KeyMapping*> mapping){ this->mapping = mapping; });
	emit colorsChanged(settings->getBackground(),settings->getWallcolor(),settings->getGridcolor());
	this->mapping = settings->getMapping();
}

void GameControll::showSettings()
{
	settings->exec();
}

Board * GameControll::createBoard(int width, int height, int playerNumber)
{
	if(board)
	{
		board->deleteLater();
		board = nullptr;
	}
	board = new Board(width, height, playerNumber, this);
	connect(board,&Board::goalHit,this,&GameControll::nextTarget);
	return board;
}

bool GameControll::triggerAction(PlayerAction action, QUuid userID)
{
	qDebug()<<"Called function TriggerAction with parameters "<<action<<" and User ID "<<userID;
	if(activeUserID.isNull() || userID == activeUserID)
	{
		if(action & PlayerAction::movement)
		{
			if(currentPhase == Phase::presentation || currentPhase == Phase::freeplay) //If online only let the active user move
			{
				//we subtract movement from action to get a direction (clever enum numbers)

				board->moveActivePlayer(static_cast<Direction>(action - PlayerAction::movement));
				emit actionTriggered(action);
				return true;
			}
		}
		else if(action & PlayerAction::playerSwitch)
		{
			if(currentPhase == Phase::presentation || currentPhase == Phase::freeplay)
			{
				board->switchPlayer(static_cast<Direction>(action-PlayerAction::playerSwitch));
				emit actionTriggered(action);
				return true;
			}
		}
		else if(action & PlayerAction::bidding)
		{
			qDebug()<<"Currently in GameControl: triggerAction -> bidding, current Phase is "<<static_cast<int>(currentPhase);
			if(currentPhase == Phase::search || currentPhase == Phase::countdown)
			{
				if(action == PlayerAction::sendBidding)
					switchPhase(Phase::countdown); //If timer has not been started, start the dödöööö FINAL COUNTDOWN dödödödö dödödödödö
				emit actionTriggered(action);
				return true;
			}
		}
		else if(action & PlayerAction::other)
		{
			if(currentPhase == Phase::presentation || currentPhase == Phase::freeplay)
			{
				if(action == PlayerAction::revert)
				{
					board->revert();
				}
				emit actionTriggered(action);
				return true;
			}
		}
	}
	return false;
}

void GameControll::activePlayerChanged(int playerNumber)
{
   if(triggerAction(PlayerAction::playerSwitch, ""))
   {
	   board->changeActivePlayer(playerNumber);
   }
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
