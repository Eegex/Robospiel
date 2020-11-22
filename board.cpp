#include <random>
#include "board.h"

Board::Board(QObject *parent, int width, int height, int playerNumber) : QObject(parent)
{
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
            //TODO generate walls
        }
        tiles.append(innerVector);
    }

    std::default_random_engine generator;
    std::uniform_int_distribution<int> x(0, width-1);
    std::uniform_int_distribution<int> y(0, height-1);
    for(int i=0; i<playerNumber; i++)
    {
        players.append(tiles.at(y(generator)).at(x(generator)));
    }

    goal=tiles.at(y(generator)).at(x(generator));
}

Tile* Board::getTile(int x, int y)
{
    return tiles.at(y).at(x);
}
