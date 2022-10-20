#include "solver.h"
#include <QThread>
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
			QVector<QPoint> result = makeMove(zug, alt);
			if(result.length() == 0)
			{
				continue;
			}
			//Check for duplicate
			PosKnoten* newPos = makeUniquePosKnoten(result);
			if(!newPos)
			{
				continue;
			}

			//insert into parent
			ZugKnoten* newZugKnoten = new ZugKnoten(zug, newPos);
			alt->addChild(newZugKnoten);

			//insert into boundary
			if(goalHit(newZugKnoten->pos))
			{
				goal = newZugKnoten;
				return;
			}
			if(alt == levelEdge)
			{
				++depth;
				width.append(generated - width.last());
				levelEdge = newZugKnoten;
				qDebug() << "next Level" << depth;
			}
			++generated;
			frontier.append(newZugKnoten);
		}
	}
}

QVector<QPoint> Solver::makeMove(ZugKnoten::Zug zug, ZugKnoten* alt)
{

	QVector<QPoint> playerCoordinates;
	alt->pos->getCoordinates(&playerCoordinates);

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
			nextPos = currentPos + QPoint(changeOfXAxis,changeOfYAxis);
		}
		playerCoordinates[zug.player] = currentPos;
	}
	if(actualMovement)
	{
		return playerCoordinates;
	}
	return QVector<QPoint>();
}

void Solver::solveBoard(Board* b, QVector<ZugKnoten::Zug>& path)
{
	qDebug() << "Solver::solveBoard(Board * b)";
	if(board)
	{
		frontier.clear();
		delete posBaum;
		delete zugBaum;
		goal = nullptr;
		width.clear();
		depth = 0;
		generated = 0;
		explored = 0;
	}
	width << 0;
	board = b;
	path.clear();
	posBaum = new PosKnoten();

	QVector<QPoint> playerCoordinates;
	for(Tile* t:board->players)
	{
		playerCoordinates.append(t->getPosition());
	}
	playerCoordinates.prepend(playerCoordinates.takeAt(board->seeker));
	zugBaum = new ZugKnoten({}, makeUniquePosKnoten(playerCoordinates));
	levelEdge = zugBaum;

	ZugKnoten* current = zugBaum;
	do
	{
		generateChildren(current);
		if(!frontier.isEmpty())
		{
			current = frontier.takeFirst();
			++explored;
		}
	} while(!goal);
	exportPath(path);
	++depth;
	width.append(generated - width.last());
	qDebug() << "Final depth:" << depth;
	qDebug() << "Generated:" << generated;
	qDebug() << "Explored:" << explored;
	qDebug() << "Percentage:" << static_cast<double>(explored) / generated;
	for(int i = 0; i+1 < width.size();i++)
	{
		QDebug d = qDebug();
		d.setAutoInsertSpaces(false);
		d << "Level " << i << ": " << width.at(i+1);
	}
}

void Solver::exportPath(QVector<ZugKnoten::Zug>& path)
{
	if(goal)
	{
		ZugKnoten* zug = goal;
		while(zug->parent)
		{
			path.prepend(zug->zug);
			zug = zug->parent;
		}
	}
	for(ZugKnoten::Zug& z:path)
	{
		if(z.player == 0)
		{
			z.player = board->seeker;
		}
		else if(z.player <= board->seeker)
		{
			z.player--;
		}
	}
}

/**
 * @brief Solver::makeUniquePosKnoten
 * @param players
 * @return newly created PosKnoten or nullpointer if the node already existed.
 */
PosKnoten * Solver::makeUniquePosKnoten(QVector<QPoint> players)
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
