#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QPen>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QSequentialAnimationGroup>

#include "pawnwidget.h"
#include "board.h"

class PlayerWidget : public PawnWidget
{
    Q_OBJECT
public:    
    explicit PlayerWidget(QSize size, int playerNumber, Board *board, QWidget *parent= nullptr);
    void moveAnimated(QPoint point);
    bool resizeWhileAnimation(double widthFactor, double heightFactor);
private:
    int playerNumber;
    bool debugMode = false;
    double fractionOfTile = 0.7;
    QSequentialAnimationGroup* animations = new QSequentialAnimationGroup();

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
