#include "tile.h"

#include <QJsonObject>

/**
 * @brief Tile::Tile Represents a tile of the board.
 * In each tile only information about east and south walls are stored to prevent duplicate information when representing a complete board with multiple tiles.
 * When the north/west neighbour doesn't exist, the tile has a wall in this direction.
 * @param position x and y coordiantes in relation to the whole board.
 * @param north The upper neighbour
 * @param west The left neighbour
 * @param parent
 */
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
/**
 * @brief After the conversion of multiple tiles of a board, they have to be coupled by setting northTile and westTile.
 * @param json
 * @return the new tile
 */
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

/**
 * @brief Tile::setWall
 * @param direction
 * @param if set==false, existing walls are removed
 */
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

/**
 * @brief Tile::setInnerWall sets a wall at direction and one clockwise next to it
 * @param direction
 * @param if set==false, existing walls are removed
 */
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

/**
 * @brief Tile::getWall
 * @param direction
 * @return When the north/west neighbour is missing, a default wall in this direction is returned.
 */
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
