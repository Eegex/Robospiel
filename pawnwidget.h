#ifndef PAWNWIDGET_H
#define PAWNWIDGET_H

#include <QWidget>
#include <QPoint>
#include <QMouseEvent>
#include <QDebug>
#include "board.h"

class PawnWidget : public QWidget
{
	Q_OBJECT
public:
	explicit PawnWidget(QSize size, Board* board, QWidget *parent = nullptr);
	void move(QPoint point);
	void setEditable(bool value = true);

	QColor getPlayerColor(int playerIndex);
protected:
	Board* board;
	bool editable = false;
	bool dragging = false;
	QPoint offset;
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);

	bool ownPlayerColors = true;


signals:
	void placeMe(PawnWidget * me);
};

#endif // PAWNWIDGET_H
