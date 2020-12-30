#include "pawnwidget.h"
PawnWidget::PawnWidget(QSize size, Board* board, QWidget *parent) : QWidget(parent), board(board)
{
	setFixedSize(size);
}

void PawnWidget::move(QPoint point)
{
    QWidget::move(point);

}



