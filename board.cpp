#include <QDebug>
#include <Direction.h>
#include "board.h"

Board::Board(int width, int height, int playerNumber, QObject *parent) : QObject(parent)
{
	r = new QRandomGenerator(QTime::currentTime().msecsSinceStartOfDay());
	if(playerNumber+1 > width*height)
	{
		qDebug()<< "Board contructor was called with two many players!";
	}
	for(int i=0; i< height; i++)
	{
		QVector<Tile*> innerVector;
		for(int j=0; j<width; j++)
		{
			Tile* tile = new Tile(QPoint(j, i), tiles.isEmpty()?nullptr:tiles.last().at(j), innerVector.isEmpty()?nullptr: innerVector.last(), this);
			if(j==width-1)
			{
				tile->setWall(Direction::east, true);
			}
			if(i==height-1)
			{
				tile->setWall(Direction::south, true);
			}
			innerVector.append(tile);
		}
		tiles.append(innerVector);
	}
	for(int i=0; i<playerNumber; i++)
	{
		Tile * t = getRandomUnoccupiedTile();
		t->setPlayer(i);
		players.append(t);
	}
	placeOuterWalls();
	placeInnerWalls();
	startNewRound();
}

void Board::setPlayerOnTile(int player, Tile* tile)
{
    players[player]->setPlayer(-1);
	tile->setPlayer(player);
	players[player] = tile;
}

void Board::startNewRound()
{
	seeker = r->bounded(players.size());
	//goal only in corner?
	placeGoalInCorner();
    emit goalMoved();
	//else:
	//placeGoalAwayFromSeeker();
    emit boardChanged();
}

Tile* Board::getTile(int x, int y)
{

    if(x < getSize().width() && y < getSize().height())
    {
        return tiles.at(y).at(x);
    }
    return nullptr;
}

QSize Board::getSize()
{
	return QSize(tiles.first().size(),tiles.size());
}

Tile* Board::getRandomUnoccupiedTile()
{
	bool tileIsValid = false;
	Tile* initialTile;
	int counter = 0;
	while(!tileIsValid)
	{
		counter++;
		initialTile = tiles.at(r->bounded(tiles.length()-1)).at(r->bounded(tiles.at(0).length()-1));
		tileIsValid = true;
		for(Tile* player : players)
		{
			if(player == initialTile)
			{
				tileIsValid = false;
			}
		}
		if(goal == initialTile)
		{
			tileIsValid = false;
		}
		if(counter>1000)
		{
			qDebug() << "Can't find any unoccupied tile";
			return nullptr;
		}
	}
	return initialTile;
}

void Board::placeOuterWalls()
{
	int outerWallspots = tiles.at(0).length()*2 + tiles.length()*2 - NUM_PLACES_THAT_CANT_HAVE_OUTER_WALLS;

	int numberOfOuterWalls = (int) (outerWallspots/AVG_DIST_OF_OUTER_WALLS + 1);

	int i =0;

	std::default_random_engine generator(QTime::currentTime().msecsSinceStartOfDay());
    std::uniform_int_distribution<int> randomXIndex(1,(tiles.at(0).length()-NUMBER_OF_UNUSED_OUTER_POSITIONS)); //?
    std::uniform_int_distribution<int> randomYIndex(1,(tiles.length()-NUMBER_OF_UNUSED_OUTER_POSITIONS)); //?
	while(i<numberOfOuterWalls)
	{
		int x= randomXIndex(generator);
		int y = randomYIndex(generator);
		Direction randDir = getNextDirection(Direction::east, r->bounded(1,2));
		int randSide = 0;
		if(randDir == Direction::west)
		{
			if(r->bounded(0,1))
			{
				randSide = 0;
			}
			else
			{
				randSide = tiles.length()-1;
			}
			if(!placeOuterWallIfFits(tiles.at(randSide).at(x), Direction::east))
			{
				i--;
			}
		}
		else
		{
			if(r->bounded(0,1))
			{
				randSide = 0;
			}
			else
			{

				randSide = tiles.at(0).length()-1;
			}
			if(!placeOuterWallIfFits(tiles.at(y).at(randSide), Direction::south))
			{
				i--;
			}
		}
		i++;
	}
}

/**
This method tries to place one outerwall. (One wall that is in a 90 degree angle to the sides of the board)
It can only be called with the Direction south (for walls on the left and right side)
or east (for walls on the upper and lower side)as an argument.
A wall will only be placed if there is no wall directly next to it (this includes walls that belong to the sides of the board)
The method will return false if the wall couldn't be placed.
  **/
