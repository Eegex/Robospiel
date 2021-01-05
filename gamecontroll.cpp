#include "gamecontroll.h"


using namespace std::chrono_literals;

GameControll::GameControll(QObject *parent) : QObject(parent)
{
	mapping.append(new KeyMapping(PlayerAction::movePlayerNorth,{Qt::Key::Key_W,Qt::Key::Key_Up}));
	mapping.append(new KeyMapping(PlayerAction::movePlayerEast,{Qt::Key::Key_D,Qt::Key::Key_Right}));
	mapping.append(new KeyMapping(PlayerAction::movePlayerSouth,{Qt::Key::Key_S,Qt::Key::Key_Down}));
	mapping.append(new KeyMapping(PlayerAction::movePlayerWest,{Qt::Key::Key_A,Qt::Key::Key_Left}));
	mapping.append(new KeyMapping(PlayerAction::switchPlayerNorth,Qt::Key::Key_I));
	mapping.append(new KeyMapping(PlayerAction::switchPlayerEast,Qt::Key::Key_L));
	mapping.append(new KeyMapping(PlayerAction::switchPlayerSouth,Qt::Key::Key_K));
	mapping.append(new KeyMapping(PlayerAction::switchPlayerWest,Qt::Key::Key_J));
	mapping.append(new KeyMapping(PlayerAction::revert,Qt::Key::Key_R));
    mapping.append(new KeyMapping(PlayerAction::revertToBeginning,Qt::Key::Key_B));
	mapping.append(new KeyMapping(PlayerAction::giveUp,Qt::Key::Key_Q));
	mapping.append(new KeyMapping(PlayerAction::enterBidding,Qt::Key::Key_Space));
	mapping.append(new KeyMapping(PlayerAction::clearBidding,Qt::Key::Key_Backspace));
	mapping.append(new KeyMapping(PlayerAction::sendBidding,Qt::Key::Key_Return));
	countdown.setSingleShot(false);
	countdown.setInterval(1s);
	connect(&countdown,&QTimer::timeout,this,&GameControll::updateTimer);
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
    if(activeUserID == nullptr /*@Jan Ist das so gewollt? */ || userID == activeUserID)
    {
		if(action & PlayerAction::movement)
		{
			if(currentPhase == Phase::presentation || currentPhase == Phase::freeplay)
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
			if(currentPhase == Phase::search || currentPhase == Phase::countdown)
			{
				if(action == PlayerAction::sendBidding)
				{
					switchPhase(Phase::countdown);
				}
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
				if(action == PlayerAction::revertToBeginning)
                                {
                                    board -> revertToBeginning();
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
	return &mapping;
}
void GameControll::setMapping(QVector<KeyMapping*> mapping)
{
	this->mapping=mapping;
}

Board * GameControll::getBoard() const
{
	return board;
}

void GameControll::updateTimer()
{
	if(--timeLeft <= 0)
	{
		countdown.stop();
		switchPhase(Phase::presentation);
	}
	emit time(timeLeft);
}
