#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include <QRandomGenerator>
#include <QVector>
#include <QTime>
#include <QSize>
#include <QVector>
#include "tile.h"


class Board : public QObject
{
	Q_OBJECT
public:
	Board(int width, int height, int playerNumber, QObject *parent = nullptr);
	Tile *getTile(int x, int y);
	QSize getSize();
	QString printDirection(Direction direction);
	QVector<Tile*> players;
	Tile* goal;
	int seeker = 0;
	int activePlayer = 0;
	void moveActivePlayer(Direction d);
	void setPlayerOnTile(int player, Tile *tile);
    void changeActivePlayer(int playerNumber);
public slots:
	void startNewRound();
signals:
	void boardChanged();
    void playerMoved(int playerNumber);
    void goalMoved();
protected:
	void placeGoalAwayFromSeeker();
private:
	QVector<QVector<Tile*>> tiles;
	QRandomGenerator * r = nullptr;
	//TODO history
	//TODO active pawn

	Tile *getRandomUnoccupiedTile();
	// for each side: -1 because there are more tiles than walls, -2 because the walls next to the sides shouldn't be set, so 3*4 = 12
	const int NUM_PLACES_THAT_CANT_HAVE_OUTER_WALLS = 12;
	const int SPREAD_FACTOR_INNER_WALLS = 15;
    const int NUMBER_OF_UNUSED_OUTER_POSITIONS = 3;

	const int AVG_DIST_OF_OUTER_WALLS = 4;
	void placeInnerWalls();
	void placeOuterWalls();
	bool placeInnerWallifFits(Tile *tile, Direction direection);
	Direction getNextDirection(Direction direction, int numberOfClockwiseSteps);
	bool placeOuterWallIfFits(Tile *, Direction direction);
	void placeGoalInCorner();
};

#endif // BOARD_H
