#include "playerwidget.h"
#include "QDebug"

#include <QPropertyAnimation>


PlayerWidget::PlayerWidget(QSize size,  int playerNumber, Board *board, QWidget *parent):
    PawnWidget(size, board, parent), playerNumber(playerNumber)
{
    connect(animations, &QSequentialAnimationGroup::finished, this, [=]()->void{
        animations->clear();
        emit reposition(playerNumber);
    });

}

void PlayerWidget::mousePressEvent(QMouseEvent *event)
{
    emit clicked(playerNumber);
    event->accept();
}

void PlayerWidget::mouseReleaseEvent(QMouseEvent *event)
{

}

void PlayerWidget::enterEvent(QEvent *event)
{

}

void PlayerWidget::leaveEvent(QEvent *event)
{
}

void PlayerWidget::paintEvent(QPaintEvent *event)
{
    QRect bounds = rect();
    double width = bounds.width()*fractionOfTile;
    double height = bounds.height()*fractionOfTile;

    int playerNum = board->players.length();
    double stepSize = 359/playerNum;
    QColor color;
    color.setHsv(playerNumber*stepSize,200,200);

    QPainter painter;

    QPen pen;
    pen.setColor(color);

    QSize offset = bounds.size() - QSize(width, height);

    bounds.moveTopLeft({(int) offset.width()/2, (int) offset.height()/2});
    bounds.setSize(QSize((int) width, (int) height));

    if(painter.begin(this))

    {

        painter.setBrush(color.darker(200));
        painter.setPen(pen);
        if(board->activePlayer == playerNumber)
        {
            painter.setBrush(color);
            painter.setPen(color.lighter(130));
        }
        painter.drawEllipse(bounds);
        if(debugMode)
        {
            pen.setColor(QColor(255,0,255));
            painter.setPen(pen);
            painter.drawText(this->rect().center(), QString::number(playerNumber));
        }
        painter.end();
    }
    event->accept();
}

void PlayerWidget::moveAnimated(QPoint point)
{
    double speed = 350; //in pixels per second

    double distance;
    if(animations->animationCount()>0)
    {
        QPropertyAnimation* lastAnimation=dynamic_cast<QPropertyAnimation*>(animations->animationAt(animations->animationCount()-1));
        distance = (lastAnimation->endValue().toRect().topLeft()-point).manhattanLength();
    }
    else
    {
        distance = (this->pos()-point).manhattanLength();
    }


    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(1000*distance/speed);
    animation->setEndValue(QRect(point.x(), point.y(), 0, 0));

    animations->addAnimation(animation);
    if(animations->state() != QAbstractAnimation::Running)
    {
        animations->start();
    }
}

bool PlayerWidget::resizeWhileAnimation(double widthFactor, double heightFactor)
{
    if(animations->currentAnimation())
    {
        for(int i=animations->indexOfAnimation(animations->currentAnimation()); i<animations->animationCount(); i++)
        {
            QPropertyAnimation* animation=dynamic_cast<QPropertyAnimation*>(animations->animationAt(i));
            if(animation)
            {
                //change the endPosition
                QRect oldEndRect = animation->endValue().toRect();
                int newX = round(oldEndRect.x()*widthFactor);
                int newY = round(oldEndRect.y()*heightFactor);
                animation->setEndValue(QRect(newX, newY, 0, 0));

                //change the current position
                QRect currentPosition = animation->currentValue().toRect();
                move(QPoint(round(currentPosition.x()*widthFactor), round(currentPosition.y()*heightFactor)));
            }
        }
        return true;
    }
    return false;


}
