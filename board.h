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

};

#endif // BOARD_H
