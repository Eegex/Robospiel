#ifndef PAWNWIDGET_H
#define PAWNWIDGET_H

#include <QWidget>
#include <QPoint>
#include "board.h"

class PawnWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PawnWidget(QSize size, Board* board, QWidget *parent = nullptr);
    void move(QPoint point);


protected:
    Board* board;

    
signals:
    
};

#endif // PAWNWIDGET_H
