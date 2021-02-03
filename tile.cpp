#include "tile.h"

#include <QJsonObject>

Tile::Tile(QPoint position, Tile* north, Tile* west, QObject *parent) :  QObject(parent), northTile(north), westTile(west), position(position), playerOnThisTile(-1)
{
}

Tile::Tile(){}

QJsonObject Tile::toJSON()
{
	QJsonObject json;
	json.insert("eastWall", eastWall);
	json.insert("southWall", southWall);
	json.insert("playerOnThisTile", playerOnThisTile);
	json.insert("positionX", position.x());
	json.insert("positionY", position.y());

	return json;
}

//the tile still has to be coupled with its north and west neighbours
Tile* Tile::fromJSON(QJsonObject json)
{
	Tile* t = new Tile();
	t->eastWall = json.value("eastWall").toBool();
	t->southWall = json.value("southWall").toBool();
	t->playerOnThisTile = json.value("playerOnThisTile").toInt();
	t->position = QPoint(json.value("positionX").toInt(), json.value("positionY").toInt());
	return t;
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
	case Direction::none:
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
	case Direction::none:
		break;
	}

}

bool Tile::getWall(Direction direction)
{
	switch(direction)
	{
	case Direction::north:
	{
		return northTile ? northTile->getWall(Direction::south) : true;
	}
	case Direction::east:
	{
		return eastWall;
	}
	case Direction::south:
	{
		return southWall;
	}
	case Direction::west:
	{
		return westTile ? westTile->getWall(Direction::east) : true;
	}
	case Direction::none:
	{
		return false;
	}
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