bool Board::placeOuterWallIfFits(Tile* tile, Direction direction)
{
	if(direction == Direction::north||direction == Direction::west)
	{
		qDebug()<< "Wrong direction as argument in placeOuterWallsIfFits!";
	}
	else
	{
		int x = tile->getPosition().x();
		int y = tile->getPosition().y();
		if(direction == Direction::east)
		{
			if(!tiles.at(y).at(x-1)->getWall(direction)&&!tiles.at(y).at(x+1)->getWall(direction))
			{
				tile->setWall(direction, true);
				return true;
			}
		}
		if(direction == Direction::south)
		{
			if(!tiles.at(y-1).at(x)->getWall(direction)&&!tiles.at(y+1).at(x)->getWall(direction))
			{
				tile->setWall(direction, true);
				return true;
			}
		}
	}
	return false;
}

void Board::placeInnerWalls()
{
	int numberOfInnerWalls = (tiles.at(0).length()-2) *(tiles.length()-2) / SPREAD_FACTOR_INNER_WALLS;
	int i = 0;
	while(i<numberOfInnerWalls)
	{
		i++;
		int x = r->bounded(1,(tiles.at(0).length()-2));
		int y = r->bounded(1,(tiles.length()-2));
		if(!placeInnerWallifFits(tiles.at(y).at(x), getNextDirection(Direction::north, r->bounded(1,4))))
		{
			i--;
		}
	}
}


/**
This method tries to place one innerwall on a specific Tile. (One "corner" consisting of two walls that are in a 90 degree angle to each other)
It is called with the Tile it should be placed on and a direction. The Direction indicates the position one of the walls will have on the Tile, the other wall will be at the clockwise next direction.
EXAMPLE: |_  this innerwall would be a south wall. _| this innerwall would be an east wall.
A innerwall will only be placed if there is no wall that would connect to it (this includes walls that belong to the sides of the board).
The method will return false if the wall couldn't be placed on the given Tile.
  **/

bool Board::placeInnerWallifFits(Tile* tile, Direction direction)
{

	int x = tile->getPosition().x();
	int y = tile->getPosition().y();
	if(!tile->getWall(Direction::north) &&!tile->getWall(Direction::east) &&!tile->getWall(Direction::south) &&!tile->getWall(Direction::west))
	{
		bool noConflictWithNeighbors = false;
		switch(direction)
		{
		case Direction::north:
			noConflictWithNeighbors =
					!tiles.at(y-1).at(x)->getWall(Direction::west)
					&&!tiles.at(y-1).at(x)->getWall(Direction::east)
					//&&!tiles.at(y+1).at(x)->getWall(Direction::west)
					&&!tiles.at(y+1).at(x)->getWall(Direction::east)
					&&!tiles.at(y).at(x-1)->getWall(Direction::north)
					//&&!tiles.at(y).at(x-1)->getWall(Direction::south)
					&&!tiles.at(y).at(x+1)->getWall(Direction::north)
					&&!tiles.at(y).at(x+1)->getWall(Direction::south);
			break;
		case Direction::east:
			noConflictWithNeighbors =
					//!tiles.at(y-1).at(x)->getWall(Direction::west)
					!tiles.at(y-1).at(x)->getWall(Direction::east)
					&&!tiles.at(y+1).at(x)->getWall(Direction::west)
					&&!tiles.at(y+1).at(x)->getWall(Direction::east)
					//&&!tiles.at(y).at(x-1)->getWall(Direction::north)
					&&!tiles.at(y).at(x-1)->getWall(Direction::south)
					&&!tiles.at(y).at(x+1)->getWall(Direction::north)
					&&!tiles.at(y).at(x+1)->getWall(Direction::south);
			break;
		case Direction::south:
			noConflictWithNeighbors =
					!tiles.at(y-1).at(x)->getWall(Direction::west)
					//&&!tiles.at(y-1).at(x)->getWall(Direction::east)
					&&!tiles.at(y+1).at(x)->getWall(Direction::west)
					&&!tiles.at(y+1).at(x)->getWall(Direction::east)
					&&!tiles.at(y).at(x-1)->getWall(Direction::north)
					&&!tiles.at(y).at(x-1)->getWall(Direction::south)
					//&&!tiles.at(y).at(x+1)->getWall(Direction::north)
					&&!tiles.at(y).at(x+1)->getWall(Direction::south);
			break;
		case Direction::west:
			noConflictWithNeighbors =
					!tiles.at(y-1).at(x)->getWall(Direction::west)
					&&!tiles.at(y-1).at(x)->getWall(Direction::east)
					&&!tiles.at(y+1).at(x)->getWall(Direction::west)
					//&&!tiles.at(y+1).at(x)->getWall(Direction::east)
					&&!tiles.at(y).at(x-1)->getWall(Direction::north)
					&&!tiles.at(y).at(x-1)->getWall(Direction::south)
					&&!tiles.at(y).at(x+1)->getWall(Direction::north);
					//&&!tiles.at(y).at(x+1)->getWall(Direction::south);
			break;
		}
		if(noConflictWithNeighbors)
		{
			tile->setInnerWall(direction, true);
			return true;
		}
	}
	return false;
}

