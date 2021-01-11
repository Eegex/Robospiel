#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QPen>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QSequentialAnimationGroup>
#include <QPoint>
#include <QVector>
#include <QTimer>

#include "pawnwidget.h"
#include "board.h"

struct Animation
{
    QPoint target;
    int endTime;
    double progress;
};


class PlayerWidget : public PawnWidget
{
    Q_OBJECT
public:    
    explicit PlayerWidget(QSize size, int playerNumber, Board *board, QWidget *parent= nullptr);
    void moveAnimated(QPoint point, double speed);
    bool resizeWhileAnimation(double widthFactor, double heightFactor);
private:
    int playerNumber;
    bool debugMode = true;
    double fractionOfTile = 0.7;
    QVector<Animation*> animations;
    QTimer* animationTimer;
    double animationRate = 2;

    static double length(QPoint vector);
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
private slots:
    void animationUpdate();
};

#endif // PLAYERWIDGET_H
