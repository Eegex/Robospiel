#include "user.h"
#include <QString>
#include <QColor>
#include <QUuid>


User::User(QString name, QColor color, QObject *parent) : QObject(parent), name(name), color(color)
{
    User::id = QUuid::createUuid();
}
QString User::getName()
{
	return name;
}
QColor User::getColor()
{
	return color;
}
QUuid User::getId()
{
    return id;
}
int User::getBidding()
{
    return bidding;
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
void User::setBidding(int b)
{
    bidding = b;
}
void User::addPoints(int p)
{
	points += p;
}
