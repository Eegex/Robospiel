#include <QDebug>
#include <Direction.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QDataStream>
#include <QIODevice>
#include "board.h"

Board::Board(QObject *parent) : QObject(parent)
{
	r = new QRandomGenerator(QTime::currentTime().msecsSinceStartOfDay());
}

/*!
 * \brief Board::Board constructs a new game board
 * \param width - how many tiles are there in the width direction
 * \param height - how many tiles there are in the height direction
 * \param playerNumber - number of players
 * \param parent
 */
Board::Board(int width, int height, int playerNumber, QObject *parent) : Board(parent)
{
	if(playerNumber + 1 > width * height)
	{
		qDebug()<< "Board contructor was called with too many players!";
	}
	makeNewBoard(width, height, playerNumber);
	//saveCurrentPositionOfPlayers();
	startNewRound();
}

/*!
 * \brief Board::makeNewBoard - constructs a new game board with new Walls, players and a goal
 * \param width
 * \param height
 * \param playerNumber
 */
void Board::makeNewBoard(int width, int height, int playerNumber)
{
	makeNewWalls(width, height);
	makeNewPlayers(playerNumber);
	makeNewGoal();
	emit boardChanged();
}

/*!
 * \brief Board::makeNewPlayers deletes all players and places them new random tiles. Also emits the playerMoved signal.
 * \param playerNumber
 */
void Board::makeNewPlayers(int playerNumber)
{
	players.clear();
	for(int i=0; i<playerNumber; i++)
	{
		Tile * t = getRandomUnoccupiedTile();
		t->setPlayer(i);
		players.append(t);
		emit playerMoved(i,(goal == t && seeker == activePlayer) ? moves : -1);
		emit boardChanged();
	}
}

/*!
 * \brief Board::makeNewSeeker - changes the player that is the seeker to a new one
 * \param random - do you want the new player to a surprise? Else the seekers will switch in a certain order (depending on their id)
 */
void Board::makeNewSeeker(bool random)
{
	if(players.length()<2) //just one player? This will stay the seeker!
	{
		return;
	}
	if(random) //random new seeker
	{
		int originalSeeker = seeker;
		while(seeker == originalSeeker)
		{
			seeker = r->bounded(players.size());
		}
	}
	else //the player with the next id will be the seeker
	{
		seeker = (seeker+1)%players.length();
	}
	activePlayer = seeker;
	emit boardChanged();
}

/*!
 * \brief Board::makeNewGoal - places a new goal
 */
void Board::makeNewGoal()
{
	placeGoalInCorner();
	emit goalMoved();
	emit boardChanged();
}

void Board::makeNewWalls(int width, int height)
{
	tiles.clear();
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
	placeOuterWalls();
	placeInnerWalls();
	emit boardChanged();
}

QString Board::toBinary()
{
	QByteArray binary;
	QDataStream s(&binary,QIODevice::WriteOnly);
	s << static_cast<quint8>(tiles.size());
	s << static_cast<quint8>(tiles.at(0).size());
	QVector<QPoint> eastWalls;
	QVector<QPoint> southWalls;
	for(const QVector<Tile*> &row : qAsConst(tiles))
	{
		for(Tile* tile : row)
		{
			if(tile->getWall(Direction::east))
			{
				eastWalls.append(tile->getPosition());
			}
			if(tile->getWall(Direction::south))
			{
				southWalls.append(tile->getPosition());
			}
		}
	}
	s << static_cast<quint16>(eastWalls.size());
	for(QPoint p:eastWalls)
	{
		s << static_cast<quint8>(p.x()) << static_cast<quint8>(p.y());
	}
	s << static_cast<quint16>(southWalls.size());
	for(QPoint p:southWalls)
	{
		s << static_cast<quint8>(p.x()) << static_cast<quint8>(p.y());
	}
	s << static_cast<quint8>(players.size());
	for(Tile* player: qAsConst(players))
	{
		s << static_cast<quint8>(player->getPosition().x());
		s << static_cast<quint8>(player->getPosition().y());
	}
	s << static_cast<quint8>(goal->getPosition().x());
	s << static_cast<quint8>(goal->getPosition().y());
	s << static_cast<quint16>(history.size());
	for(HistoryElement historyElement : qAsConst(history))
	{
		s << static_cast<quint8>(historyElement.action);
		s << static_cast<quint8>(historyElement.previousPlayer);
		s << static_cast<quint8>(historyElement.previousPosition.x());
		s << static_cast<quint8>(historyElement.previousPosition.y());
	}
	s << static_cast<quint8>(seeker);
	s << static_cast<quint8>(activePlayer);
	s << static_cast<quint8>(moves);
	return QString::fromUtf8(binary.toBase64());
}

