
#include <QPropertyAnimation>
#include "playerwidget.h"
#include "QDebug"

PlayerWidget::PlayerWidget(QSize size,  int playerNumber, Board *board, QWidget *parent):
	PawnWidget(size, board, parent), playerNumber(playerNumber)
{

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


double PlayerWidget::length(QPoint vector)
{
    return sqrt(pow(vector.x(), 2) + pow(vector.y(), 2));
}

/**
 * @brief PlayerWidget::moveAnimated
 * @param point the coordinates in the window
 * @param target the coordinates of the target tile
 * @param speed of the animation in pixels per second
 */
void PlayerWidget::moveAnimated(QPoint point, QPoint target, double speed)
{

	double distance;
    if(animations.size() > 0)
	{
        QPropertyAnimation * lastAnimation=dynamic_cast<QPropertyAnimation*>(animations.last().animation);
        distance = PlayerWidget::length(lastAnimation->endValue().toRect().topLeft()-point);
	}
	else
	{
        distance = PlayerWidget::length(this->pos()-point);
	}


	QPropertyAnimation * animation = new QPropertyAnimation(this, "geometry");
	animation->setDuration(1000*distance/speed);
	animation->setEndValue(QRect(point.x(), point.y(), 0, 0));
    animation->setEasingCurve(QEasingCurve::InQuad);
    connect(animation, &QAbstractAnimation::finished, this, [=]()->void{
        animations.remove(0);
        if(animations.isEmpty())
        {
            emit reposition(playerNumber);
        }
        else
        {
            animations.at(0).animation->start();
        }

    });
    Animation animationStruct;
    animationStruct.animation=animation;
    animationStruct.target=target;

    animations.append(animationStruct);
    if(animations.at(0).animation->state() != QAbstractAnimation::Running)
	{
        animations.at(0).animation->start();
	}
}

double PlayerWidget::timeFactor(QPoint delta, double factorX, double factorY)
{
    double xProportion = delta.x()/(delta.x()+delta.y());

    return xProportion*factorX + (1-xProportion)*factorY;
}

bool PlayerWidget::resizeWhileAnimation(QVector<QPoint> newTargets, QPoint newPosition, double factorX, double factorY)
{
    if(!animations.isEmpty() || newTargets.size()!=animations.size())
    {
        animations.at(0).animation->pause();

        for(int i=1; i<animations.size(); i++)
		{
            animations.at(i).animation->setEndValue(QRect(newTargets.at(i), QSize(0, 0)));
            double newDuration = animations.at(i).animation->duration()*timeFactor(newTargets.at(i)-newTargets.at(i-1), factorX, factorY);
            newDuration = std::max(std::min(newDuration, MAX_DURATION), MIN_DURATION);
            animations.at(i).animation->setDuration(newDuration);
		}

        //replace the current transition
        QPropertyAnimation * animation = new QPropertyAnimation(this, "geometry");
        double newDuration = (animations.at(0).animation->duration()-animations.at(0).animation->currentTime())*timeFactor(newTargets.at(0)-newPosition, factorX, factorY);
        newDuration = std::max(std::min(newDuration, MAX_DURATION), MIN_DURATION);
        animation->setDuration(newDuration);

        animation->setEndValue(QRect(newTargets.at(0), QSize(0, 0)));
        animation->setEasingCurve(QEasingCurve::InQuad);
        connect(animation, &QAbstractAnimation::finished, this, [=]()->void{
            animations.remove(0);
            if(animations.isEmpty())
            {
                emit reposition(playerNumber);
            }
            else
            {
                animations.at(0).animation->start();
            }

        });
        Animation animationStruct;
        animationStruct.animation=animation;
        animationStruct.target=animations.at(0).target;


        animations.at(0).animation->stop();
        animations.remove(0);
        //change the current position
        move(newPosition);

        animations.insert(0, animationStruct);
        animation->start();

		return true;
	}
	return false;


}

bool PlayerWidget::getInAnimation()
{
    return (!animations.isEmpty()) && animations.at(0).animation->state() == QAbstractAnimation::Running;
}

QVector<QPoint> PlayerWidget::getTargets()
{
    QVector<QPoint> targets;
    for(Animation animation : animations)
    {
        targets.append(animation.target);
    }
    return targets;

}
