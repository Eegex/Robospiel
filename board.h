#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include <QRandomGenerator>
#include <QVector>
#include <QTime>
#include <QSize>
#include <QColor>
#include <QVector>
#include "tile.h"

struct HistoryElement
{
	PlayerAction action;
	int previousPlayer;
	QPoint previousPosition;
};

class Board : public QObject
{
	Q_OBJECT
public:
	Board(int width, int height, int playerNumber, QObject *parent = nullptr);
	Tile *getTile(int x, int y);
	QSize getSize();
	QString printDirection(Direction direction);
	QVector<Tile*> players;
	Tile * goal;
	int seeker = 0;
	int activePlayer = 0;
	bool goalHit = 0;
	void moveActivePlayer(Direction d, int targetX = -1, int targetY = -1, bool isRevert = false);
	void setPlayerOnTile(int player, Tile *tile);
	void changeActivePlayer(int playerNumber, bool isRevert=false);
	void revert();
	void revertToBeginning();
	void makeNewBoard(int width, int height, int playerNumber);
	QJsonObject toJSON();
	static Board* fromJSON(QJsonObject json);
	int addPlayer(Tile * t);
	void resetMoves();
	QColor getBackground() const;
	QColor getPrimary() const;
	QColor getGrid() const;
	QColor getPlayerColorLow() const;
	QColor getPlayerColorHigh() const;
	void makeNewPlayers(int playerNumber);
	void makeNewWalls(int height, int width);
	void makeNewGoal();
	void makeNewSeeker(bool random);
	int getMoves() const;
	void updateColors(QColor b, QColor w, QColor g, QColor p1, QColor p2);
	QString toBinary();
	static Board * fromBinary(const QString base64);
public slots:
	void startNewRound();
	int switchPlayer(Direction d);
signals:
	void boardChanged();
	void paintPlayers();
	void playerMoved(int playerNumber, int goalHit); //-1 when goal was not reached, number of moves otherwise
	void goalMoved();
protected:
	void placeGoalAwayFromSeeker();
private:
	QVector<QVector<Tile*>> tiles;
	QRandomGenerator * r = nullptr;
	int moves = 0;
	QVector<HistoryElement> history;
	QColor background = QColor(0xff,0xff,0);
	QColor primary = QColor(0xff,0,0xff);
	QColor grid = QColor(0,0xff,0xff);
	QColor playerHigh = QColor(0,0xff,0xff);
	QColor playerLow = QColor(0,0xff,0xff);


	Tile *getRandomUnoccupiedTile();
	// for each side: -1 because there are more tiles than walls, -2 because the walls next to the sides shouldn't be set, so 3*4 = 12
	const int NUM_PLACES_THAT_CANT_HAVE_OUTER_WALLS = 12;
	const int SPREAD_FACTOR_INNER_WALLS = 15;
	const int NUMBER_OF_UNUSED_OUTER_POSITIONS = 2;

	const int AVG_DIST_OF_OUTER_WALLS = 4;
	void placeInnerWalls();
	void placeOuterWalls();
	bool placeInnerWallifFits(Tile *tile, Direction direection);
	Direction getNextDirection(Direction direction, int numberOfClockwiseSteps);
	bool placeOuterWallIfFits(Tile *, Direction direction);
	void placeGoalInCorner();
	Board();
	bool isTileCorner(Tile *tile);
	Tile *getRandomTile();
};

#endif // BOARD_H
