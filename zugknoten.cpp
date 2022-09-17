#include "zugknoten.h"

ZugKnoten::ZugKnoten()
{

}

ZugKnoten::~ZugKnoten()
{
	while(!children.isEmpty())
	{
		delete children.takeFirst();
	}
}
