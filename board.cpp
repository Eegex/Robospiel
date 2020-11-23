#include <random>
#include <QDebug>
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

    placeOuterWalls();
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
//        pos += randStepSize(generator);
        positions.push_back(randStepSize(generator));

        qDebug()<< randStepSize(generator);
    }

    Tile* startTile = tiles.at(0).at(0);
    for(int i; i< positions.size(); i++){
        for(int j = 0; j < positions.front(); j ++){
            if(&startTile->westTile){

                startTile = startTile->westTile;
            }
            else{
                if(&startTile->northTile == nullptr){

                    startTile = startTile->northTile;
                }
                else{
                    Tile* startTile = tiles.at(0).at(0);

                }
            }
            //if(startTile->getPosition().x())

        }

        //startTile->setWall()
    }


}
