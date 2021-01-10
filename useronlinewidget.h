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
private:
    QGridLayout * lay = new QGridLayout(this);
    QPushButton * addBtn = new QPushButton(this);
    QPushButton * bidBtn = new QPushButton(this);
    QLineEdit * nameField = new QLineEdit(this);
    QLabel * name = new QLabel(this);
    QSpinBox * biddingBox = new QSpinBox(this);
    QListWidget * listWidget = new QListWidget(this);
    int userBidding = 999;
public slots:
    void setUser();
    void btnPressed();
signals:
    void userAdded(struct UserData * newUser);
    void biddingChangedOnline(int userBidding);
};

#endif // USERONLINEWIDGET_H