void Board::placeGoalInCorner()
{
	bool noCorner = true;
	while(noCorner)
	{
		int numberOfWalls = 0;

		placeGoalAwayFromSeeker();
        for(int i = 0; i<5; i++)
		{
            //we need to make sure that only tiles with real corners are used.
            //A tile like this | | should not get a goal. So we check if the two walls of the tile are in neighboring directions.
			Direction dir = getNextDirection(Direction::north, i);
			if(goal->getWall(dir))
			{
                qDebug() << goal << printDirection(dir);
                if(numberOfWalls ==1){
                    numberOfWalls = 2;
                }
                else if(numberOfWalls==0){

                    numberOfWalls++;
                }

			}
            else{
                if(numberOfWalls==1){
                numberOfWalls=0;
                }
            }
		}
		noCorner = numberOfWalls<2;
		numberOfWalls=0;
	}
	return;
}


void Board::placeGoalAwayFromSeeker()
{
	bool inRowOrColWithSeeker = true;
	while(inRowOrColWithSeeker)
	{
		goal = getRandomUnoccupiedTile();
		//Random launch out_of_range
		if(!(goal->getPosition().rx() == players.at(seeker)->getPosition().rx()) &&
		   !(goal->getPosition().ry() == players.at(seeker)->getPosition().ry())    )
		{
			inRowOrColWithSeeker = false;
		}
	}
	return;

}

Direction Board::getNextDirection(Direction direction, int numberOfClockwiseSteps)
{
	if(numberOfClockwiseSteps ==0)
	{
		return direction;
	}
	if(numberOfClockwiseSteps<0)
	{
		qDebug()<< "getNextDirection was called with an int lower than 0";
	}
	if(numberOfClockwiseSteps > 1)
	{
		Direction nextDir = getNextDirection(direction, 1);
		numberOfClockwiseSteps--;
		//qDebug()<<printDirection(nextDir).c_str() << numberOfClockwiseSteps;
		return getNextDirection(nextDir, numberOfClockwiseSteps);
	}
	switch(direction)
	{
	case Direction::north:
		return Direction::east;
		break;
	case Direction::east:
		return Direction::south;
		break;
	case Direction::south:
		return Direction::west;
		break;
	case Direction::west:
		return Direction::north;
		break;
	}
}

QString Board::printDirection(Direction direction)
{
	switch(direction)
	{
	case Direction::north:
		return "north";
		break;
	case Direction::east:
		return "east";
		break;
	case Direction::south:
		return "south";
		break;
	case Direction::west:
		return "west";
		break;
	}
}

void Board::moveActivePlayer(Direction d)
{
	int changeOfXAxis = 0;
	int changeOfYAxis = 0;
	switch(d)
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
	}
    //qDebug()<< "blaaaaaa" << changeOfXAxis << "   " << changeOfYAxis;
	Tile* currentTile = players.at(activePlayer);

	Tile* nextTile = getTile(
						 currentTile->getPosition().rx() + changeOfXAxis,
						 currentTile->getPosition().ry() + changeOfYAxis);
    if(nextTile == nullptr)
    {
        return;
    }
    while(!currentTile->getWall(d)&& nextTile->getPlayer()==-1)
	{
		bool nextTileFree = true;
		for(Tile* player : players)
		{
			if(player == nextTile)
			{
				nextTileFree = false;
			}
		}
		if(!nextTileFree)
		{
			return;
		}
		currentTile = nextTile;
		if(!nextTile->getWall(d))
		{
			nextTile = getTile(
						   currentTile->getPosition().rx() + changeOfXAxis,
						   currentTile->getPosition().ry() + changeOfYAxis);
		}
		setPlayerOnTile(activePlayer, currentTile);
        emit playerMoved(activePlayer);
	}
}

void Board::changeActivePlayer(int playerNumber)
{

    activePlayer = playerNumber;
    emit boardChanged();

}




