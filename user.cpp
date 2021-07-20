#include "user.h"
#include <QString>
#include <QColor>
#include <QUuid>
#include <QJsonObject>
#include <QDebug>

int User::userCount = 1;

const int User::maxBid = INT32_MAX;

/**
 * @brief User::User Creates a new User with new Uuid
 * @param name when the name is "", a default name is generated
 * @param color
 * @param parent
 */
User::User(QString name, QColor color, QObject *parent) : QObject(parent), name(name), color(color)
{
	User::id = QUuid::createUuid();
	if(name=="")
	{
		name = tr("user ")+User::userCount++;
	}
}

User::~User()
{
	qDebug() << "User " << name << " (id:" << id << ") destroyed";
}

//User::User(QString name, QColor color, QUuid uuid, QObject *parent) : QObject(parent), name(name), color(color), id(uuid)
//{
//	if(name=="")
//	{
//		name = tr("user ")+User::userCount++;
//	}
//}

User::User(QObject *parent) : QObject(parent)
{

}

bool User::getHasBid() const
{
	return hasBid;
}

void User::setTimeStamp(unsigned long value)
{
	timeStamp = value;
}

QJsonObject User::toJSON()
{
	QJsonObject json;
	json.insert("name", getName());
	json.insert("color", getColor().name());
	json.insert("id", getId().toString());
	json.insert("bidding", getBidding());
	json.insert("points", getPoints());
	json.insert("hasBid", hasBid);

	QString timeStampString = QString("%1").arg(getTimeStamp());
	json.insert("lastBiddingTime", timeStampString);
	return json;
}

User* User::fromJSON(QJsonObject json)
{
	User* user = new User();
	user->setName(json.value("name").toString());
	user->setColor(QColor(json.value("color").toString()));
	user->id = QUuid(json.value("id").toString());
	user->setBidding(json.value("bidding").toInt());
	user->points = json.value("points").toInt();
	user->timeStamp = json.value("lastBiddingTime").toString().toULong();
	qDebug() << "fromJSON hasBid";
	user->hasBid = json.value("hasBid").toBool();
	return user;
}

void User::incrementPoints()
{
	points++;
	emit pointsChanged(id, points);
}

QString User::getName() const
{
	return name;
}
QColor User::getColor() const
{
	return color;
}
QUuid User::getId() const
{
	return id;
}
int User::getBidding() const
{
	return bidding;
}
int User::getPoints() const
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

/**
 * @brief User::setBidding
 * @param b = INT32_MAX to reset
 *
 */
void User::setBidding(int b)
{
	qDebug() << "User::setBidding(int " << b << ")" << name << id;
	bidding = b;
	hasBid = 1 < b && b < maxBid;
	if(hasBid)
	{
		timeStamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
	}
	else
	{
		timeStamp = 0;
	}
	qDebug() << hasBid;
	emit biddingChanged(id, b);
}

void User::addPoints(int p)
{
	points += p;
}

unsigned long User::getTimeStamp() const
{
	return timeStamp;
}

bool User::operator<(const User & u) const
{
	qDebug() << "User::operator<(const User & u) const";
	return this < &u;
}

bool User::operator<(const User * u) const
{
	//qDebug() << "User::operator<(const User * u) const";
	if(hasBid && u->hasBid)
	{
		return (getBidding() == u->getBidding() && getTimeStamp() < u->getTimeStamp()) || getBidding() < u->getBidding();
	}
	return hasBid || !u->hasBid;
}
