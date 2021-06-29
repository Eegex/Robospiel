#include "networkdebugger.h"
#include <QHeaderView>

NetworkDebugger::NetworkDebugger(NetworkModel * model) : QWidget()
{
	setWindowFlag(Qt::WindowStaysOnTopHint);
	view->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	view->horizontalHeader()->setStretchLastSection(true);
	lay->addWidget(view);
	view->setModel(model);
	resize(800,400);
	move(0,0);
	update();

    connect(model, &NetworkModel::layoutChanged, this, [&](){
        view->scrollToBottom();
    });
}
