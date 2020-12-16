#include "gamecontroll.h"

GameControll::GameControll(QObject *parent) : QObject(parent)
{
    mapping.append( new KeyMapping(PlayerAction::movePlayerNorth,{Qt::Key::Key_W,Qt::Key::Key_Up}));
    mapping.append( new KeyMapping(PlayerAction::movePlayerEast,{Qt::Key::Key_D,Qt::Key::Key_Right}));
    mapping.append( new KeyMapping(PlayerAction::movePlayerSouth,{Qt::Key::Key_S,Qt::Key::Key_Down}));
    mapping.append( new KeyMapping(PlayerAction::movePlayerWest,{Qt::Key::Key_A,Qt::Key::Key_Left}));
    mapping.append( new KeyMapping(PlayerAction::revert,Qt::Key::Key_R));
    mapping.append( new KeyMapping(PlayerAction::giveUp,Qt::Key::Key_Q));
    mapping.append( new KeyMapping(PlayerAction::enterBidding,Qt::Key::Key_Space));
    mapping.append( new KeyMapping(PlayerAction::clearBidding,Qt::Key::Key_Backspace));
    mapping.append( new KeyMapping(PlayerAction::sendBidding,Qt::Key::Key_Return));
}

Board * GameControll::createBoard(int width, int height, int playerNumber)
{
	if(board)
	{
		board->deleteLater();
		board = nullptr;
	}
	board = new Board(width, height, playerNumber, this);
	return board;
}

bool GameControll::triggerAction(PlayerAction action, QString user)
{
	if((activeUser.isEmpty() || user == activeUser) && true /*action possible?*/)
	{
		if(action & PlayerAction::movement)
		{
			emit actionTriggered(action);
		}
		else if(action & PlayerAction::playerSwitch)
		{
			emit actionTriggered(action);
		}
		else if(action & PlayerAction::bidding)
		{
			emit actionTriggered(action);
		}
		else if(action & PlayerAction::other)
		{
			emit actionTriggered(action);
		}
		return true;
	}
	else
	{
		return false;
	}
}

void GameControll::nextTarget()
{
	board->startNewRound();
}

void GameControll::remakeBoard()
{

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
