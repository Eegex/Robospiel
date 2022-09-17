#include "posknoten.h"

PosKnoten* PosKnoten::getRoot()
{
    PosKnoten* parent = this->parent;
    while(parent != nullptr){
        parent = parent->parent;
    }
    return parent;

}

QPoint PosKnoten::getTileOfActivePlayer()
{
    QPoint activePlayerTile;
    PosKnoten* parent = this->parent;
    while(true){
        PosKnoten* newParent = parent->parent;
        if(newParent == nullptr){
            activePlayerTile = newParent->children.key(parent);
            break;
        }
        parent = newParent;
    }
    return activePlayerTile;

}

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
