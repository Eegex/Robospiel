#ifndef NETWORKDEBUGGER_H
#define NETWORKDEBUGGER_H

#include <QWidget>
#include <QTableView>
#include <QVBoxLayout>
#include "networkmodel.h"

class NetworkDebugger : public QWidget
{
	Q_OBJECT
public:
	NetworkDebugger(NetworkModel * model, QWidget * parent);

private:
	QTableView * view = new QTableView(this);
	QVBoxLayout * lay = new QVBoxLayout(this);

signals:

};

#endif // NETWORKDEBUGGER_H
