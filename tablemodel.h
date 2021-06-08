#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <gamecontroll.h>
#include "user.h"

class TableModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	TableModel(QObject *parent=nullptr);

	// QAbstractItemModel interface
public:
	void update();
	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	void setUser(QVector<User *> * newUsers);
    void updateUsers();
	User * findUser(QUuid id);
};

#endif // TABLEMODEL_H
