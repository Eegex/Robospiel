#ifndef DIRECTION_H
#define DIRECTION_H

#include <QString>
#include <QMap>
#include <QColor>

enum class Direction{none = 0x0, north = 0x1, east = 0x2, south = 0x4, west = 0x8};

//add string representation to networkmodel.cpp
enum PlayerAction{none = 0x0, movement = 0x10, movePlayerNorth = 0x11,movePlayerEast = 0x12,movePlayerSouth = 0x14,movePlayerWest = 0x18,
				  playerSwitch = 0x20, switchPlayerNorth = 0x21,switchPlayerEast = 0x22,switchPlayerSouth = 0x24,switchPlayerWest = 0x28,
				  bidding = 0x40, enterBidding = 0x41, sendBidding = 0x42, clearBidding = 0x44,
				  other = 0x80, giveUp = 0x81, revert = 0x82, revertToBeginning = 0x84, freePlay = 0x88,
				  user = 0x100, newUser = 0x101, changedUsername = 0x102, changedUserColor = 0x104, userLeft=0x108,
				  update = 0x200, completeUpdate = 0x201, boardUpdate = 0x202, skipTimer = 0x48,
				  menuAction = 0x400, editBoard = 0x401, nextTarget = 0x402, setIdle = 0x404,
				  intern = 0x1000, syncRandomGenerators = 0x1001, registerClient = 0x1002
				 };

//the Intern subspace should be the end of this enum
#endif // DIRECTION_H
