#ifndef POSKNOTEN_H
#define POSKNOTEN_H

#include <QPoint>
#include <QVector>
#include <QMap>

/**
 * @brief Die erste Ebene im PosBaum ist der aktive Spieler
 */
class PosKnoten
{
	PosKnoten* parent;
	QMap<QPoint,PosKnoten*> children;
    PosKnoten *getRoot();
public:
    QPoint getTileOfActivePlayer();
	PosKnoten();
};

#endif // POSKNOTEN_H

