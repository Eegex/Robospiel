#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QUuid>

class User : public QObject
{
    Q_OBJECT
public:
    User(QString name, QColor color, QObject *parent);
    QString getName();
    QColor getColor();
    QUuid getId();
    int getBidding();
    int getPoints();
    void setName(QString n);
    void setColor(QColor c);
    void setBidding(int b);
    void addPoints(int p);

private:
    QString name;
    QColor color;
    QUuid id;
    int bidding;
    int points;

signals:


};

#endif // USER_H
