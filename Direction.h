#ifndef DIRECTION_H
#define DIRECTION_H

#include <QString>
#include <QColor>

enum class Direction{none = 0x0, north = 0x1, east = 0x2, south = 0x4, west = 0x8};

enum PlayerAction{none = 0x0, movement = 0x10, movePlayerNorth = 17,movePlayerEast = 18,movePlayerSouth = 20,movePlayerWest = 24,
				  playerSwitch = 0x20, switchPlayerNorth = 33,switchPlayerEast = 34,switchPlayerSouth = 36,switchPlayerWest = 40,
				  bidding = 0x40, enterBidding = 65, sendBidding = 66, clearBidding = 68, skipTimer= 72,
				  other = 0x80, giveUp = 129, revert = 130, revertToBeginning= 132, freePlay = 136,
				  user = 0x100, newUser = 0x101, changedUsername = 0x102, changedUserColor = 260,
				  update = 0x200, completeUpdate = 0x201, boardUpdate = 0x202,
				  menuAction = 0x400, editBoard=0x401, nextTarget=0x402, setIdle=0x403,
				  intern=0x1000, syncRandomGenerators=0x1001
				 };
//the Intern subspace should be the end of this enum
#endif // DIRECTION_H
