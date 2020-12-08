#include "user.h"

User::User(QString name, QColor color, QObject *parent) : QObject(parent), name(name), color(color)
{
}

QString User::getName()
{
    return name;
}
QColor User::getColor()
{
    return color;
}
int User::getTip()
{
    return tip;
}
int User::getPoints()
{
    return points;
}

void User::setName(QString n)
{
    name = n;
}
void User::setColor(QColor c)
{
    color = c;
}
void User::setTip(int t)
{
    tip = t;
}
void User::addPoints(int p)
{
    points += p;
}
