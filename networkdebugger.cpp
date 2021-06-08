#include "networkdebugger.h"

NetworkDebugger::NetworkDebugger(NetworkModel * model) : QWidget()
{
	lay->addWidget(view);
	view->setModel(model);
}
