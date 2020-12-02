#ifndef KEYMAPPING_H
#define KEYMAPPING_H

#include <Qt>
#include <QVector>
#include "Direction.h"

class KeyMapping
{
public:
	KeyMapping(PlayerAction a, Qt::Key k);
	KeyMapping(PlayerAction a, QVector<Qt::Key> k);
	bool operator==(const Qt::Key k) const;
	bool operator==(const int code) const;
	PlayerAction getAction() const;

private:
	PlayerAction action;
	QVector<Qt::Key> keys;
};

#endif // KEYMAPPING_H
