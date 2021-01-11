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
