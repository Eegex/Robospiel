#include "keymapping.h"

KeyMapping::KeyMapping(PlayerAction a, Qt::Key k) : action(a)
{
	keys.append(k);
}

KeyMapping::KeyMapping(PlayerAction a, QVector<Qt::Key> k) : action(a), keys(k)
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
