#include "tile.h"


#include <QPoint>

Tile::Tile(QPoint position, Tile* north, Tile* west, QObject *parent) :  QObject(parent), position(position), northTile(north), westTile(west)
{
}

QPoint Tile::getPosition() const
{
    return position;
}

void Tile::setPosition(const QPoint &value)
{
    position = value;
}

void Tile::setWall(Direction direction, bool set)
{
    switch(direction)
    {
        case Direction::north:
            if(northTile)
            {
                northTile->setWall(Direction::south, set);
            }
            break;
        case Direction::east:
            eastWall=set;
            break;
        case Direction::south:
            southWall=set;
            break;
        case Direction::west:
            if(westTile)
            {
                westTile->setWall(Direction::east, set);
            }
            break;
    }
}

bool Tile::getWall(Direction direction)
{
    switch(direction)
    {
        case Direction::north:
            northTile ? northTile->getWall(Direction::south) : true;
        case Direction::east:
            return eastWall;
        case Direction::south:
            return southWall;
        case Direction::west:
            westTile ? westTile->getWall(Direction::east) : true;
    }
}

//TODO bool Square::isBlocked()

