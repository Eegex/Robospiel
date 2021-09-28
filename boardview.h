#ifndef BOARDVIEW_H
#define BOARDVIEW_H

#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QPixmapCache>
#include <QRandomGenerator>
#include <QRect>
#include <QMouseEvent>
#include "QDebug"
#include "board.h"
#include "keymapping.h"
#include "playerwidget.h"
#include "goalwidget.h"
#include <algorithm>
#include <QTimer>
#include <QMargins>
#include <QGestureEvent>
#include <QSwipeGesture>


/*!
 * \class BoardView
 * \brief The BoardView class displays a Board
 */
class BoardView : public QWidget
{
	Q_OBJECT
public:
	explicit BoardView(QWidget *parent = nullptr);
	QSize sizeHint() const;
	virtual void setBoard(Board * b);
	void setDebugOutputEnabled(bool set = true);
	void resize(int pixelPerTile);
	void setMapping(QVector<KeyMapping *> *value);
	Board * getBoard();
	void makeNewAll();
	void makeNewPlayers();
	void makeNewWalls();
	void makeNewTarget();
	void makeNewSeeker();
protected slots:
	void paintEvent(QPaintEvent * event);
	void resizeEvent(QResizeEvent * event);
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);
	void keyPressEvent(QKeyEvent * event);
	void handleKeyPress(int key);
	bool event(QEvent * event);

protected:
	Tile * coordsToTile(QPoint p);
	Board * board = nullptr;
	QSize tileSize;
	QPoint mouseStart;
	int lastKey = 0;
	bool showDebugOutput = false;
	QVector<KeyMapping*> * mapping = nullptr;
	void callChangeActivePlayer(Tile *t);
	QVector<PlayerWidget* > playerWidgets;
	GoalWidget* goalwidget = nullptr;
	//void translateMapping(PlayerAction action);
	//void callMoveActivePlayer(Direction d);
	QPoint tileToDesktopCoordinates(Tile *tile);
	virtual PlayerWidget * addPlayer(int i);
	QResizeEvent currentEvent = QResizeEvent(QSize(-1,-1), QSize(-1,-1));
signals:
	void tileHovered(Tile * t);
	void tileClicked(Tile * t);
	void swipe(Direction d);
	void action(PlayerAction a, QString userName);
	void activePlayerChanged(int playerNumber);
	void animationEnded();
};

#endif // BOARDVIEW_H
