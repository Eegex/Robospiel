#include "keymapping.h"

#include <QKeySequence>
#include <QDebug>

KeyMapping::KeyMapping(const KeyMapping &obj)
{
    action=obj.action;
    keys=obj.keys;
}

KeyMapping::KeyMapping(PlayerAction a, Qt::Key k) : action(a)
{
	keys.append(k);
}

KeyMapping::KeyMapping(PlayerAction a, QVector<Qt::Key> k) : action(a), keys(k)
{

}

KeyMapping::KeyMapping(PlayerAction a) : action(a)
{

}

bool KeyMapping::operator==(const Qt::Key k) const
{
	return keys.contains(k);
}

bool KeyMapping::operator==(const int code) const
{
	return keys.contains(static_cast<Qt::Key>(code));
}

PlayerAction KeyMapping::getAction() const
{
	return action;
}

QVector<Qt::Key> KeyMapping::getKeys() const
{
    return keys;
}

/**
 * @brief KeyMapping::addKey
 * @param key
 * @return false if the key was already contained and nothing changed
 */
bool KeyMapping::addKey(Qt::Key key)
{
    if(keys.contains(key))
    {
        return false;
    }
    keys.append(key);
    return true;
}

void KeyMapping::removeKey(Qt::Key key)
{
    keys.removeAll(key);
}

void KeyMapping::replaceKeyAt(int index, Qt::Key key)
{
    keys.replace(index, key);
}
