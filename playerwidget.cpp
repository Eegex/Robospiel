
#include <QPropertyAnimation>
#include "playerwidget.h"
#include "QDebug"
#include <algorithm>

PlayerWidget::PlayerWidget(QSize size,  int playerNumber, Board *board, QWidget *parent):
	PawnWidget(size, board, parent), playerNumber(playerNumber)
{
    animationTimer=new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &PlayerWidget::animationUpdate);
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

void PlayerWidget::moveAnimated(QPoint point, double speed) //speed in pixels per second
{

    double distance;
    if(animations.size() > 0)
    {
        distance = PlayerWidget::length(animations.last()->target-point);
    }
    else
    {
        distance = PlayerWidget::length(this->pos()-point);
    }

    qDebug()<<"move animated";
    Animation* animation = new Animation;
    animation->target=point;
    qDebug()<<"target="<<point;
//    animation->duration=1000*distance/speed;
    animation->endTime=QTime::currentTime().msecsSinceStartOfDay()+6000;
    animation->progress=0;

    if(animations.size()==0)
    {
        animationTimer->start(animationRate/1000);
    }
    animations.append(animation);
}


void PlayerWidget::animationUpdate()
{
    qDebug()<<"a";
    if(animations.size()>0)
    {
        if(animations.at(0)->progress>=1)
        {
            qDebug()<<"ERROR";
        }
        double remainingPercentage = 1-animations.at(0)->progress;
        QPoint deltaBefore = animations.at(0)->target-pos();
        double remainingTime = animations.at(0)->endTime-QTime::currentTime().msecsSinceStartOfDay();
        double remainingFrames = animationRate*remainingTime;
        QPoint wayInThisFrame = deltaBefore/remainingFrames;
        double progressInThisFrame = remainingPercentage*PlayerWidget::length(wayInThisFrame)/PlayerWidget::length(deltaBefore);

        qDebug()<<remainingPercentage<<remainingTime<<remainingFrames;
        qDebug()<<deltaBefore<<"+"<<wayInThisFrame<<progressInThisFrame;

        move(pos()+wayInThisFrame);
        animations.at(0)->progress+=progressInThisFrame;

        QPoint deltaAfter = animations.at(0)->target-pos();
        qDebug()<<"End: "<<pos()<<deltaAfter;

        if(deltaAfter.isNull())
        {
            qDebug()<<"Animation ended";
            move(animations.at(0)->target);
            if(animations.size()==1)
            {
                qDebug()<<"all animation ended";
                animationTimer->stop();
                emit reposition(playerNumber);

            }
            animations.remove(0);
        }
    }
}

double PlayerWidget::length(QPoint vector)
{
    return std::sqrt(std::pow(vector.x(), 2) + std::pow(vector.y(), 2));
}

bool PlayerWidget::resizeWhileAnimation(double widthFactor, double heightFactor)
{
//	if(animations->currentAnimation())
//	{
//		for(int i=animations->indexOfAnimation(animations->currentAnimation()); i<animations->animationCount(); i++)
//		{
//			QPropertyAnimation* animation=dynamic_cast<QPropertyAnimation*>(animations->animationAt(i));
//			if(animation)
//			{
//				//change the endPosition
//				QRect oldEndRect = animation->endValue().toRect();
//				int newX = round(oldEndRect.x()*widthFactor);
//				int newY = round(oldEndRect.y()*heightFactor);
//				animation->setEndValue(QRect(newX, newY, 0, 0));

//				//change the current position
//				QRect currentPosition = animation->currentValue().toRect();
//				move(QPoint(round(currentPosition.x()*widthFactor), round(currentPosition.y()*heightFactor)));
//			}
//		}
//		return true;
//	}
//	return false;
    return true; //TODO


}
