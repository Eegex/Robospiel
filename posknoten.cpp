#include "posknoten.h"

QMap<int, PosKnoten *> & PosKnoten::getChildren()
{
	return children;
}

PosKnoten* PosKnoten::getRoot()
{
	PosKnoten* parent = this->parent;
	while(parent != nullptr)
	{
		parent = parent->parent;
	}
	return parent;

}

int PosKnoten::pointToInt(QPoint p)
{
	return p.x() * maxBoardSize + p.y();
}

QPoint PosKnoten::intToPoint(int i)
{
	return QPoint(i / maxBoardSize,i % maxBoardSize);
}

QPoint PosKnoten::getTileOfSeeker()
{
	QPoint seekerTile;

	PosKnoten* knot = this;
	PosKnoten* parent = this->parent;
	while(parent->parent)
	{
		parent = parent->parent;
		knot = knot->parent;
	}
	seekerTile = intToPoint(parent->children.key(knot));
	return seekerTile;

}

PosKnoten::PosKnoten()
{

}

PosKnoten::~PosKnoten()
{
	QList<PosKnoten*> toDelete = children.values();
	while(!toDelete.isEmpty())
	{
		delete toDelete.takeFirst();
	}
	children.clear();
}
