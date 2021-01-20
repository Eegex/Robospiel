#include <QDebug>
#include <Direction.h>
#include <QJsonObject>
#include <QJsonArray>
#include "board.h"

Board::Board(int width, int height, int playerNumber, QObject *parent) : QObject(parent)
{
    r = new QRandomGenerator(QTime::currentTime().msecsSinceStartOfDay());
    if(playerNumber+1 > width*height)
    {
        qDebug()<< "Board contructor was called with too many players!";
    }

    makeNewBoard(width, height, playerNumber);

    startNewRound();
}

void Board::makeNewBoard(int width, int height, int playerNumber){
    makeNewWalls(  width, height);

    makeNewPlayers(playerNumber);


    makeNewGoal();






    //emit boardChanged();
}


void Board::makeNewPlayers(int playerNumber){
    players.clear();
    for(int i=0; i<playerNumber; i++)
    {
        Tile * t = getRandomUnoccupiedTile();
        t->setPlayer(i);
        players.append(t);
    }
    for (int i = 0; i < players.size(); i++){
        emit playerMoved(i);
    }

}

void Board::makeNewSeeker(bool random){
    if(players.length()<2){
        return;
    }
    if(random){
        int originalSeeker = seeker;
        while(seeker == originalSeeker){
            seeker = r->bounded(players.size());
        }
    }
    else{
        seeker = (seeker+1)%players.length();
    }
    activePlayer = seeker;
    emit boardChanged();

}

void Board::makeNewGoal(){

    placeGoalInCorner();
    emit goalMoved();


}

void Board::makeNewWalls(int width, int height){
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



Board::Board()
{

}

QJsonObject Board::toJSON()
{
    QJsonObject json;

    //tiles
    QJsonArray tileArray;
    for(QVector<Tile*> row : tiles)
    {
        QJsonArray innerTileArray;
        for(Tile* tile : row)
        {
            innerTileArray.append(tile->toJSON());
        }
        tileArray.append(innerTileArray);
    }
    json.insert("tiles", tileArray);

    //players
    QJsonArray playerPositions;
    for(Tile* player: players)
    {
        playerPositions.append(player->getPosition().x());
        playerPositions.append(player->getPosition().y());
    }
    json.insert("playerPositions", playerPositions);

    //goal
    QJsonArray goalPosition;
    goalPosition.append(goal->getPosition().x());
    goalPosition.append(goal->getPosition().y());
    json.insert("goalPosition", goalPosition);

    //history
    QJsonArray historyArray;
    for(HistoryElement historyElement : history)
    {
        QJsonObject historyElementJson;
        historyElementJson.insert("action", historyElement.action);
        historyElementJson.insert("previousPlayer", historyElement.previousPlayer);
        QJsonArray previousPosition;
        previousPosition.append(historyElement.previousPosition.x());
        previousPosition.append(historyElement.previousPosition.y());
        historyElementJson.insert("previousPosition", previousPosition);

        historyArray.append(historyElementJson);
    }
    json.insert("history", historyArray);

    json.insert("seeker", seeker);
    json.insert("activePlayer", activePlayer);
    json.insert("moves", moves);

    return json;
}

Board* Board::fromJSON(QJsonObject json)
{
    Board* b = new Board();

    //tiles
    QJsonArray jsonTileArray = json.value("tiles").toArray();
    for(int i=0; i<jsonTileArray.size(); i++)
    {
        QVector<Tile*> row;
        QJsonArray jsonRow = jsonTileArray.at(i).toArray();
        for(int j=0; j<jsonRow.size(); j++)
        {
            Tile * t = Tile::fromJSON(jsonRow.at(j).toObject());
            if(i>0)
            {
                t->northTile = b->tiles.at(i-1).at(j);
            }
            if(j>0)
            {
                t->westTile = row.at(j-1);
            }
            row.append(t);
        }
        b->tiles.append(row);
    }

    //players
    QJsonArray playerPositions = json.value("playerPositions").toArray();
    for(int i=0; i<playerPositions.size(); i+=2)
    {
        int x = playerPositions.at(i).toInt();
        int y = playerPositions.at(i+1).toInt();
        b->players.append(b->tiles.at(y).at(x));
    }

    //goal
    int x = json.value("goalPosition").toArray().at(0).toInt();
    int y = json.value("goalPosition").toArray().at(1).toInt();
    b->goal = b->tiles.at(y).at(x);

    //history
    for(QJsonValue historyElem : json.value("history").toArray())
    {
        QJsonObject jsonElem = historyElem.toObject();
        HistoryElement he;
        he.action = static_cast<PlayerAction>(jsonElem.value("action").toInt());
        he.previousPlayer = jsonElem.value("previousPlayer").toInt();
        he.previousPosition = QPoint(jsonElem.value("previousPosition").toArray().at(0).toInt(),jsonElem.value("previousPosition").toArray().at(1).toInt());
        b->history.append(he);
    }

    b->seeker = json.value("seeker").toInt();
    b->activePlayer = json.value("activePlayer").toInt();
    b->moves = json.value("moves").toInt();

    return b;
}

void Board::setPlayerOnTile(int player, Tile* tile)
{
    players[player]->setPlayer(-1);
    tile->setPlayer(player);
    players[player] = tile;
}

void Board::startNewRound()
{
    resetMoves();
    makeNewSeeker(true);
    history.clear();
    activePlayer = seeker;
    //goal only in corner?
    placeGoalInCorner();
    emit goalMoved();
    //else:
    //placeGoalAwayFromSeeker();
    emit boardChanged();
}

Tile* Board::getTile(int x, int y)
{

    if(x < getSize().width() && y < getSize().height() && x>= 0 && y>= 0)
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
        initialTile = tiles.at(r->bounded(tiles.length())).at(r->bounded(tiles.at(0).length()));
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
        int x = r->bounded(1,(tiles.at(0).length()-1));
        int y = r->bounded(1,(tiles.length()-1));
        if(!placeInnerWallifFits(tiles.at(y).at(x), getNextDirection(Direction::north, r->bounded(1,5))))
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
        if(!(goal->getPosition().rx() == players.at(seeker)->getPosition().rx()) &&
                !(goal->getPosition().ry() == players.at(seeker)->getPosition().ry())    )
        {
            inRowOrColWithSeeker = false;
        }
    }
    return;

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
    case Direction::none:
        return "none";
        break;
    }
}

