#include "gamecontroll.h"

GameControll::GameControll(QObject *parent) : QObject(parent)
{
	mapping.append(KeyMapping(PlayerAction::movePlayerNorth,{Qt::Key::Key_W,Qt::Key::Key_Up}));
	mapping.append(KeyMapping(PlayerAction::movePlayerEast,{Qt::Key::Key_D,Qt::Key::Key_Right}));
	mapping.append(KeyMapping(PlayerAction::movePlayerSouth,{Qt::Key::Key_S,Qt::Key::Key_Down}));
	mapping.append(KeyMapping(PlayerAction::movePlayerWest,{Qt::Key::Key_A,Qt::Key::Key_Left}));
	mapping.append(KeyMapping(PlayerAction::revert,Qt::Key::Key_R));
	mapping.append(KeyMapping(PlayerAction::giveUp,Qt::Key::Key_Q));
	mapping.append(KeyMapping(PlayerAction::enterBidding,Qt::Key::Key_Space));
	mapping.append(KeyMapping(PlayerAction::clearBidding,Qt::Key::Key_Backspace));
	mapping.append(KeyMapping(PlayerAction::sendBidding,Qt::Key::Key_Return));
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

QVector<KeyMapping> * GameControll::getMapping()
{
	return &mapping;
}

Board * GameControll::getBoard() const
{
	return board;
}
