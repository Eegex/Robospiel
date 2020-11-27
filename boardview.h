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
#include "board.h"
#include "keymapping.h"

class BoardView : public QWidget
{
	Q_OBJECT
public:
	explicit BoardView(QWidget *parent = nullptr);
	QSize sizeHint() const;
	void setBoard(Board * b);
	void setDebugOutputEnabled(bool set = true);
	void resize(int pixelPerTile);

private slots:
	void paintEvent(QPaintEvent * event);
	void resizeEvent(QResizeEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void keyPressEvent(QKeyEvent * event);

private:
	void fillCache(QSize tileRect);
	Tile * coordsToTile(QPoint p);
	Board * board = nullptr;
	QPixmapCache cache;
	QSize tileSize;
	QPoint mouseStart;
	bool cachedPaintig = true;
	bool showDebugOutput = false;
	QVector<KeyMapping> mapping;
signals:
	void tileHovered(Tile * t);
	void tileClicked(Tile * t);
	void swipe(Direction d);
	void action(PlayerAction a);

};

#endif // BOARDVIEW_H
