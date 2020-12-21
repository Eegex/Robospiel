#include "pawnwidget.h"

PawnWidget::PawnWidget(QSize size, QWidget *parent) : QWidget(parent)
{
    setFixedSize(size);


}

void PawnWidget::move(QPoint point)
{
    QWidget::move(point);

}



