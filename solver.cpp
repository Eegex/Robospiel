#include "solver.h"
#include <QDebug>


Solver::Solver(QObject *parent) : QObject{parent}
{

}

void Solver::solve(Board * b)
{
	if(board)
	{
		delete posBaum;
		delete zugBaum;
	}
	board = b;
	posBaum = new PosKnoten();
	zugBaum = new ZugKnoten();
	ZugKnoten* current = zugBaum;
	while(!goalHit(current->pos))
	{
		generateChildren(currentZug);
		assert(!frontier.isEmpty());
		current = frontier.takeFirst();
		qDebug() << "Frontier size: " << frontier.size();
	}

}

QVector<ZugKnoten::Zug> Solver::exportPath()
{
	QVector<ZugKnoten::Zug> path;
	if(goal)
	{
		ZugKnoten* zug = goal;
		while(zug)
		{
			path.prepend(zug->zug);
			zug = zug->parent;
		}
	}
	return path;
}

PosKnoten * Solver::posExists(QVector<QPoint> players)
{

	//Wenn der PosKnoten bereits existiert für die aktuellen Spieler, dann gib einen Nullptr zurück.
	bool containsAllPositions = true;
	//Speichere die aktuelle Ebene im Graphen zwischen, falls wir irgendwo aufhören und da Krams einfügen müssen.
	PosKnoten * currentParent = posBaum;
	//Gehe über alle "Ebenen" drüber
	for(QPoint player : players)
	{
		if(!posBaum->getChildren()->contains(player))
		{
			containsAllPositions = false;
		}
		else
		{
			currentParent = posBaum->getChildren()->find(player).value();
		}
	}
	//Wenn die aktuelle Position schon existiert, gib einen Nullzeiger zurück.
	if(containsAllPositions)
	{
		return nullptr;
	}

	//Ansonsten erstelle einen neuen Knoten mit den entsprechenden Daten und gib diesen zurück.
	posBaum->getChildren()->insert(players.last(), currentParent);
	return posBaum->getChildren()->find(players.last()).value();
}