Board * Board::fromBinary(const QString base64)
{
	QByteArray data = QByteArray::fromBase64(base64.toUtf8());
	QDataStream s(&data,QIODevice::ReadOnly);
	quint8 height, width, playerSize, seeker, activePlayer, moves;
	quint8 x, y, action, prevPlayer;
	quint16 eastWallSize, southWallSize, historySize;
	QVector<QPoint> eastWalls;
	QVector<QPoint> southWalls;
	QVector<QPoint> playerPos;
	QVector<HistoryElement> history;
	// Read data
	s >> height >> width >> eastWallSize;
	for(int i = 0; i < eastWallSize;i++)
	{
		s >> x >> y;
		eastWalls << QPoint(x,y);
	}
	s >> southWallSize;
	for(int i = 0; i < southWallSize;i++)
	{
		s >> x >> y;
		southWalls << QPoint(x,y);
	}
	s >> playerSize;
	for(int i = 0; i < playerSize;i++)
	{
		s >> x >> y;
		playerPos << QPoint(x,y);
	}
	s >> x >> y;
	QPoint goal = QPoint(x,y);
	s >> historySize;
	for(int i = 0; i < historySize;i++)
	{
		s >> action >> prevPlayer >> x >> y;
		HistoryElement he;
		he.action = static_cast<PlayerAction>(action);
		he.previousPlayer = prevPlayer;
		he.previousPosition = QPoint(x,y);
		history.append(he);
	}
	s >> seeker >> activePlayer >> moves;
	// Construct Board
	Board * newBoard = new Board();
	newBoard->tiles.clear();
	for(int i=0; i < height; i++)
	{
		QVector<Tile*> innerVector;
		for(int j=0; j < width; j++)
		{
			Tile* tile = new Tile(QPoint(j, i), newBoard->tiles.isEmpty()?nullptr:newBoard->tiles.last().at(j), innerVector.isEmpty()?nullptr: innerVector.last(), newBoard);
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
		newBoard->tiles.append(innerVector);
	}
	for(const QPoint & w:qAsConst(eastWalls))
	{
		newBoard->getTile(w.x(),w.y())->setWall(Direction::east,true);
	}
	for(const QPoint & w:qAsConst(southWalls))
	{
		newBoard->getTile(w.x(),w.y())->setWall(Direction::south,true);
	}
	for(const QPoint & w:qAsConst(playerPos))
	{
		Tile * t = newBoard->getTile(w.x(),w.y());
		t->setPlayer(newBoard->players.size());
		newBoard->players.append(t);
	}
	newBoard->goal = newBoard->getTile(goal.x(),goal.y());
	newBoard->history = history;
	newBoard->seeker = seeker;
	newBoard->activePlayer = activePlayer;
	newBoard->moves = moves;
	return newBoard;
}

/*!
 * \brief Board::setPlayerOnTile yes, actually removes player from former tile
 * \param player
 * \param tile
 */
void Board::setPlayerOnTile(int player, Tile* tile)
{
	players[player]->setPlayer(-1);
	tile->setPlayer(player);
	players[player] = tile;
}

/*!
 * \brief Board::startNewRound is responsible for setting everything up for a new round.
 * It makes a new seeker and goal and resets history and moves.
 */
void Board::startNewRound()
{
	resetMoves();
	makeNewSeeker(true);
	activePlayer = seeker;
	history.clear();
	//goal only in corner?
	makeNewGoal();
	//else:
	//placeGoalAwayFromSeeker();
}

/*!
 * \brief Board::getTile returns a pointer to a tile given the coordinates of that tile
 * \param p - coordinates of the tile (column of tile starting at 0, row of tile starting at 0)
 * \return pointer to the tile
 */
Tile* Board::getTile(const QPoint p)
{
	if(p.x() < getSize().width() && p.y() < getSize().height() && p.x()>= 0 && p.y()>= 0)
	{
		return tiles.at(p.y()).at(p.x());
	}
	return nullptr;
}

/*!
 * \brief Board::getTile returns a pointer to a tile given the coordinates of that tile
 * \param x - column of tile starting at 0
 * \param y - row of tile starting at 0
 * \return pointer to the tile
 */
Tile* Board::getTile(int x, int y)
{
	return getTile({x,y});
}

/*!
 * \brief Board::getSize
 * \return the size of the board as (width, height)
 */
QSize Board::getSize()
{
	return QSize(tiles.first().size(),tiles.size());
}

/*!
 * \brief Board::getRandomTile
 * \return pointer to a random tile from the board
 */
Tile* Board::getRandomTile()
{
	return tiles.at(r->bounded(tiles.length())).at(r->bounded(tiles.at(0).length()));
}


/*!
 * \brief Board::getRandomUnoccupiedTile
 * \return a tile that has no player nor goal currently placed on it
 */
Tile* Board::getRandomUnoccupiedTile()
{
	bool tileIsValid = false;
	Tile* initialTile = nullptr;
	int counter = 0;
	while(!tileIsValid)
	{
		counter++;
		initialTile = getRandomTile();
		tileIsValid = true;
		for(Tile* player : qAsConst(players))
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
		if(counter>100000)
		{
			for(int i =0; i<tiles.size(); i++){
				for(int j = 0; j<tiles.at(0).size(); j++){
					initialTile = tiles.at(i).at(j);
					tileIsValid = true;
					for(Tile* player : qAsConst(players))
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
				}
				if(!tileIsValid){
					qDebug() << "Can't find any unoccupied tile";
					return nullptr;
				}
			}

		}
	}
	return initialTile;
}

void Board::placeOuterWalls()
{
	int outerWallspots = tiles.at(0).length()*2 + tiles.length()*2 - NUM_PLACES_THAT_CANT_HAVE_OUTER_WALLS;

	int numberOfOuterWalls = (int) (outerWallspots/AVG_DIST_OF_OUTER_WALLS + 1);
	int i = 0;
	while(i<numberOfOuterWalls)
	{
		int x = r->bounded(1,tiles.at(0).length()-NUMBER_OF_UNUSED_OUTER_POSITIONS);
		int y = r->bounded(1,tiles.length()-NUMBER_OF_UNUSED_OUTER_POSITIONS);
		Direction randDir = getNextDirection(Direction::east, r->bounded(1,3));
		int randSide = 0;
		if(randDir == Direction::west)
		{
			if(r->bounded(0,2))
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
			if(r->bounded(0,2))
			{
				randSide = 0;
			}
			else
			{
				randSide = tiles[0].length()-1;
			}
			if(!placeOuterWallIfFits(tiles.at(y).at(randSide), Direction::south))
			{
				i--;
			}
		}
		i++;
	}
}

/*!
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
		int x = r->bounded(1,(tiles.at(0).length()-1));
		int y = r->bounded(1,(tiles.length()-1));
		if(!placeInnerWallifFits(tiles.at(y).at(x), getNextDirection(Direction::north, r->bounded(1,5))))
		{
			i--;
		}
	}
}

/*!
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
		case Direction::none:
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

/*!
 * \brief Board::placeGoalInCorner - places a goal into a spot that has two walls that are in a right angle to each other
 */
void Board::placeGoalInCorner()
{
	bool noCorner = true;
	int count = 0;
	while(noCorner) //try to just find a new corner spot for the goal
	{
		placeGoalAwayFromSeeker();
		noCorner = !isTileCorner(goal);
		count++;
		if(count > 1000) // when this didn't work after 1000 tries
		{
			//find a corner
			Tile*  tile;
			while(noCorner)
			{
				tile = getRandomTile();
				noCorner = !isTileCorner(tile);
			}
			//put the goal there
			goal = tile;
			//move the player that's in that corner to a random tile
			int playerNum = tile->getPlayer();
			if (playerNum != -1)
			{
				setPlayerOnTile(playerNum, getRandomUnoccupiedTile());
				//emit boardChanged();
				emit playerMoved(playerNum, -1);
			}
		}
	}
	emit goalMoved();
	return;
}

/*!
 * \brief Board::isTileCorner - checks if a tile has two walls that are in a right angle to each other
 * \param tile
 * \return
 */
bool Board::isTileCorner(Tile* tile){

	int numberOfWalls = 0;
	for(int i = 0; i<5; i++)
	{
		//we need to make sure that only tiles with real corners are used.
		//A tile like this | | is not a corner. So we check if the two walls of the tile are in neighboring directions.
		Direction dir = getNextDirection(Direction::north, i);
		if(tile->getWall(dir))
		{
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
	return numberOfWalls>=2;

}

void Board::placeGoalAwayFromSeeker()
{

	bool inRowOrColWithSeeker = true;
	while(inRowOrColWithSeeker)
	{
		goal = getRandomUnoccupiedTile();
		if(goal==nullptr)
		{
			goal = getRandomUnoccupiedTile();
		}
		if(!(goal->getPosition().rx() == players.at(seeker)->getPosition().rx()) &&
		   !(goal->getPosition().ry() == players.at(seeker)->getPosition().ry()))
		{
			inRowOrColWithSeeker = false;
		}
	}
	return;

}

int Board::getMoves() const
{
	return moves;
}

void Board::setMoves(int moves)
{
	this->moves = moves;
}

QColor Board::getGrid() const
{
	return grid;
}

QColor Board::getPrimary() const
{
	return primary;
}

QColor Board::getBackground() const
{
	return background;
}

QColor Board::getPlayerColorLow() const
{
	return playerLow;
}

QColor Board::getPlayerColorHigh() const
{
	return playerHigh;
}

/*!
 * \brief Board::getNextDirection
 * \param direction
 * \param numberOfClockwiseSteps
 * \return
 */
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
	case Direction::east:
		return Direction::south;
	case Direction::south:
		return Direction::west;
	case Direction::west:
		return Direction::north;
	case Direction::none:
		return Direction::none;
	}
}

QString Board::printDirection(Direction direction)
{
	switch(direction)
	{
	case Direction::north:
		return "north";
	case Direction::east:
		return "east";
	case Direction::south:
		return "south";
	case Direction::west:
		return "west";
	case Direction::none:
		return "none";
	}
}

void Board::moveActivePlayer(Direction d, int targetX, int targetY, bool isRevert)
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
	case Direction::none:
		break;
	}
	Tile * currentTile = players.at(activePlayer);
	HistoryElement h = HistoryElement();
	h.action = static_cast<PlayerAction>((int)PlayerAction::movement+(int)d);
	h.previousPosition = currentTile->getPosition();

	Tile * nextTile = getTile(currentTile->getPosition().rx() + changeOfXAxis,
							  currentTile->getPosition().ry() + changeOfYAxis);
	//Q_ASSERT_X(!nextTile ,"Board::moveActivePlayer","nextTile is nullptr");
	bool actualMovement = false;
	while(!currentTile->getWall(d) && nextTile->getPlayer()==-1 && (currentTile->getPosition().x()!=targetX || currentTile->getPosition().y()!=targetY))
	{
		bool nextTileFree = true;
		for(Tile* player : qAsConst(players))
		{
			if(player == nextTile)
			{
				nextTileFree = false;
			}
		}
		if(!nextTileFree)
		{
			break;
		}
		currentTile = nextTile;
		actualMovement = true;
		if(!nextTile->getWall(d))
		{
			nextTile = getTile(currentTile->getPosition().rx() + changeOfXAxis,
							   currentTile->getPosition().ry() + changeOfYAxis);
		}
		setPlayerOnTile(activePlayer, currentTile);
	}
	if(actualMovement)
	{
		if(!isRevert)
		{
			moves++;
		}
		qDebug()<<"Moves:"<<moves;
		goalHit = (goal == currentTile && seeker == activePlayer);
		qDebug()<<"Currently in Board Class, the goal has"<<(goalHit?"been hit!":"!NOT! been hit!");
		if(!isRevert)
		{
			history.append(h);
		}
		emit playerMoved(activePlayer, (goal == currentTile && seeker == activePlayer) ? moves : -1);
	}
}

