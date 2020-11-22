#ifndef BOARDVIEW_H
#define BOARDVIEW_H

#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QPixmapCache>
#include <QRect>
#include "board.h"

class BoardView : public QWidget
{
	Q_OBJECT
public:
	explicit BoardView(QWidget *parent = nullptr);
	QSize sizeHint() const;
	void setBoard(Board * b);
	void setDebugOutputEnabled(bool set = true);

private slots:
	void paintEvent(QPaintEvent * event);
	void resizeEvent(QResizeEvent * event);

private:
	void fillCache(QSize tileRect);
	Board * board = nullptr;
	QPixmapCache cache;
	bool showDebugOutput = false;
signals:

};

#endif // BOARDVIEW_H
