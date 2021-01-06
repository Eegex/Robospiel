#ifndef DIRECTION_H
#define DIRECTION_H

enum class Direction{none = 0x0, north = 0x1, east = 0x2, south = 0x4, west = 0x8};

enum PlayerAction{none = 0x0, movement = 0x10, movePlayerNorth = 0x11,movePlayerEast = 0x12,movePlayerSouth = 0x14,movePlayerWest = 0x18,
						playerSwitch = 0x20, switchPlayerNorth = 0x21,switchPlayerEast = 0x22,switchPlayerSouth = 0x24,switchPlayerWest = 0x28,
						bidding = 0x40, enterBidding = 0x41, sendBidding = 0x42, clearBidding = 0x44,
						other = 0x80, giveUp = 0x81, revert = 0x82, revertToBeginning= 0x84, freePlay = 0x88};

#endif // DIRECTION_H
