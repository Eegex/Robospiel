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

    int playerNum = board->players.length();
    double stepSize = 359/playerNum;
    QColor color;
    color.setHsv(board->seeker*stepSize,200,200);

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
