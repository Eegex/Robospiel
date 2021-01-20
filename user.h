#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QUuid>
#include <QDateTime>

class User : public QObject
{
	Q_OBJECT
public:
	User(QString name, QColor color, QObject *parent);
	User(QString name, QColor color, QUuid uuid, QObject *parent);
	QString getName();
	QColor getColor();
	QUuid getId();
	int getBidding();
	int getPoints();
	void setName(QString n);
	void setColor(QColor c);
	void setBidding(int b);
	void addPoints(int p);
	QDateTime getLastBidding() const;

private:
	QString name;
	QColor color;
	QUuid id;
	int bidding;
	int points;
	QDateTime lastBidding;

	static int userCount;

signals:


};

#endif // USER_H
