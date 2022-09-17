#include "solver.h"

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
			PosKnoten* newPos = posExists(result);
			if(!newPos) {
				continue;
			}

			//insert into parent
			ZugKnoten* newZugKnoten = new ZugKnoten(zug, newPos);
			alt->addChild(newZugKnoten);

			//insert into boundary
			frontier.append(newZugKnoten);

			//Check for goal
			if(goalHit(newPos)) {
				emit solved();
			}

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

	bool playerOnNextTile = false;
	for(QPoint player:playerCoordinates) {
		playerOnNextTile |= (player==nextPos);
	}

	while(!board->getTile(currentPos)->getWall(zug.d) && playerOnNextTile && currentPos!=board->goal->getPosition())
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
	if(actualMovement) {
		return playerCoordinates;
	}
	return QVector<QPoint>();
}


Solver::Solver(QObject *parent) : QObject{parent}
{

}
