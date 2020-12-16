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
    QVector<Qt::Key> getKeys() const;

    bool addKey(Qt::Key key);
    void replaceKeyAt(int index, Qt::Key key);
    void removeKey(Qt::Key key);
    KeyMapping(const KeyMapping &obj);
private:
	PlayerAction action;
	QVector<Qt::Key> keys;
};

#endif // KEYMAPPING_H
