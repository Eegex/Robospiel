#ifndef NETWORKMODEL_H
#define NETWORKMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QJsonObject>
#include <QObject>
#include "user.h"

class NetworkModel : public QAbstractListModel
{
	Q_OBJECT
public:
	NetworkModel();
	void addTransmission(QJsonObject & transmission);
	void clear();
	int rowCount(const QModelIndex & parent) const;
	int columnCount(const QModelIndex & parent) const;
	QVariant data(const QModelIndex & index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	void setUsers(QVector<User *> * value);

private:
	int PlayerActionStringIndex(int a);
	QVector<QString> columns;
	QVector<QJsonObject> list;
	QVector<User*> * users;
	static QMap<int,QString> playerActionStrings;
};

#endif // NETWORKMODEL_H
