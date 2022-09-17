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
	ZugKnoten* parent = nullptr;
	QVector<ZugKnoten*> children;
	Zug zug;
public:
    PosKnoten* pos;
	ZugKnoten();
	ZugKnoten(Zug z, PosKnoten* pos);
	~ZugKnoten();
	void addChild(ZugKnoten* child);

};

#endif // ZUGKNOTEN_H
