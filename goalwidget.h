#ifndef GOALWIDGET_H
#define GOALWIDGET_H

#include "pawnwidget.h"
#include <QWidget>

class GoalWidget : public PawnWidget
{
    Q_OBJECT
    void paintEvent(QPaintEvent *event);
public:
    GoalWidget(QSize size, Board* board, QWidget *parent);
};

#endif // GOALWIDGET_H
