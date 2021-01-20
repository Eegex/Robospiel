#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QPen>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPropertyAnimation>

#include "pawnwidget.h"
#include "board.h"
struct Animation
{
    QPropertyAnimation* animation;
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
private:
    int playerNumber;
    bool debugMode = true;
    double fractionOfTile = 0.7;
    QVector<Animation> animations;
    double MAX_DURATION = 5000.0;
    double MIN_DURATION = 10.0;

    double length(QPoint vector);
    double timeFactor(QPoint delta, double factorX, double factorY);
protected:

signals:
    void clicked(int playerNumber);
    void reposition(int playerNumber);



	// QWidget interface
protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);
	void paintEvent(QPaintEvent *event);
};

#endif // PLAYERWIDGET_H
