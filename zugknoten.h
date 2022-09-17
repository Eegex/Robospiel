#ifndef ZUGKNOTEN_H
#define ZUGKNOTEN_H

#include <QVector>
#include "Direction.h"
#include "posknoten.h"

class ZugKnoten
{
public:
	struct Zug
	{
		int player;
		Direction d;
	};
	ZugKnoten* parent = nullptr;
	QVector<ZugKnoten*> children;
	Zug zug;
	PosKnoten* pos;

	ZugKnoten();
	ZugKnoten(Zug z, PosKnoten* pos);
	~ZugKnoten();
	void addChild(ZugKnoten* child);

	ZugKnoten * getParent() const;
	void setParent(ZugKnoten * newParent);
};

#endif // ZUGKNOTEN_H
