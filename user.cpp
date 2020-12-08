#include "user.h"

User::User(std::string name, QColor color, QObject *parent) : QObject(parent), name(name), color(color)
{
}

std::string User::getName()
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

void User::setName(std::string n)
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
