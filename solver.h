#ifndef SOLVER_H
#define SOLVER_H

#include <QObject>
#include <QMutex>
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
	PosKnoten* makeUniquePosKnoten(QVector<QPoint> players); // Luca  // nullptr falls treffer, sonst Rückgabe in ZugKnoten verwenden
	bool goalHit(PosKnoten* pos); // Nora
	ZugKnoten* goal = nullptr;
	QMutex deletable;
	int depth = 0;
	int generated = 0;
	int explored = 0;
	QVector<int> width;
	ZugKnoten* levelEdge = nullptr;
	void exportPath(QVector<ZugKnoten::Zug>& path); // Jan
public:
	explicit Solver(QObject *parent = nullptr);
public slots:
	void solveBoard(Board* b, QVector<ZugKnoten::Zug>& path); // Annalena

private:
	QVector<QPoint> makeMove(ZugKnoten::Zug zug, ZugKnoten* alt);
	void printBoard(PosKnoten* k);
};

#endif // SOLVER_H
