#include "solver.h"

bool Solver::goalHit(PosKnoten *pos)
{
    QPoint activePlayerTile = pos->getTileOfActivePlayer();
    QPoint goalTile = board->goal->getPosition();
    return activePlayerTile == goalTile;

}


Solver::Solver(QObject *parent) : QObject{parent}
{

}