void Board::changeActivePlayer(int playerNumber, bool isRevert)
{
	if(!isRevert)
	{
		HistoryElement h = HistoryElement();
		h.action=PlayerAction::playerSwitch;
		h.previousPlayer=activePlayer;
		history.append(h);
	}
	activePlayer = playerNumber;
	//emit boardChanged();
}

/*!
 * \brief Board::revert ist super
 */
void Board::revert()
{
	if(!history.isEmpty())
	{
		HistoryElement h = history.takeLast();
		if(h.action & PlayerAction::movement)//binary number magic
		{
			int direction = h.action-PlayerAction::movement;
			direction = direction > static_cast<int>(Direction::east) ? direction>>2 : direction<<2; //invert direction
			moves--; //delete the former move and do this action without incrementing moves.
			// Has to be before moveActivePlayer(), because otherwise calculateGameStatus() would have a wrong number of moves.
			qDebug() << "move to" << h.previousPlayer << h.previousPosition;
			moveActivePlayer(static_cast<Direction>(direction), h.previousPosition.x(), h.previousPosition.y(),true);
			qDebug()<<"Moves:"<<moves;
		}
		if(h.action == PlayerAction::playerSwitch)
		{
			changeActivePlayer(h.previousPlayer, true);
		}
	}
	//TODO delete history after each presentation and after the freeplay-phase
}

