#ifndef TILE_H
#define TILE_H

#include <QObject>
#include <QPoint>
#include "Direction.h"

class Tile : public QObject
{
	Q_OBJECT
public:
	Tile(QPoint position, Tile *north, Tile *west, QObject *parent);
	Tile(const Tile & tile);
	QPoint getPosition() const;
	void setPosition(const QPoint &value);

	void setWall(Direction direction, bool set);
	bool getWall(Direction direction);
	int getPlayer() const;
	void setPlayer(int value);
	Tile * northTile = nullptr;
	Tile * westTile = nullptr;
	Tile * copyTile(Tile* tile);

	void setInnerWall(Direction direction, bool set);
	static Tile * fromJSON(QJsonObject json);
	QJsonObject toJSON();
private:
	QPoint position;
	bool eastWall=false;
	bool southWall=false;
	int playerOnThisTile;
	Tile();


signals:

};

#endif // TILE_H
