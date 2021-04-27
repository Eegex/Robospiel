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
    User(QString name, QColor color, QObject *parent=nullptr);
	QString getName();
	QColor getColor();
	QUuid getId();
	int getBidding();
	int getPoints();
	void setName(QString n);
	void setColor(QColor c);
	void setBidding(int b);
	void addPoints(int p);
    unsigned long getTimeStamp() const;
	bool hasBid = false;
    QJsonObject toJSON();
    static User* fromJSON(QJsonObject json);
    void incrementPoints();
    void setTimeStamp(unsigned long value);

private:
    User(QObject *parent=nullptr);
	QString name;
	QColor color;
	QUuid id;
	int bidding = 99;
	int points = 0;
	unsigned long timeStamp;
	static int userCount;

signals:
	void biddingChanged(QUuid id, int bid);
    void pointsChanged(int points);


};

#endif // USER_H