void Board::revertToBeginning()
{
	while(!history.isEmpty())
	{
		revert();
	}
}

void Board::saveCurrentPositionOfPlayers()
{
	playersAfterGoalHit.resize(players.size());
	for(int i = 0; i < players.size();i++)
	{
		playersAfterGoalHit[i] = players.at(i)->getPosition();
	}
}

void Board::setSavedStateToCurrent()
{
	if(playersAfterGoalHit.length() >0)
	{
		int i = 0;
		for(QPoint p:qAsConst(playersAfterGoalHit))
		{
			setPlayerOnTile(i,getTile(p));
			emit playerBeam(i);
			i++;
		}
		playersAfterGoalHit.clear();
	}
	//emit boardChanged(); //no idea if that is necessary...
}

int Board::addPlayer(Tile * t)
{
	players.append(t);
	t->setPlayer(players.size() - 1);
	return t->getPlayer();
}

void Board::updateColors(QColor b, QColor w, QColor g, QColor p1, QColor p2)
{
	background = b;
	primary = w;
	grid = g;
	playerLow = p1;
	playerHigh = p2;
	emit paintPlayers();
	//emit boardChanged();
}

/*!
 * \brief Board::switchPlayer This method is called with a direction that indicates the way the gamer wants to switch the player.
 * \param d search Direction
 * \return id of next active Player
 * We compute the angle each player has from the activePlayer (up would be 360/0 degrees, then it goes clockwise)
 * As well as the distance each playe has from the active one
 *  From these two values a Fittingscore is computed and the player with the SMALLEST one is chosen as the next active player
 */
