
#include <QPropertyAnimation>
#include "playerwidget.h"
#include "QDebug"

PlayerWidget::PlayerWidget(QSize size,  int playerNumber, Board *board, QWidget *parent):
	PawnWidget(size, board, parent), playerNumber(playerNumber)
{

}

void PlayerWidget::mousePressEvent(QMouseEvent *event)
{
	PawnWidget::mousePressEvent(event);
	if(!editable)
	{
		emit clicked(playerNumber);
		event->accept();
	}
}




void PlayerWidget::paintEvent(QPaintEvent *event)
{

	QRect bounds = rect();
	double width = bounds.width()*fractionOfTile;
	double height = bounds.height()*fractionOfTile;

	QColor color = getPlayerColor();
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
			QColor debugColor = *new QColor(0,0,0);
			if(getPlayerColor().value()<200){
				debugColor = *new QColor(255,255,255);
			}
			pen.setColor(debugColor);
			painter.setPen(pen);
			painter.drawText(this->rect().center(), QString::number(playerNumber));
		}
		painter.end();
	}
	event->accept();
}

/*!
 * \brief PlayerWidget::getPlayerColor
 * \return computes a color for the player based on color settings of the board.
 */
QColor PlayerWidget::getPlayerColor()
{
	return getPlayerColor(board, playerNumber); //this is just a guess, I hope that playerNumber is really the index
}

/*!
 * \brief PlayerWidget::getPlayerColor
 * \param playerIndex
 * \return computes a color for the playerIndex-th player based on color settings of the board.
 */
QColor PlayerWidget::getPlayerColor(Board* board, int playerIndex) //maybe not the perfect place for this method... but playerWidgets and goalwidgets need it and it's right there...
{

	int playerNum = board->players.length();
	QColor color;

//    if(ownPlayerColors)
//    {
//        QColor colorLow = board->getPlayerColorLow();
//        QColor colorHigh = board->getPlayerColorHigh();
//        playerNum --; //we want the endColor to be displayed!
//        double hueStep = (colorLow.hue() - colorHigh.hue())/playerNum;
//        double satStep = (colorLow.saturation() - colorHigh.saturation())/playerNum;
//        double brightStep = (colorLow.value() - colorHigh.value())/playerNum;
//        color.setHsv(colorLow.hue() - playerIndex*hueStep,colorLow.saturation() - playerIndex*satStep, colorLow.value() - playerIndex*brightStep);
//        //if(playerIndex ==0)
//        //{
//        //	qDebug() << "Origin" << colorLow << colorHigh;
//        //	qDebug() << "Farbelow" << colorLow.hue() << colorLow.saturation() << colorLow.value();
//        //	qDebug() << "Farbehigh" << colorHigh.hue() << colorHigh.saturation() << colorHigh.value();
//        //}
//        //qDebug() << "Farbe: " << playerIndex << color.hue() << color.saturation() << color.value();
//        //qDebug() << "Steps: " << hueStep << satStep << brightStep;
//    }
//    else
//    {
		double stepSize = 359/playerNum;
		color.setHsv(playerIndex*stepSize,200,200);
//    }
	return color;
}


/*!
 * \param vector
 * \return length of the vector
 */
double PlayerWidget::length(QPoint vector)
{
	return sqrt(pow(vector.x(), 2) + pow(vector.y(), 2));
}

/*!
 * \brief PlayerWidget::moveAnimated
 * \param point the coordinates in the window
 * \param target the coordinates of the target tile
 * \param speed of the animation in pixels per second
 */
void PlayerWidget::moveAnimated(QPoint point, QPoint target, double speed)
{
	qDebug()<<"Called Function MoveAnimated!!!!";
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

	auto startAnimation = [&](QPropertyAnimation * a)
	{
		double duration = std::min((0.9 + std::pow(animations.size(),2) * 0.05),1000.0);
		a->setDuration(a->duration() / duration);
		a->start();
	};

	QPropertyAnimation * animation = new QPropertyAnimation(this, "geometry");
	animation->setDuration(1000*distance/speed);
	animation->setEndValue(QRect(point.x(), point.y(), 0, 0));
	animation->setEasingCurve(QEasingCurve::InQuad);
	connect(animation, &QAbstractAnimation::finished, this, [=]()->void
	{
		animations.remove(0);
		if(animations.isEmpty())
		{
			emit reposition(playerNumber);
		}
		else
		{
			startAnimation(animations.at(0).animation);
		}
	});
	Animation animationStruct;
	animationStruct.animation=animation;
	animationStruct.target=target;

	animations.append(animationStruct);
	if(animations.at(0).animation->state() != QAbstractAnimation::Running)
	{
		startAnimation(animations.at(0).animation);
	}
}

double PlayerWidget::timeFactor(QPoint delta, double factorX, double factorY)
{
	double xProportion = delta.x()/std::min((delta.x()+delta.y()),1);

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
		connect(animation, &QAbstractAnimation::finished, this, [=]()->void
		{
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

void PlayerWidget::setPlayer(int value)
{
	playerNumber = value;
}
