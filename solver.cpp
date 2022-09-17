#include "solver.h"
#include <QDebug>


Solver::Solver(QObject *parent) : QObject{parent}
{

}

bool Solver::goalHit(PosKnoten *pos)
{
	QPoint seekerTile = pos->getTileOfSeeker();
	QPoint goalTile = board->goal->getPosition();
	return seekerTile == goalTile;

}


void Solver::generateChildren(ZugKnoten* alt)
{
	for(int player = 0; player<board->players.size(); player++)
	{
		for(Direction d : {Direction::north, Direction::east, Direction::south, Direction::west})
		{
			ZugKnoten::Zug zug = {player, d};
			//Zug machen
			QVector<QPoint> result = makeMove(zug);
			if(result.length() == 0)
			{
				continue;
			}
			//Check for duplicate
			result.prepend(result.takeAt(board->seeker));
			PosKnoten* newPos = posExists(result);
			if(!newPos)
			{
				continue;
			}

			//insert into parent
			ZugKnoten* newZugKnoten = new ZugKnoten(zug, newPos);
			alt->addChild(newZugKnoten);

			//insert into boundary
			frontier.append(newZugKnoten);
		}
	}
}

QVector<QPoint> Solver::makeMove(ZugKnoten::Zug zug)
{
	QVector<QPoint> playerCoordinates;
	for(Tile* t:board->players) {
		playerCoordinates.append(t->getPosition());
	}

	int changeOfXAxis = 0;
	int changeOfYAxis = 0;
	switch(zug.d)
	{
	case Direction::north:
		changeOfYAxis = -1;
		break;
	case Direction::east:
		changeOfXAxis = 1;
		break;
	case Direction::south:
		changeOfYAxis = 1;
		break;
	case Direction::west:
		changeOfXAxis = -1;
		break;
	case Direction::none:
		break;
	}
	QPoint currentPos = playerCoordinates.at(zug.player);

	QPoint nextPos = currentPos + QPoint(changeOfXAxis, changeOfYAxis);
	//Q_ASSERT_X(!nextTile ,"Board::moveActivePlayer","nextTile is nullptr");
	bool actualMovement = false;

	//qDebug() <<"HIER"<< !board->getTile(currentPos)->getWall(zug.d) << playerOnNextTile << (currentPos!=board->goal->getPosition());
	while(!board->getTile(currentPos)->getWall(zug.d) && currentPos!=board->goal->getPosition())
	{
		bool nextTileFree = true;
		for(QPoint player : playerCoordinates)
		{
			if(player == nextPos)
			{
				nextTileFree = false;
			}
		}
		if(!nextTileFree)
		{
			break;
		}
		currentPos = nextPos;
		actualMovement = true;
		if(!board->getTile(nextPos)->getWall(zug.d))
		{
			nextPos = currentPos+ QPoint(changeOfXAxis,changeOfYAxis);
		}
		playerCoordinates[zug.player] = currentPos;
	}
	if(actualMovement)
	{
		return playerCoordinates;
	}
	return QVector<QPoint>();
}

void Solver::solve(Board * b)
{
	if(board)
	{
		delete posBaum;
		delete zugBaum;
	}
	board = b;
	posBaum = new PosKnoten();
	zugBaum = new ZugKnoten();
	ZugKnoten* current = zugBaum;
	do
	{
		generateChildren(current);
		assert(!frontier.isEmpty());
		current = frontier.takeFirst();
		qDebug() << "Frontier size: " << frontier.size();
	} while(!goalHit(current->pos));
	emit solved();
}

QVector<ZugKnoten::Zug> Solver::exportPath()
{
	QVector<ZugKnoten::Zug> path;
	if(goal)
	{
		ZugKnoten* zug = goal;
		while(zug)
		{
			path.prepend(zug->zug);
			zug = zug->parent;
		}
	}
	return path;
}

PosKnoten * Solver::posExists(QVector<QPoint> players)
{
	bool returnNull = true;
	PosKnoten* current = posBaum;
	for(QPoint p:players)
	{
		if(!current->getChildren().contains(PosKnoten::pointToInt(p)))
		{
			PosKnoten* neu = new PosKnoten();
			neu->parent = current;
			current->getChildren().insert(PosKnoten::pointToInt(p),neu);
			returnNull = false;
		}
		current = current->getChildren().value(PosKnoten::pointToInt(p));
	}
	if(returnNull)
	{
		return nullptr;
	}
	return current;
}