int Board::switchPlayer(Direction d)
{
	qDebug() << "Board::switchPlayer(Direction d)" << printDirection(d);
	if(!static_cast<int>(d))
	{
		qDebug() << "keine Richtung";
		return activePlayer;
	}
	int targetAngle;
	switch(d)
	{
	case Direction::north:
	{
		targetAngle = 0;
		break;
	}
	case Direction::east:
	{
		targetAngle = 90;
		break;
	}
	case Direction::south:
	{
		targetAngle = 180;
		break;
	}
	case Direction::west:
	{
		targetAngle = 270;
		break;
	}
	}
	qDebug() << targetAngle;
	float angleFactor = 2; //indicates the priority of the angle in deciding to what player you should jump
	float distanceFactor = 1; //indicates the priority of the distance in deciding to what player you should jump
	float largestPossibleFittingScore = angleFactor + distanceFactor;
	float minFit = largestPossibleFittingScore;
	Tile * min = nullptr;

	for(Tile * t:qAsConst(players))
	{
		if(t->getPlayer() != activePlayer)
		{
			qDebug() << "Player: " << t->getPlayer();
			QPoint delta = t->getPosition() - players.at(activePlayer)->getPosition();
			qDebug() << delta;

			int tileAngle = 0;
			if(delta.x()) //so we don't divide by 0
			{
				//int tileAngle = atan(abs(fraction) * 180/3.14159265358979323846 - targetAngle);
				tileAngle = atan(((float) delta.y()/(float) delta.x())) * 180/3.14159265358979323846 ;

			}
			if(tileAngle < 0) // a negative angle should actually be the reverse one
			{
				tileAngle += 90;
			}

			//qDebug() << "initial Winkel" << tileAngle;
			// as till now we have only an angle between 0 and 90, we must add degrees depending on the quadrant.
			// note that we always include the first achsis and exclude the last axis (clockwise)
			if(delta.y()<0 && delta.x()>=0)
			{
				//qDebug() << "in 1st quadrant";
			}
			if(delta.y()>=0 && delta.x()>0)
			{
				//qDebug() << "in 4th quadrant";
				tileAngle = tileAngle + 90;
			}
			else if(delta.y()>0 && delta.x()<=0)
			{
				//qDebug() << "in 3rd quadrant";
				tileAngle = tileAngle +180;
			}
			else if(delta.y()<=0 && delta.x()<0)
			{
				//qDebug() << "in 2nd quadrant";
				tileAngle = tileAngle +270;
			}

			//qDebug() << "standardized angle" << tileAngle;

			float distanceOfAngles = std::min(abs(tileAngle- targetAngle), abs(360-tileAngle+targetAngle));
			//            qDebug() << distanceOfAngleToTargetAngle;

			float distance = sqrt(pow(delta.x(),2) + pow(delta.y(),2) );
			float largestPossibleDistance = sqrt(pow(tiles.size(),2) + pow(tiles.at(0).size(),2) );
			//            qDebug() << "Distance: " << distance << largestPossibleDistance;

			//falls jmd nen besseren Namen weiß, gerne umbennen, soll heißen wie gut der Player sich eignet um für diese Bewegung genommen zu werden
			float fittingScore = largestPossibleFittingScore;

			if(!(distanceOfAngles>=90)) //we never want to move down when the gamer said to move up
			{
				fittingScore = angleFactor * distanceOfAngles/360 + distanceFactor * distance/largestPossibleDistance;
			}

			//            qDebug() << "Fitting: " << fittingScore << minFit;

			if(fittingScore < minFit) //looking for the best (smallest) fittingScore
			{
				min = t;
				minFit = fittingScore;
				//                qDebug() << "FittingAfter: " << fittingScore << minFit;
			}
		}
	}
	if(minFit < largestPossibleFittingScore)
	{
		//qDebug() << "Ergebnis" << min->getPlayer();
		changeActivePlayer(min->getPlayer(), false);
		return min->getPlayer();
	}
	else
	{
		return activePlayer;
	}
}

void Board::resetMoves()
{
	moves = 0;
	qDebug()<<"Moves:"<<moves;
}

void Board::updateRandomGenerator(int seed)
{
	if(r)
	{
		delete r;
	}
	r = new QRandomGenerator(seed);
}
