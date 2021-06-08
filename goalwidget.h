#ifndef GOALWIDGET_H
#define GOALWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QPen>
#include <QDebug>
#include <QPaintEvent>
#include "playerwidget.h"

class GoalWidget : public PawnWidget
{
	Q_OBJECT
	void paintEvent(QPaintEvent *event);
public:
	GoalWidget(QSize size, Board* board, QWidget *parent);
protected:
	void mousePressEvent(QMouseEvent * event);
private:
	const double fractionOfTile = 0.3; //how large is the goal in contrast to a tile
};

#endif // GOALWIDGET_H
