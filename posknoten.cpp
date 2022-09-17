#include "posknoten.h"

PosKnoten::PosKnoten()
{

}

PosKnoten::~PosKnoten()
{
	QVector<PosKnoten*> toDelete = children.values();
	while(!toDelete.isEmpty())
	{
		delete toDelete.takeFirst();
	}
	children.clear();
}
