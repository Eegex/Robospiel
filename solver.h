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
	QVector<ZugKnoten*> frontier;
	void generateChildren(ZugKnoten* alt); //Dorothee
	PosKnoten* posExists(QVector<QPoint> players); // Luca  // nullptr falls treffer, sonst Rückgabe in ZugKnoten verwenden
	bool goalHit(PosKnoten* pos); // Nora
	ZugKnoten* goal = nullptr;
public:
	explicit Solver(QObject *parent = nullptr);
	void solve(Board* b); // Annalena
	QVector<ZugKnoten::Zug> exportPath(); // Jan

signals:
	void solved();
};

#endif // SOLVER_H
