#include "goalwidget.h"
#include <QPainter>
#include <QColor>
#include <QPen>
#include <QPaintEvent>

GoalWidget::GoalWidget(QSize size, Board *board, QWidget *parent):
    PawnWidget(size, board, parent)
{

}

void GoalWidget::paintEvent(QPaintEvent *event)
{


    int playerNum = board->players.length();
    double stepSize = 359/playerNum;
    QColor color;
    color.setHsv(board->seeker*stepSize,200,200);

    QPainter painter;

    QPen pen;
    pen.setColor(color);
    if(painter.begin(this))
    {

        painter.setBrush(color);

        painter.setPen(pen);
        painter.drawEllipse(this->rect());

        painter.end();
    }

    event->accept();
}