void Board::moveActivePlayer(Direction d, int targetX, int targetY)
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
    //qDebug()<< "blaaaaaa" << changeOfXAxis << "   " << changeOfYAxis;
    Tile* currentTile = players.at(activePlayer);
    HistoryElement h = HistoryElement();
    h.action = static_cast<PlayerAction>((int)PlayerAction::movement+(int)d);
    h.previousPosition = currentTile->getPosition();

    Tile* nextTile = getTile(
                currentTile->getPosition().rx() + changeOfXAxis,
                currentTile->getPosition().ry() + changeOfYAxis);
    if(nextTile == nullptr)
    {
        qDebug()<< "nullptr";

        return;
    }
    while(!currentTile->getWall(d)&& nextTile->getPlayer()==-1 && (currentTile->getPosition().x()!=targetX || currentTile->getPosition().y()!=targetY))
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
            break;
        }
        currentTile = nextTile;
        if(!nextTile->getWall(d))
        {
            nextTile = getTile(
                        currentTile->getPosition().rx() + changeOfXAxis,
                        currentTile->getPosition().ry() + changeOfYAxis);
        }
        setPlayerOnTile(activePlayer, currentTile);


    }

    emit playerMoved(activePlayer);
    moves++;
    if(goal == currentTile && seeker == activePlayer)
    {
        emit goalHit(moves);
    }
    history.append(h);

}

void Board::changeActivePlayer(int playerNumber)
{
    HistoryElement h = HistoryElement();
    h.action=PlayerAction::playerSwitch;
    h.previousPlayer=activePlayer;
    history.append(h);
    activePlayer = playerNumber;
    emit boardChanged();
}

void Board::revert()
{
    if(!history.isEmpty())
    {
        HistoryElement h = history.takeLast();
        if(h.action & PlayerAction::movement)
        {
            int direction = h.action-PlayerAction::movement;
            direction = direction>(int) Direction::east ? direction>>2 : direction<<2; //invert direction
            moveActivePlayer(static_cast<Direction>(direction), h.previousPosition.x(), h.previousPosition.y());
            moves--;
        }
        if(h.action == PlayerAction::playerSwitch)
        {
            changeActivePlayer(h.previousPlayer);
        }
        //remove the new written history
        history.removeLast();
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

int Board::addPlayer(Tile * t)
{
    qDebug() << "Board::switchPlayer(Tile * t)";
    players.append(t);
    t->setPlayer(players.size() - 1);
    return t->getPlayer();
}

void Board::updateColors(QColor b, QColor w, QColor g)
{
    background = b;
    primary = w;
    grid = g;
    emit boardChanged();
}

// This method is called with a direction that indicates the way the gamer wants to switch the player.
// We compute the angle each player has from the activePlayer (up would be 360/0 degrees, then it goes clockwise)
// As well as the distance each playe has from the active one
// From these two values a Fittingscore is computed and the player with the SMALLEST one is chosen as the next active player

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

    for(Tile * t:players)
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
        //        qDebug() << "Ergebnis" << min->getPlayer();
        changeActivePlayer(min->getPlayer());
        return min->getPlayer();
    }
    else{
        return activePlayer;
    }
}

void Board::resetMoves(){moves = 0;}





