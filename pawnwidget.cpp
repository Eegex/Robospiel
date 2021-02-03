#include "pawnwidget.h"
PawnWidget::PawnWidget(QSize size, Board* board, QWidget *parent) : QWidget(parent), board(board)
{
	setFixedSize(size);
}

void PawnWidget::move(QPoint point)
{
	QWidget::move(point);
}

void PawnWidget::setEditable(bool value)
{
	qDebug() << "PawnWidget::setEditable(bool value)";
	editable = value;
}

void PawnWidget::mousePressEvent(QMouseEvent * event)
{
	if(editable && event->button() == Qt::LeftButton)
	{
		offset = event->pos();
		QRect innerRect = rect();
		innerRect.moveTopLeft(QPoint(5,5));
		innerRect.setSize(innerRect.size() - QSize(10,10));
		if(innerRect.contains(offset))
		{
			dragging = true;
		}
		event->accept();
	}
}

void PawnWidget::mouseReleaseEvent(QMouseEvent * event)
{
	dragging = false;
	if(editable)
	{
		emit placeMe(this);
	}
	event->accept();
}

void PawnWidget::mouseMoveEvent(QMouseEvent * event)
{
	if(editable && dragging)
	{
		move(mapToParent(event->pos()) - offset);
	}
	event->accept();
}

QColor PawnWidget::getPlayerColor(int playerIndex){ //maybe not the perfect place for this method... but playerWidgets and goalwidgets need it and it's right there...
    QColor colorLow = board->getPlayerColorLow();
    QColor colorHigh = board->getPlayerColorHigh();
    int playerNum = board->players.length();
     QColor color;

    if(ownPlayerColors){

        playerNum --; //we want the endColor to be displayed!

        double hueStep = (colorLow.hue() - colorHigh.hue())/playerNum;
        double satStep = (colorLow.saturation() - colorHigh.saturation())/playerNum;
        double brightStep = (colorLow.value() - colorHigh.value())/playerNum;


        color.setHsv(colorLow.hue() - playerIndex*hueStep,colorLow.saturation() - playerIndex*satStep, colorLow.value() - playerIndex*brightStep);

//        if(playerIndex ==0){
//            qDebug() << "Origin" << colorLow << colorHigh;
//            qDebug() << "Farbelow" << colorLow.hue() << colorLow.saturation() << colorLow.value();
//            qDebug() << "Farbehigh" << colorHigh.hue() << colorHigh.saturation() << colorHigh.value();
//        }

//        qDebug() << "Farbe: " << playerIndex << color.hue() << color.saturation() << color.value();

//        qDebug() << "Steps: " << hueStep << satStep << brightStep;

    }

    else{

        int playerNum = board->players.length();
            double stepSize = 359/playerNum;

            color.setHsv(playerIndex*stepSize,200,200);
    }




    return color;

}

