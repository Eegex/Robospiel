#include <random>
#include <QDebug>
#include <Direction.h>
#include "board.h"

Board::Board(QObject *parent, int width, int height, int playerNumber) : QObject(parent)
{

    if(playerNumber+1 > width*height){
        qDebug()<< "Board contructor was called with two many players!";
    }
    for(int i=0; i< height; i++)
    {
        QVector<Tile*> innerVector;
        for(int j=0; j<width; j++)
        {
            Tile* tile = new Tile(QPoint(j, i),
                                  tiles.isEmpty()?nullptr:tiles.last().at(j),
                                  innerVector.isEmpty()?nullptr: innerVector.last(), this);
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


    std::default_random_engine generator;
    std::uniform_int_distribution<int> x(0, width-1);
    std::uniform_int_distribution<int> y(0, height-1);
    for(int i=0; i<playerNumber; i++)
    {
        Tile * t = getRandomUnoccupiedTile();
        t->setPlayer(i);
        players.append(t);
    }

    goal=getRandomUnoccupiedTile();



    //placeOuterWalls();
    placeInnerWalls();


}

Tile* Board::getTile(int x, int y)
{
    return tiles.at(y).at(x);
}

QSize Board::getSize()
{
    return QSize(tiles.first().size(),tiles.size());
}

Tile* Board::getRandomUnoccupiedTile(){

    std::default_random_engine generator;
    std::uniform_int_distribution<int> x(0, tiles.at(0).length()-1);
    std::uniform_int_distribution<int> y(0, tiles.length()-1);
    bool tileIsValid = false;
    Tile* initialTile;
    int counter = 0;
    while(!tileIsValid){
        counter++;
        initialTile = tiles.at(y(generator)).at(x(generator));

        tileIsValid = true;
        for(Tile* player : players){
            if(player == initialTile){
                tileIsValid = false;
            }
        }
        if(goal == initialTile){
            tileIsValid = false;
        }
        if(counter>1000){
            qDebug() << "Can't find any unoccupied tile";
            return nullptr;
        }
    }

    return initialTile;

}

void Board::placeOuterWalls(){

    int outerWallspots = tiles.at(0).length()*2 + tiles.length()*2 - NUM_PLACES_THAT_CANT_HAVE_OUTER_WALLS;
    qDebug()<< outerWallspots;

    int numberOfOuterWalls = (int) (outerWallspots/AVG_DIST_OF_OUTER_WALLS + 1);
    qDebug()<< numberOfOuterWalls;

    std::default_random_engine generator;
    std::uniform_int_distribution<int> randStepSize(AVG_DIST_OF_OUTER_WALLS - SPREAD_FACTOR_OF_OUTER_WALLS, AVG_DIST_OF_OUTER_WALLS + SPREAD_FACTOR_OF_OUTER_WALLS);
    std::list<int> positions;
    int pos = 0;
    for(int i = 0; i <numberOfOuterWalls; i++){

        tiles.at(0).at(tiles.at(0).length()-NO_OUTER_WALLS_PER_SIDE + 1)->setWall(Direction::east, true);


    }

    tiles.at(0).at(0)->setWall(Direction::east, true);

}

void Board::placeInnerWalls(){



    int numberOfInnerWalls = (tiles.at(0).length()-2) *(tiles.length()-2) /SPREAD_FACTOR_INNER_WALLS;


    std::default_random_engine generator;
    std::uniform_int_distribution<int> randomXIndex(1,(tiles.at(0).length()-2));
    std::uniform_int_distribution<int> randomYIndex(1,(tiles.length()-2));
    std::uniform_int_distribution<int> randomDirectionIndex(1, 4);

    int i = 0;

    while(i<numberOfInnerWalls){
            i++;


    int x= randomXIndex(generator);
    int y = randomYIndex(generator);

    if(!placeInnerWallifFits(tiles.at(y).at(x), getNextDirection(Direction::north, randomDirectionIndex(generator)))){
        i--;
    }



    }







}

bool Board::placeInnerWallifFits(Tile* tile, Direction direction){

    int x = tile->getPosition().x();
    int y = tile->getPosition().y();



    if(!tile->getWall(Direction::north)
            &&!tile->getWall(Direction::east)
            &&!tile->getWall(Direction::south)
            &&!tile->getWall(Direction::west)

            )
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
                    //&&!tiles.at(y+1).at(x)->getWall(Direction::west)
                    &&!tiles.at(y+1).at(x)->getWall(Direction::east)
                    &&!tiles.at(y).at(x-1)->getWall(Direction::north)
                    &&!tiles.at(y).at(x-1)->getWall(Direction::south)
                    &&!tiles.at(y).at(x+1)->getWall(Direction::north);
                    //&&!tiles.at(y).at(x+1)->getWall(Direction::south);
            break;

        }



        if(noConflictWithNeighbors){


            tile->setInnerWall(direction, true);
            return true;



        }


    }
    return false;



}





Direction Board::getNextDirection(Direction direction, int numberOfClockwiseSteps){


    if(numberOfClockwiseSteps != 1){
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


std::string Board::printDirection(Direction direction){



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




