#include "playerwidget.h"



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
