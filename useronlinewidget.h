#ifndef USERONLINEWIDGET_H
#define USERONLINEWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QListWidget>
#include "Direction.h"

class UserOnlineWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UserOnlineWidget(QWidget *parent = nullptr);
    void setBidding();
    void updateName(QString newName); // TODO: same for color
private:
    QGridLayout * lay = new QGridLayout(this);
    QPushButton * bidBtn = new QPushButton(this);
    QLabel * name = new QLabel(this);
    QSpinBox * biddingBox = new QSpinBox(this);
    QListWidget * listWidget = new QListWidget(this);
    int userBidding = 999;
    QString username = "";
    QColor usercolor;
public slots:
    void btnPressed();
signals:
    void userAdded(struct UserData * newUser);
    void biddingChangedOnline(int userBidding);
};

#endif // USERONLINEWIDGET_H
