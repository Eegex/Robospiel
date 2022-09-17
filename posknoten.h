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
public:
	PosKnoten* parent = nullptr;
	QMap<int,PosKnoten*> children;
	PosKnoten *getRoot();
	static constexpr int maxBoardSize = 100; //TODO: evtl. hatte Dorothee recht und das war keine gute Idee, weil hier nicht die echte board-Size übergeben wird. :(
	static int pointToInt(QPoint p);
	static QPoint intToPoint(int i);
	QPoint getTileOfSeeker();
	PosKnoten();
	~PosKnoten();
	QMap<int, PosKnoten *> & getChildren();
};

#endif // POSKNOTEN_H

