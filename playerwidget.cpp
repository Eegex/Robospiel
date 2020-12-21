#include "playerwidget.h"



PlayerWidget::PlayerWidget(QSize size, int playerNumber, Board *board, QWidget *parent):
    PawnWidget(size, parent), playerNumber(playerNumber), board(board)
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


    int playerNum = board->players.length();
    double stepSize = 359/playerNum;
    QColor color;
    color.setHsv(playerNumber*stepSize,200,200);

    QPainter painter;

    QPen pen;
    pen.setColor(color);
    if(painter.begin(this))
    {
        if(board->activePlayer==playerNumber)
        {
        painter.setBrush(color.lighter(200));
        }
        else
        {
            painter.setBrush(color);
        }
        painter.setPen(pen);
        painter.drawEllipse(this->rect());

        if(debugMode){
            painter.drawText(this->rect(), QString::number(playerNumber));
        }

        painter.end();
    }

    event->accept();
}
