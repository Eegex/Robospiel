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





    placeOuterWalls();
    placeInnerWalls();

    //goal only in corner?

    //goal=getRandomUnoccupiedTile();
    placeGoalInCorner();


    //option where goals can't be reached in one step? Goal does not have an associated player yet, does it?

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

    int numberOfOuterWalls = (int) (outerWallspots/AVG_DIST_OF_OUTER_WALLS + 1);


    int i =0;

    std::default_random_engine generator;
    std::uniform_int_distribution<int> randomXIndex(1,(tiles.at(0).length()-3));
    std::uniform_int_distribution<int> randomYIndex(1,(tiles.length()-3));

    std::uniform_int_distribution<int> randomDirectionIndex(1, 2);
    std::uniform_int_distribution<int> randomSideIndex(1, 2);
    while(i<numberOfOuterWalls){




        int x= randomXIndex(generator);
        int y = randomYIndex(generator);

        Direction randDir = getNextDirection(Direction::east, randomDirectionIndex(generator));

        int randSide = 0;


        if(randDir == Direction::west){


            if(randomSideIndex(generator) ==1 ){

                randSide = 0;
            }
            else{

                randSide= tiles.length()-1;

            }

            if(!placeOuterWallIfFits(tiles.at(randSide).at(x), Direction::east)){
                i--;
            }


        }
        else{

            if(randomSideIndex(generator) ==1 ){

                randSide = 0;
            }
            else{

                randSide= tiles.at(0).length()-1;

            }
            if(!placeOuterWallIfFits(tiles.at(y).at(randSide), Direction::south)){
                i--;
            }





        }

        i++;


    }







}

bool Board::placeOuterWallIfFits(Tile* tile, Direction direction){

    if(direction == Direction::north||direction == Direction::west){

        qDebug()<< "Wrong direction as argument in placeOuterWallsIfFits!";
    }
    else{

        int x = tile->getPosition().x();
        int y = tile->getPosition().y();

        if(direction== Direction::east){

            if(!tiles.at(y).at(x-1)->getWall(direction)&&!tiles.at(y).at(x+1)->getWall(direction)){

                tile->setWall(direction, true);
                return true;
            }
        }

        if(direction== Direction::south){

            if(!tiles.at(y-1).at(x)->getWall(direction)&&!tiles.at(y+1).at(x)->getWall(direction)){

                tile->setWall(direction, true);
                return true;
            }
        }

    }

    return false;






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
                    &&!tiles.at(y+1).at(x)->getWall(Direction::west)
                    //&&!tiles.at(y+1).at(x)->getWall(Direction::east)
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


void Board::placeGoalInCorner(){


    bool noCorner = true;


    while(noCorner){

        int numberOfWalls = 0;

        goal = getRandomUnoccupiedTile();
        for(int i = 0; i<4; i++){
            Direction dir = getNextDirection(Direction::north, i);
                       if(goal->getWall(dir)){
                numberOfWalls++;
            }
        }
        noCorner = numberOfWalls<2;
        numberOfWalls=0;


    }
}





Direction Board::getNextDirection(Direction direction, int numberOfClockwiseSteps){

    if(numberOfClockwiseSteps ==0){
        return direction;
    }

    if(numberOfClockwiseSteps<0){
        qDebug()<< "getNextDirection was called with an int lower than 0";
    }


    if(numberOfClockwiseSteps > 1){
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




