#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QPen>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QPoint>
#include <QVector>
#include <QTimer>

#include "pawnwidget.h"
#include "board.h"
struct Animation
{
	QPropertyAnimation* animation = nullptr;
	QPoint target;
};

class PlayerWidget : public PawnWidget
{
	Q_OBJECT
public:
	explicit PlayerWidget(QSize size, int playerNumber, Board *board, QWidget *parent= nullptr);
	void moveAnimated(QPoint point, QPoint target, double speed);
	bool resizeWhileAnimation(QVector<QPoint> newTargets, QPoint newPosition, double factorX, double facotrY);
	QVector<QPoint> getTargets();
	bool getInAnimation();
	void setPlayer(int value);
	static QColor getPlayerColor(Board* board, int playerIndex);
private:
	int playerNumber;
	bool debugMode = true;

	double fractionOfTile = 0.7;
	QVector<Animation> animations;
	double MAX_DURATION = 5000.0;
	double MIN_DURATION = 10.0;

	double length(QPoint vector);
	double timeFactor(QPoint delta, double factorX, double factorY);
	QColor getPlayerColor();
protected:
	void mousePressEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);

signals:
	void clicked(int playerNumber);
	void reposition(int playerNumber);
};

#endif // PLAYERWIDGET_H
