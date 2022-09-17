#ifndef SOLVER_H
#define SOLVER_H

#include <QObject>
#include "posknoten.h"
#include "zugknoten.h"
#include "board.h"

class Solver : public QObject
{
	Q_OBJECT
	Board* board = nullptr;
	PosKnoten* posBaum = 0;
	ZugKnoten* zugBaum = 0;

	void generateChildren(ZugKnoten* alt);
	PosKnoten* posExists(QVector<QPoint> murmeln); // nullptr falls treffer, sonst Rückgabe in ZugKnoten verwenden
	bool goalHit(PosKnoten* pos);
public:
	explicit Solver(QObject *parent = nullptr);
	void solve(Board* b);
	QVector<ZugKnoten::Zug> exportPath();

signals:

};

#endif // SOLVER_H
