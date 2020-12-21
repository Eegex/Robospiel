#ifndef PAWNWIDGET_H
#define PAWNWIDGET_H

#include <QWidget>
#include <QPoint>

class PawnWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PawnWidget(QSize size, QWidget *parent = nullptr);
    void move(QPoint point);


private:

    
signals:
    
};

#endif // PAWNWIDGET_H
