#ifndef DIRECTION_H
#define DIRECTION_H

#include <QString>
#include <QMap>
#include <QColor>

enum class Direction{none = 0x0, north = 0x1, east = 0x2, south = 0x4, west = 0x8};

//add string representation to networkmodel.cpp
enum PlayerAction{none = 0x0,
				  movement = 0x100, movePlayerNorth = 0x101,movePlayerEast = 0x102,movePlayerSouth = 0x104,movePlayerWest = 0x108,
				  playerSwitch = 0x200, switchPlayerNorth = 0x201,switchPlayerEast = 0x202,switchPlayerSouth = 0x204,switchPlayerWest = 0x208,
				  bidding = 0x400, enterBidding = 0x401, sendBidding = 0x402, clearBidding = 0x404,
				  other = 0x800, giveUp = 0x801, revert = 0x802, revertToBeginning = 0x804, freePlay = 0x808, vote = 0x810,
                  user = 0x1000, newUser = 4097, changedUsername = 0x1002, changedUserColor = 0x1004, userLeft=0x1008,changeActiveUser = 0x1010, changedTimerTime = 0x1100,resetPoints = 0x1200,
				  update = 0x2000, completeUpdate = 0x2001, boardUpdate = 0x2002,
				  menuAction = 0x4000, blockBoard = 0x4001, editBoard = 0x4002, nextTarget = 0x4004, setIdle = 0x4008,
				  intern = 0x8000, syncRandomGenerators = 0x8001, registerClient = 0x8002
				 };

//the Intern subspace should be the end of this enum
#endif // DIRECTION_H
