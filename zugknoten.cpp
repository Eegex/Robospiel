#include "zugknoten.h"

ZugKnoten * ZugKnoten::getParent() const
{
	return parent;
}

void ZugKnoten::setParent(ZugKnoten * newParent)
{
	parent = newParent;
}

ZugKnoten::ZugKnoten()
{

}

ZugKnoten::ZugKnoten(Zug z, PosKnoten * pos): pos(pos), zug(z)
{

}

ZugKnoten::~ZugKnoten()
{
	while(!children.isEmpty())
	{
		delete children.takeFirst();
	}
}

void ZugKnoten::addChild(ZugKnoten * child)
{
	children.append(child);
	child->parent = this;
}
