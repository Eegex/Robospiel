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
private:
	QVector<ZugKnoten*> children;
	PosKnoten* pos;
	Zug zug;
public:
	ZugKnoten(Direction move, int player, PosKnoten* pos);
	void addChild(ZugKnoten* child);

};

#endif // ZUGKNOTEN_H
