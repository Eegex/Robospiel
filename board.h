#ifndef BOARD_H
#define BOARD_H

#include <QObject>

#include "tile.h"


class Board : public QObject
{
    Q_OBJECT
public:

    Board(QObject *parent, int width, int height, int playerNumber);
    Tile *getTile(int x, int y);
private:
    QVector<QVector<Tile*>> tiles;
    QVector<Tile*> players;
    Tile* goal;
    //TODO history
    //TODO active user

    Tile *getRandomUnoccupiedTile();
    // for each side: -1 because there are more tiles than walls, -2 because the walls next to the sides shouldn't be set, so 3*4 = 12
    const int NUM_PLACES_THAT_CANT_HAVE_OUTER_WALLS = 12;
    const int AVG_DIST_OF_OUTER_WALLS = 5;
    const int SPREAD_FACTOR_OF_OUTER_WALLS = 2;
    void placeOuterWalls();
};

#endif // BOARD_H
