#include "gamecontroll.h"
#include "votekickdelegate.h"

VotekickDelegate::VotekickDelegate(QSize size, QWidget * parent): ButtonDelegate(parent)
{
	customSizeHint = size;
}

QSize VotekickDelegate::getCustomSizeHint(QPoint p) const
{
	if(p.y()==3)
	{
		return customSizeHint;
	}
	else
	{
		return {0,0};
	}
}

void VotekickDelegate::updateSizeHint(int logicalIndex, int /*oldSize*/, int newSize)
{
	if(logicalIndex==0)
	{
		customSizeHint.setWidth(newSize);
	}
}

int VotekickDelegate::hasButton(QPoint p) const
{
	if(p.x()==3)
	{
		if(GameControll::getUserIndexById(GameControll::getLocalUser()->getId())==p.y()) //own row
		{
			return 1;
		}
		return 2;
	}
	return 0;
}

bool VotekickDelegate::isButtonDisabled(QPoint p) const
{
	User* u = GameControll::getUsers()->at(p.y());
	GameControll::getLocalUser()->votekickInfo();
	return hasButton(p) && GameControll::getLocalUser()->getVotekick(u->getId());
}
