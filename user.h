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
	static const int maxBid;
	User(QString name, QColor color, QObject *parent=nullptr);
	~User();
	QString getName() const;
	QColor getColor() const;
	QUuid getId() const;
	int getBidding() const;
	int getPoints() const;
	void setName(QString n);
	void setColor(QColor c);
	void setBidding(int b);
	void addPoints(int p);
	unsigned long getTimeStamp() const;
	QJsonObject toJSON();
	static User* fromJSON(QJsonObject json);
	void incrementPoints();
	void setTimeStamp(unsigned long value);
	bool operator <(const User & u) const;
	bool getHasBid() const;

private:
	User(QObject *parent=nullptr);
	QString name;
	QColor color;
	QUuid id;
	int bidding = maxBid;
	int points = 0;
	unsigned long timeStamp;
	bool hasBid = false;
	static int userCount;

signals:
	void biddingChanged(QUuid id, int bid);
	void pointsChanged(QUuid id, int points);


};

#endif // USER_H
