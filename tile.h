#ifndef TILE_H
#define TILE_H

#include <QObject>

#include "Direction.h"

class Tile : public QObject
{
    Q_OBJECT
public:
    Tile(QPoint position, Tile *north, Tile *west, QObject *parent);

    QPoint getPosition() const;
    void setPosition(const QPoint &value);

    void setWall(Direction direction, bool set);
    bool getWall(Direction direction);
private:
    QPoint position;
    bool eastWall=false;
    bool southWall=false;
    Tile* northTile;
    Tile* westTile;
    int playerOnThisTile;


signals:

};

#endif // TILE_H
