#ifndef KEYMAPPING_H
#define KEYMAPPING_H

#include <Qt>
#include <QVector>
#include <QLabel>
#include <QPushButton>
#include <QSet>
#include "Direction.h"

class KeyMapping
{
/**
  * Qt::Key_F31 = swipe up
  * Qt::Key_F32 = swipe right
  * Qt::Key_F33 = swipe down
  * Qt::Key_F34 = swipe left
  * Qt::Key_F35 = hover
  *
*/
public:
	KeyMapping(PlayerAction a, Qt::Key k);
	KeyMapping(PlayerAction a, QVector<Qt::Key> k);
	KeyMapping(PlayerAction);
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
