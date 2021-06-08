#include "networkdebugger.h"

NetworkDebugger::NetworkDebugger(NetworkModel * model, QWidget *parent) : QWidget(parent)
{
	lay->addWidget(view);
	view->setModel(model);
}
