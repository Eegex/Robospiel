#include "tile.h"

Tile::Tile(QPoint position, Tile* north, Tile* west, QObject *parent) :  QObject(parent), position(position), northTile(north), westTile(west), playerOnThisTile(-1)
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

//sets a wall at direction and one clockwise next to it

void Tile::setInnerWall(Direction direction, bool set)
{
	switch(direction)
	{
	case Direction::north:
		if(northTile)
		{
			northTile->setWall(Direction::south, set);
		}
		eastWall = set;
		break;
	case Direction::east:
		eastWall=set;
		southWall=set;

		break;
	case Direction::south:
		if(westTile)
		{
			westTile->setWall(Direction::east, set);
		}
		southWall=set;
		break;
	case Direction::west:
		if(westTile)
		{
			westTile->setWall(Direction::east, set);
		}
		if(northTile)
		{
			northTile->setWall(Direction::south, set);
		}
		break;
	}

}

bool Tile::getWall(Direction direction)
{
	switch(direction)
	{
	case Direction::north:
		return northTile ? northTile->getWall(Direction::south) : true;
	case Direction::east:
		return eastWall;
	case Direction::south:
		return southWall;
	case Direction::west:
		return westTile ? westTile->getWall(Direction::east) : true;
	}
}

int Tile::getPlayer() const
{
	return playerOnThisTile;
}

void Tile::setPlayer(int value)
{
	playerOnThisTile = value;
}

//TODO bool Square::isBlocked()

