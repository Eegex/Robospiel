#include "goalwidget.h"


GoalWidget::GoalWidget(QSize size, Board *board, QWidget *parent):
	PawnWidget(size, board, parent)
{
}

void GoalWidget::paintEvent(QPaintEvent *event)
{
	QRect bounds = rect();
	double width = bounds.width()*fractionOfTile;
	double height = bounds.height()*fractionOfTile;


    QColor color = PlayerWidget::getPlayerColor(board, board->seeker);


	QPainter painter;

	QPen pen;

	QSize offset = bounds.size() - QSize(width, height);

	bounds.moveTopLeft({(int) offset.width()/2, (int) offset.height()/2});
	bounds.setSize(QSize((int) width, (int) height));
	pen.setColor(color);
	if(painter.begin(this))
	{

		painter.setBrush(color);

		painter.setPen(pen);
		painter.drawEllipse(bounds);

		painter.end();
	}

	event->accept();
}

void GoalWidget::mousePressEvent(QMouseEvent * event)
{
	if(editable)
	{
		PawnWidget::mousePressEvent(event);
		event->accept();
	}
	else
	{
		event->ignore();
	}
}
